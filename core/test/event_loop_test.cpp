#include "mud/core/event_loop.h"
#include "mud/core/event_mechanism.h"
#include "mud/test.h"
#include "test_mechanism.h"
#include <future>
#include <list>
#include <ostream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        event.ch = other.ch = '\0';
        event.calls = other.calls = 0;
        event.future_task = event.promise_task.get_future();
        other.future_task = other.promise_task.get_future();
    }

    /* Destructor after each scenario */
    ~context() {
        future_loop = event_loop.terminate();
        if (future_loop.valid()) {
            future_loop.wait();
        }
        if (future_thread.valid()) {
            future_thread.wait();
        }
    }

    /* The time to give asynchronous operations some time */
    const std::chrono::milliseconds timeout = std::chrono::milliseconds(10);

    /* The event loop */
    mud::core::event_loop event_loop;

    /* The status of the thread running the event loop */
    std::future<void> future_thread;

    /* The status of the event loop */
    std::shared_future<void> future_loop;

    /* A handle for testing purposes (inter-thread communiation) */
    test_resource itc;

    /* Event data structure. This contains the event itself and the associated
     * data. */
    struct event_data {
        mud::core::event event;
        char ch;
        int calls;
        std::promise<void> promise_task;
        std::future<void> future_task;
    };

    /* An event */
    struct event_data event;

    /* Another event */
    struct event_data other;
END_CONTEXT()

FEATURE("Event loop")

  /*
   * The pre-defined Gherkin steps.
   */

  DEFINE_GIVEN("A running event loop",
      [](context& ctx) {
          ctx.future_thread = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.add_mechanism(mud::core::handle::type_t::__TEST);
              ctx.event_loop.loop();
          });
          std::this_thread::sleep_for(ctx.timeout);
      })
  DEFINE_GIVEN("A restarted running event loop",
      [](context& ctx) {
          ctx.future_thread = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.add_mechanism(mud::core::handle::type_t::__TEST);
              ctx.event_loop.loop();
          });
          std::this_thread::sleep_for(ctx.timeout);
          ctx.future_loop = ctx.event_loop.terminate();
          ASSERT(true, ctx.future_loop.valid());
          ctx.future_loop.wait();
          ASSERT(true, ctx.future_thread.valid());
          ctx.future_thread.wait();
          ctx.future_thread = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.loop();
          });
          std::this_thread::sleep_for(ctx.timeout);
      })
  DEFINE_GIVEN("A registered handler that handles an event",
      [](context& ctx) {
          ctx.event.event = mud::core::event(
              ctx.itc.handle(),
              mud::core::event::signal_type::READING,
              [&ctx]() {
                  ctx.event.ch = ctx.itc.read();
                  ++ctx.event.calls;
                  ctx.event.promise_task.set_value();
                  return mud::core::event::return_type::CONTINUE;
          });
          ctx.event_loop.register_handler(ctx.event.event);
      })
  DEFINE_GIVEN("The same event is registered again",
      [](context& ctx) {
          ctx.event_loop.register_handler(ctx.event.event);
      })
  DEFINE_GIVEN("Another registered handler that handles an event",
      [](context& ctx) {
          ctx.other.event = mud::core::event(
              ctx.itc.handle(),
              mud::core::event::signal_type::READING,
              [&ctx]() {
                  ctx.other.ch = ctx.itc.read();
                  ++ctx.other.calls;
                  ctx.other.promise_task.set_value();
                  return mud::core::event::return_type::CONTINUE;
          });
          ctx.event_loop.register_handler(ctx.other.event);
      })
  DEFINE_WHEN("The event is triggered",
      [](context& ctx) {
          ctx.itc.write('A');
      })
  DEFINE_WHEN ("The event loop is requested to terminate",
        [](context& ctx) {
          ctx.future_loop = ctx.event_loop.terminate();
      })
  DEFINE_WHEN ("The event loop is terminated",
      [](context& ctx) {
          ctx.future_loop = ctx.event_loop.terminate();
          ASSERT(true, ctx.future_loop.valid());
          ctx.future_loop.wait();
          ASSERT(true, ctx.future_thread.valid());
          ctx.future_thread.wait();
      })
  DEFINE_WHEN ("The event loop is restarted",
      [](context& ctx) {
          ctx.future_thread = std::async(std::launch::async, [&ctx]() {
              ctx.event_loop.loop();
          });
          std::this_thread::sleep_for(ctx.timeout);
      })
  DEFINE_THEN("The event loop is terminated",
      [](context& ctx) {
          ASSERT(true, ctx.future_loop.valid());
          ctx.future_loop.wait();
          ASSERT(true, ctx.future_thread.valid());
          ctx.future_thread.wait();
      })
  DEFINE_THEN("The event handler is not called",
      [](context& ctx) {
          std::this_thread::sleep_for(ctx.timeout);
          ASSERT(0, ctx.event.calls);
      })
  DEFINE_THEN("The event handler is called exactly once",
      [](context& ctx) {
          ASSERT(true, ctx.event.future_task.valid());
          ctx.event.future_task.wait();
          ASSERT(1, ctx.event.calls);
      })
  DEFINE_THEN("The other event handler is called exactly once",
      [](context& ctx) {
          ASSERT(true, ctx.other.future_task.valid());
          ctx.other.future_task.wait();
          ASSERT(1, ctx.other.calls);
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
                  mud::core::event_loop>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::event_loop>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::core::event_loop,
                  mud::core::event_loop>::value);
        })

  SCENARIO("A stopped event loop returns an invalid termination future")
    GIVEN("A stopped event loop", [](context&){})
    WHEN ("The event loop is requested to terminate")
    THEN ("The termination future is invalid",
      [](context& ctx) {
        ASSERT(false, ctx.future_loop.valid());
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
            ctx.event_loop.deregister_handler(ctx.event.event);
        })
     AND ("The event is triggered")
    THEN ("The event handler is not called")

  SCENARIO("Event loop is called once when the same event is registered twice")
    GIVEN("A running event loop")
      AND("A registered handler that handles an event")
      AND("The same event is registered again")
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")

  SCENARIO("Event loop is called for all registered handlers")
    GIVEN("A running event loop")
      AND("A registered handler that handles an event")
      AND("Another registered handler that handles an event")
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")
     AND ("The other event handler is called exactly once")

  SCENARIO("Event loop terminates by request from a handler routine")
    GIVEN("A running event loop")
      AND("A registered handler that terminates the loop",
          [](context& ctx) {
            ctx.event.event = mud::core::event(
                ctx.itc.handle(),
                mud::core::event::signal_type::READING,
                [&ctx]() {
                    ctx.event.ch = ctx.itc.read();
                    ++ctx.event.calls;
                    ctx.future_loop = ctx.event_loop.terminate();
                    ctx.event.promise_task.set_value();
                    return mud::core::event::return_type::REMOVE;
            });
            ctx.event_loop.register_handler(ctx.event.event);
        })
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")
     AND ("The event loop is terminated")

  SCENARIO("Event loop can restart after being terminated")
    GIVEN("A running event loop")
    WHEN ("The event loop is terminated")
     AND ("The event loop is restarted")
    THEN ("The event loop is running",
      [](context& ctx) {
          ASSERT(true, ctx.future_thread.valid());
          std::future_status status = ctx.future_thread.wait_for(ctx.timeout);
          ASSERT(status, std::future_status::timeout);
      })

  SCENARIO("Event loop cannot be started while it is already running")
    GIVEN("A running event loop")
    WHEN ("The event loop is running again", [](context& ctx){})
    THEN ("An assertion is thrown",
      [](context& ctx) {
        ASSERT_THROW(std::invalid_argument, ctx.event_loop.loop());
      })

  SCENARIO("Event loop that is restarted calls an event handler when triggered")
    GIVEN("A restarted running event loop")
     AND ("A registered handler that handles an event")
    WHEN ("The event is triggered")
    THEN ("The event handler is called exactly once")
/*
  SCENARIO("Registering an event for a mechanism that is not loaded fails")
    GIVEN("A running event loop")
    WHEN ("An event is registered for a mechanism that is not loaded",
      [](context&) {})
    THEN ("An assertion is thrown",
      [](context& ctx) {
        ASSERT_THROW(std::invalid_argument,
                     ctx.event_loop.register_handler(ctx.event));
      })
*/
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
