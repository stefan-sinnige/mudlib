#include "mud/io/kernel_event_loop.h"
#include "mud/io/pipe.h"
#include "mud/test.h"
#include <future>
#include <ostream>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        calls = 0;
        other_calls = 0;
    }

    /* Destructor after each scenario */
    ~context() {
        event_loop.terminate();
        if (future.valid()) {
          std::future_status status = future.wait_for(timeout);
        }
    }

    /* The time to give asynchronous operations some time */
    const std::chrono::milliseconds timeout = std::chrono::milliseconds(20);

    /* The event loop */
    mud::io::kernel_event_loop event_loop;

    /* The status of the event loop */
    std::future<void> future;

    /* A pipe for inter thread communication. */
    mud::io::pipe pipe;

    /* A counter for the number of calls executed in a handler. */
    int calls;

    /* A counter for the number of calls executed in a different handler. */
    int other_calls;
END_CONTEXT()

FEATURE("Kernel event loop")

  /*
   * The pre-defined Gherkin steps.
   */

  DEFINE_GIVEN("A running event loop",
      [](context& ctx) {
          /* Run the event loop in an asynchronous operation. */
          ctx.future = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.loop();
          });
          std::future_status status = ctx.future.wait_for(ctx.timeout);
      })
  DEFINE_GIVEN("A registered handler that handles an event",
      [](context& ctx) {
              ctx.event_loop.register_handler(ctx.pipe.read_handle(),
                mud::io::kernel_event_loop::readiness_t::READING,
                [&ctx]() {
                  char ch;
                  ctx.pipe.istr() >> ch;
                  ++ctx.calls;
              });
          })
  DEFINE_GIVEN("Another registered handler that handles an event",
      [](context& ctx) {
              ctx.event_loop.register_handler(ctx.pipe.read_handle(),
                mud::io::kernel_event_loop::readiness_t::READING,
                [&ctx]() {
                  char ch;
                  ctx.pipe.istr() >> ch;
                  ++ctx.other_calls;
              });
          })
  DEFINE_WHEN("The event is triggered",
      [](context& ctx) {
              ctx.pipe.ostr() << 'T' << std::flush;
          })
  DEFINE_THEN("The event loop is terminated",
      [](context& ctx) {
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(status, std::future_status::ready);
      })
  DEFINE_THEN("The event handler is not called",
      [](context& ctx) {
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(0, ctx.calls);
     })
  DEFINE_THEN("The event handler is called exactly once",
      [](context& ctx) {
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(1, ctx.calls);
     })
  DEFINE_THEN("The other event handler is called exactly once",
      [](context& ctx) {
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(1, ctx.other_calls);
     })

  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An event loop type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::kernel_event_loop>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::kernel_event_loop>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::io::kernel_event_loop,
                  mud::io::kernel_event_loop>::value);
        })

  SCENARIO("Event loop calls an event handler when an event is triggered")
    GIVEN("A running event loop")
      AND("A registered handler that handles an event")
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")

  SCENARIO("Event loop does not call a deregistered event handler")
    GIVEN("A running event loop")
     AND ("A registered handler that handles an event")
    WHEN ("The event handler is deregistered",
        [](context& ctx) {
            ctx.event_loop.deregister_handler(ctx.pipe.read_handle(),
                mud::io::kernel_event_loop::readiness_t::READING);
        })
     AND ("The event is triggered")
    THEN ("The event handler is not called")

  SCENARIO("Event loop calls only one event handler when two are registered")
    GIVEN("A running event loop")
      AND("A registered handler that handles an event")
      AND("Another registered handler that handles an event")
    WHEN ("The event is triggered")
    THEN ("The event handler is not called")
     AND ("The other event handler is called exactly once")

  SCENARIO("Event loop terminates by request from another thread")
    GIVEN("A running event loop")
    WHEN ("A different thread terminates the event loop",
        [](context& ctx) {
            ctx.event_loop.terminate();
        })
    THEN ("The event loop is terminated")

  SCENARIO("Event loop terminates by request from a handler routine")
    GIVEN("A running event loop")
      AND("A registered handler that terminates the loop",
          [](context& ctx) {
            ctx.event_loop.register_handler(ctx.pipe.read_handle(),
              mud::io::kernel_event_loop::readiness_t::READING,
              [&ctx]() {
                char ch;
                ctx.pipe.istr() >> ch;
                ++ctx.calls;
                ctx.event_loop.terminate();
            });
        })
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")
     AND ("The event loop is terminated")

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
