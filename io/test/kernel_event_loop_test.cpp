#include "kernel_event_loop_test.h"
#include "mud/io/kernel_event_loop.h"
#include "mud/io/pipe.h"
#include <future>
#include <type_traits>

CPPUNIT_TEST_SUITE_REGISTRATION(KernelEventLoopTest);

const std::chrono::milliseconds timeout = std::chrono::milliseconds(20);

void
KernelEventLoopTest::setUp()
{
}

void
KernelEventLoopTest::tearDown()
{
}

void
KernelEventLoopTest::TypeTraits()
{
    // Given A type 'event_loop'
    // When  I query the tpe information
    // Then  The type is default constructible
    //  And  the type is not copy constructible
    //  And  the type is not assignable

    bool trait;
    trait = std::is_default_constructible<
            mud::io::kernel_event_loop>::value;
    CPPUNIT_ASSERT_EQUAL(true, trait);
    trait = std::is_copy_constructible<
            mud::io::kernel_event_loop>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
    trait = std::is_assignable<
            mud::io::kernel_event_loop,
            mud::io::kernel_event_loop>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
}

void
KernelEventLoopTest::ThreadTerminate()
{
    // Given An event loop
    // When  Another thread terminates the loop
    // Then  The event loop is terminated

    // Run the event-loop as an asynchronous operation. We use this, rather
    // than std::thread, as it allows the test to use timeouts.
    mud::io::kernel_event_loop loop;
    std::future<void> future = std::async(std::launch::async, [&loop]() {
        loop.loop();
    });
    std::future_status status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Terminate the event-loop
    loop.terminate();
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::ready);
}

void
KernelEventLoopTest::HandlerTerminate()
{
    // Given An event loop
    // When  An event handler terminates the loop
    // Then  The event loop is terminated

    // Run the event-loop as an asynchronous operation. We use this, rather
    // than std::thread, as it allows the test to use timeouts.
    mud::io::kernel_event_loop loop;
    std::future<void> future = std::async(std::launch::async, [&loop]() {
        loop.loop();
    });
    std::future_status status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Create a pipe and register an event handler associated to it to
    // terminate the event-loop when something is available on the pipe.
    int calls = 0;
    mud::io::pipe pipe;
    loop.register_handler(pipe.read_handle(),
    [&loop, &calls, &pipe]() {
        char ch;
        pipe.istr() >> ch;
        ++calls;
        loop.terminate();
    });
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Send something to the pipe. The loop should have terminated.
    pipe.ostr() << 'C' << std::flush;
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::ready);
    CPPUNIT_ASSERT_EQUAL((int)1, calls);
}

void
KernelEventLoopTest::HandlerRegistration()
{
    // Given An event loop
    // When  An event handler is registrated
    //  And  The event is triggered
    // Then  The event loop trigger the handler once

    // Run the event-loop as an asynchronous operation. We use this, rather
    // than std::thread, as it allows the test to use timeouts.
    mud::io::kernel_event_loop loop;
    std::future<void> future = std::async(std::launch::async, [&loop]() {
        loop.loop();
    });
    std::future_status status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Create a pipe and register an event handler associated to it. Keep
    // track of the number of calls the handler is executed.
    int calls = 0;
    mud::io::pipe pipe;
    loop.register_handler(pipe.read_handle(),
    [&calls, &pipe]() {
        char ch;
        pipe.istr() >> ch;
        ++calls;
    });
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Send something to the pipe. The event handler should be called.
    pipe.ostr() << 'C' << std::flush;
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL((int)1, calls);

    // Terminate
    loop.terminate();
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::ready);
}

void
KernelEventLoopTest::HandlerDeregistration()
{
    // Given An event loop
    //  And  A registered event handler
    // When  The event handler is deregistrated
    //  And  The event is triggered
    // Then  The event loop does not trigger the handler

    // Run the event-loop as an asynchronous operation. We use this, rather
    // than std::thread, as it allows the test to use timeouts.
    mud::io::kernel_event_loop loop;
    std::future<void> future = std::async(std::launch::async, [&loop]() {
        loop.loop();
    });
    std::future_status status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Create a pipe and register an event handler associated to it. Keep
    // track of the number of calls the handler is executed.
    int calls = 0;
    mud::io::pipe pipe;
    loop.register_handler( pipe.read_handle(),
    [&calls, &pipe]() {
        char ch;
        pipe.istr() >> ch;
        ++calls;
    });
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Deregister the event handler
    loop.deregister_handler(pipe.read_handle());

    // Send something to the pipe. The event handler should not be called.
    pipe.ostr() << 'C' << std::flush;
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL((int)0, calls);

    // Terminate
    loop.terminate();
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::ready);
}

void
KernelEventLoopTest::HandlerDoubleRegistration()
{
    // Given An event loop
    //  And  A registered event handler
    // When  Another event handler is registrated
    //  And  The event is triggered
    // Then  The event loop will only trigger the last registered handler

    // Run the event-loop as an asynchronous operation. We use this, rather
    // than std::thread, as it allows the test to use timeouts.
    mud::io::kernel_event_loop loop;
    std::future<void> future = std::async(std::launch::async, [&loop]() {
        loop.loop();
    });
    std::future_status status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Create a pipe and register an event handler associated to it. Keep
    // track of the number of calls the handler is executed.
    int calls_1 = 0;
    mud::io::pipe pipe;
    loop.register_handler(pipe.read_handle(),
    [&calls_1, &pipe]() {
        char ch;
        pipe.istr() >> ch;
        ++calls_1;
    });

    // Register another handler for the same event handle
    int calls_2 = 0;
    loop.register_handler(pipe.read_handle(),
    [&calls_2, &pipe]() {
        char ch;
        pipe.istr() >> ch;
        ++calls_2;
    });
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::timeout);

    // Send something to the pipe. The second event handler should be called.
    pipe.ostr() << 'C' << std::flush;
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL((int)0, calls_1);
    CPPUNIT_ASSERT_EQUAL((int)1, calls_2);

    // Terminate
    loop.terminate();
    status = future.wait_for(timeout);
    CPPUNIT_ASSERT_EQUAL(status, std::future_status::ready);
}

/* vi: set ai ts=4 expandtab: */
