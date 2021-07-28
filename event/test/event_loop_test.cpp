#if defined(WINDOWS) && defined(NATIVE)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #define RECVFROM_CAST (char*)
    #define SENDTO_CAST   (const char*)
    #ifndef MUDLIB_SSIZE_T
        #define MUDLIB_SSIZE_T
        typedef long ssize_t;
    #endif
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <unistd.h>
    #define RECVFROM_CAST (void*)
    #define SENDTO_CAST   (const void*)
#endif
#include "mud/event/event_loop.h"
#include "mud/test.h"
#include <future>
#include <ostream>
#include <type_traits>

/* *INDENT-OFF* */

class test_resource
{
public:
    /* Constructor, creating a UDP socket. */
    test_resource()
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        addr.sin_family = AF_INET;
        addr.sin_port = 0;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* Create socket */
        int sd = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (sd < 0) {
            throw std::system_error(errno, std::system_category(), "socket");
        }

        /* Bind to port 0 to allocate a free port */
        if (::bind(sd, (struct sockaddr*) &addr, len) < 0) {
            throw std::system_error(errno, std::system_category(), "bind");
        }

        /* Get address we've just bound to */
        len = sizeof(_addr);
        ::memset(&_addr, 0, len);
        if (::getsockname(sd, (struct sockaddr*)&_addr, &len) != 0) {
            throw std::system_error(errno, std::system_category(), "getsockname");
        }

        /* Save the handle */
        _handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle(sd));
    }

    /* Destructor */
    ~test_resource()
    {
        if (_handle != nullptr)
        {
#if defined(WINDOWS) && defined(NATIVE)
            ::closesocket(mud::core::internal_handle<int>(_handle));
#else
            ::close(mud::core::internal_handle<int>(_handle));
#endif
            _handle.reset(nullptr);
        }
    }

    /* Write a character */
    void write()
    {
        /* Write over UDP */
        struct sockaddr_in write_addr;
        ::memcpy(&write_addr, &_addr, sizeof(write_addr));
        char ch = 'S';
        int sd = mud::core::internal_handle<int>(_handle);
        ssize_t nwrite = ::sendto(sd, &ch, 1, 0,
            (struct sockaddr*)&write_addr, sizeof(write_addr));
        if (nwrite < 0) {
            throw std::system_error(errno, std::system_category(), "sendto");
        }
    }

    // Read a character */
    void read()
    {
        /* Read from UDP */
        struct sockaddr_in recv_addr;
        ::memcpy(&recv_addr, &_addr, sizeof(recv_addr));
        socklen_t len = sizeof(recv_addr);
        char ch;
        int sd = mud::core::internal_handle<int>(_handle);
        ssize_t nread = ::recvfrom(sd, &ch, 1, 0,
                (struct sockaddr*)&recv_addr, &len);
        if (nread <= 0) {
            throw std::system_error(errno, std::system_category(), "recvfrom");
        }
    }

    /* Return the handle */
    const std::unique_ptr<mud::core::handle>& handle() const {
        return _handle;
    }

private:
    /* The assigned address */
    struct sockaddr_in _addr;

    /* The UDP socket handle. */
    std::unique_ptr<mud::core::handle> _handle;
};

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
    mud::event::event_loop event_loop;

    /* The status of the event loop */
    std::future<void> future;

    /* A handle for testing purposes (inter-thread communiation) */
    test_resource itc;

    /* A counter for the number of calls executed in a handler. */
    int calls;

    /* A counter for the number of calls executed in a different handler. */
    int other_calls;
END_CONTEXT()

FEATURE("Event loop")

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
              ctx.event_loop.register_handler(std::move(mud::event::event(
                ctx.itc.handle(),
                mud::event::event::signal_type::READING,
                [&ctx]() {
                    ctx.itc.read();
                    ++ctx.calls;
                    return mud::event::event::return_type::CONTINUE;
                })));
          })
  DEFINE_GIVEN("Another registered handler that handles an event",
      [](context& ctx) {
              ctx.event_loop.register_handler(std::move(mud::event::event(
                ctx.itc.handle(),
                mud::event::event::signal_type::READING,
                [&ctx]() {
                    ctx.itc.read();
                    ++ctx.other_calls;
                    return mud::event::event::return_type::CONTINUE;
                })));
          })
  DEFINE_WHEN("The event is triggered",
      [](context& ctx) {
              ctx.itc.write();
          })
  DEFINE_WHEN ("The event loop is requested to terminate",
        [](context& ctx) {
            ctx.event_loop.terminate();
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
                  mud::event::event_loop>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::event::event_loop>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::event::event_loop,
                  mud::event::event_loop>::value);
        })

  SCENARIO("Event loop terminates by request")
    GIVEN("A running event loop")
    WHEN ("The event loop is requested to terminate")
    THEN ("The event loop is terminated")

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
            mud::event::event event(ctx.itc.handle());
            ctx.event_loop.deregister_handler(std::move(event));
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

  SCENARIO("Event loop terminates by request from a handler routine")
    GIVEN("A running event loop")
      AND("A registered handler that terminates the loop",
          [](context& ctx) {
            ctx.event_loop.register_handler(std::move(mud::event::event(
              ctx.itc.handle(),
              mud::event::event::signal_type::READING,
              [&ctx]() {
                ctx.itc.read();
                ++ctx.calls;
                ctx.event_loop.terminate();
                return mud::event::event::return_type::REMOVE;
              })));
        })
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")
     AND ("The event loop is terminated")

  SCENARIO("Event loop can restart after being terminated")
    GIVEN("A running event loop")
    WHEN ("The event loop is terminated",
      [](context& ctx) {
          ctx.event_loop.terminate();
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(status, std::future_status::ready);
      })
     AND ("The event loop is restarted",
      [](context& ctx) {
          ctx.future = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.loop();
          });
          std::future_status status = ctx.future.wait_for(ctx.timeout);
      })
    THEN ("The event loop is running",
      [](context& ctx) {
          std::future_status status = ctx.future.wait_for(ctx.timeout);
          ASSERT(status, std::future_status::timeout);
      })

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
