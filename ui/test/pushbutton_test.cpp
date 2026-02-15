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

#include "mud/test.h"
#include "mud/ui/application.h"
#include "mud/ui/pushbutton.h"
#include "mud/ui/window.h"
#include <future>
#include <memory>
#include <type_traits>

extern int g_delay;

/* clang-format off */

/* The task queue for the test-cases to run the application object on the
 * main thread.  */
extern mud::core::simple_task_queue g_app_queue;

CONTEXT()
    // Constructor, executed before each scenario run
    context()
        : control(root_window), mouse_calls(0)
    {
        // Schedule the application to run the loop on the main thread
        mud::core::simple_task tsk([]() {
              mud::ui::application::instance().loop();
        });
        g_app_queue.push(std::move(tsk));
    }

    // Destructor, executed after each scenario run
    ~context() {
        if (g_delay > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(g_delay));
        }
        auto future = mud::ui::application::instance().terminate();
        if (future.valid())
        {
            future.wait();
        }
    }

    /** The status of the window */
    std::future<void> future_window;

    /** The main window */
    mud::ui::window root_window;

    /** The pushbutton control */
    mud::ui::pushbutton control;

    /** The status of the simulated event */
    std::future<void> future_event;

    /** The number of times the pushbutton mouse-event has been called */
    int mouse_calls;
END_CONTEXT()

FEATURE("PushButton")

  // Pre-defined steps
  DEFINE_GIVEN("A window with a pushbutton",
      [](context& ctx){
          ctx.control.property<mud::ui::position>(10,10);
          ctx.control.property<mud::ui::size>(80,25);
          ctx.control.property<mud::ui::text>("Press Me");
          ctx.control.event([&ctx](const mud::ui::event::mouse& ev) {
              ++ctx.mouse_calls;
          });
      })
  DEFINE_WHEN ("The window is shown",
      [](context& ctx){
          ctx.future_window = ctx.root_window.show();
          ASSERT(true, ctx.future_window.valid());
          ctx.future_window.wait();
      })
  DEFINE_WHEN ("The pushbutton is pressed",
      [](context& ctx){
          mud::ui::event::mouse ev(ctx.control);
          ctx.future_event = mud::ui::application::instance().inject(ev);
          if (ctx.future_event.valid())
          {
              ctx.future_event.wait();
          }
      })
  DEFINE_THEN ("The pushbutton is displayed",
      [](context&){
      })
  DEFINE_THEN ("The pushbutton event has been called",
      [](context& ctx){
          ASSERT(1, ctx.mouse_calls);
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("A pushbutton type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::ui::pushbutton>::value);
        })
     AND ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::ui::pushbutton>::value);
        })
     AND ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::ui::pushbutton>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::ui::pushbutton>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::ui::pushbutton>::value);
        })

  SCENARIO("A pushbutton is created and shown")
    GIVEN("A window with a pushbutton")
    WHEN ("The window is shown")
    THEN ("The pushbutton is displayed")

  SCENARIO("A pushbutton is pressed and the event is handled")
    GIVEN("A window with a pushbutton")
    WHEN ("The window is shown")
    AND  ("The pushbutton is pressed")
    THEN ("The pushbutton event has been called")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
