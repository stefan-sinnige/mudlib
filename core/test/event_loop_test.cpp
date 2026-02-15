/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/event_loop.h"
#include "mud/core/event_mechanism.h"
#include "mud/core/object.h"
#include "mud/test.h"
#include "test_mechanism.h"
#include <future>
#include <list>
#include <ostream>
#include <type_traits>

/* clang-format off */

CONTEXT(public mud::core::object)
    /* Constructor initialised for each scenario run */
    context() {
        /* Add a special handle type */
        mud::core::event_loop::global().add_mechanism(
                mud::core::handle::type_t::__TEST);

        /* Initialise the event */
        event.event = mud::core::event(
              mud::core::uuid(),
              event.itc.handle(),
              mud::core::event::signal_type::READING);
        event.future_task = event.promise_task.get_future();

        /* Initialise the other event */
        other.event = mud::core::event(
              mud::core::uuid(),
              other.itc.handle(),
              mud::core::event::signal_type::READING);
        other.future_task = other.promise_task.get_future();
    }

    /* Destructor after each scenario */
    ~context() {
        if (event_thread.joinable()) {
            mud::core::event_loop::global().terminate();
            event_thread.join();
        }
    }

    /* The event handler */
    void on_event(const mud::core::message&) {
        event.ch = event.itc.read();
        ++event.calls;
        event.promise_task.set_value();
    }

    /* The other event handler */
    void on_other_event(const mud::core::message&) {
        other.ch = other.itc.read();
        ++other.calls;
        other.promise_task.set_value();
    }

    /* An event handler that terminates the event loop */
    void on_event_terminate(const mud::core::message&) {
        event.ch = event.itc.read();
        ++event.calls;
        event.promise_task.set_value();
        mud::core::event_loop::global().terminate();
    }

    /* The time to give asynchronous operations some time */
    const std::chrono::milliseconds timeout = std::chrono::milliseconds(10);

    /* The thread running the event loop */
    std::thread event_thread;

    /* Event data structure. This contains the event itself and the associated
     * data. */
    struct event_data {
        test_resource itc;
        mud::core::event event;
        char ch = '\0';
        int calls = 0;
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
          ctx.event_thread = std::thread([&ctx]() {
            mud::core::event_loop::global().loop();
          });
          mud::core::event_loop::global().ready().wait();
      })
  DEFINE_GIVEN("An event is added",
      [](context& ctx) {
          ctx.attach(ctx.event.event.topic(), &context::on_event);
          mud::core::event_loop::global().add(std::move(ctx.event.event));
      })
  DEFINE_GIVEN("The same event is added again",
      [](context& ctx) {
          ctx.attach(ctx.event.event.topic(), &context::on_event);
          mud::core::event_loop::global().add(std::move(ctx.event.event));
      })
  DEFINE_GIVEN("Another event is added",
      [](context& ctx) {
          ctx.attach(ctx.other.event.topic(), &context::on_other_event);
          mud::core::event_loop::global().add(std::move(ctx.other.event));
      })
  DEFINE_WHEN ("The event is removed",
        [](context& ctx) {
            mud::core::event_loop::global().remove(ctx.event.event);
        })
  DEFINE_WHEN ("The other event is removed",
        [](context& ctx) {
            mud::core::event_loop::global().remove(ctx.other.event);
        })
  DEFINE_WHEN("The event is triggered",
      [](context& ctx) {
          ctx.event.itc.write('A');
      })
  DEFINE_WHEN ("The event loop is requested to terminate",
        [](context& ctx) {
          mud::core::event_loop::global().terminate();
      })
  DEFINE_WHEN ("The event loop is terminated",
      [](context& ctx) {
          if (ctx.event_thread.joinable()) {
              mud::core::event_loop::global().terminate();
              ctx.event_thread.join();
          }
      })
  DEFINE_WHEN ("The event loop is restarted",
      [](context& ctx) {
          if (ctx.event_thread.joinable()) {
              mud::core::event_loop::global().terminate();
              ctx.event_thread.join();
          }
          ctx.event_thread = std::thread([&ctx]() {
            mud::core::event_loop::global().loop();
          });
          mud::core::event_loop::global().ready().wait();
      })
  DEFINE_THEN ("The event loop is running",
      [](context& ctx) {
          ASSERT(true, ctx.event_thread.joinable());
      })
  DEFINE_THEN("The event loop is terminated",
      [](context& ctx) {
          if (ctx.event_thread.joinable()) {
              ctx.event_thread.join();
          }
          ASSERT(false, ctx.event_thread.joinable());
      })
  DEFINE_THEN("The event handler was not called",
      [](context& ctx) {
          std::this_thread::sleep_for(ctx.timeout);
          ASSERT(0, ctx.event.calls);
      })
  DEFINE_THEN("The other event handler was not called",
      [](context& ctx) {
          std::this_thread::sleep_for(ctx.timeout);
          ASSERT(0, ctx.other.calls);
      })
  DEFINE_THEN("The event handler was called exactly once",
      [](context& ctx) {
          ASSERT(true, ctx.event.future_task.valid());
          ctx.event.future_task.wait();
          ASSERT(1, ctx.event.calls);
      })
  DEFINE_THEN("The other event handler was called exactly once",
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
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::event_loop>::value);
        })
     AND ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::core::event_loop,
                  mud::core::event_loop>::value);
        })

  SCENARIO("Event loop terminates by request")
    GIVEN("A running event loop")
    WHEN ("The event loop is requested to terminate")
    THEN ("The event loop is terminated")

  SCENARIO("Event loop calls an event handler when an event is triggered")
    GIVEN("A running event loop")
     AND ("An event is added")
    WHEN ("The event is triggered")
    THEN ("The event handler was called exactly once")

  SCENARIO("Event loop does not call a removed event handler")
    GIVEN("A running event loop")
     AND ("An event is added")
    WHEN ("The event is removed")
     AND ("The event is triggered")
    THEN ("The event handler was not called")

  SCENARIO("Event loop is called once when the same event is added twice")
    GIVEN("A running event loop")
     AND ("An event is added")
     AND ("The same event is added again")
    WHEN ("The event is triggered")
    THEN ("The event handler was called exactly once")

  SCENARIO("Event loop is only called for triggered events")
    GIVEN("A running event loop")
     AND ("An event is added")
     AND ("Another event is added")
    WHEN ("The event is triggered")
    THEN ("The event handler was called exactly once")
     AND ("The other event handler was not called")

  SCENARIO("Event loop can restart after being terminated")
    GIVEN("A running event loop")
    WHEN ("The event loop is terminated")
     AND ("The event loop is restarted")
    THEN ("The event loop is running")

  SCENARIO("Event loop terminates by request from a handler routine")
    GIVEN("A running event loop")
      AND("An event is added that terminates the event loop", 
      [](context& ctx) {
          ctx.attach(ctx.event.event.topic(), &context::on_event_terminate);
          mud::core::event_loop::global().add(std::move(ctx.event.event));
        })
    WHEN ("The event is triggered")
    THEN ("The event handler was called exactly once")
     AND ("The event loop is terminated")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
