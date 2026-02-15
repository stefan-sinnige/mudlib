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

#include "mud/core/task.h"
#include "mud/test.h"
#include "mud/ui/application.h"
#include "mud/ui/window.h"
#include <future>
#include <memory>
#include <type_traits>

/* clang-format off */

/* The task queue for the test-cases to run the application object on the
 * main thread.  */
extern mud::core::simple_task_queue g_app_queue;

CONTEXT()
    // Constructor, executed before each scenario run
    context()
    {
    }

    // Destructor, executed after each scenario run
    ~context() {
        mud::ui::application::instance().terminate();
        if (future_app.valid())
        {
            future_app.wait();
        }
    }

    /* The status of the application */
    std::future<void> future_app;

    /* The status of the window */
    std::future<void> future_window;

    /* The root window */
    mud::ui::window root_window;
END_CONTEXT()

FEATURE("Application")

  // Pre-defined steps
  DEFINE_GIVEN("A running application",
      [](context& ctx) {
          // Schedule the application to run the loop on the main thread
          mud::core::simple_task tsk([]() {
              mud::ui::application::instance().loop();
          });
          ctx.future_app = tsk.get_future();
          g_app_queue.push(std::move(tsk));
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      })
  DEFINE_WHEN("The application is requested to terminate",
      [](context& ctx) {
        mud::ui::application::instance().terminate();
      })
  DEFINE_THEN("The application is terminated",
      [](context& ctx) {
        ASSERT(true, ctx.future_app.valid());
        ctx.future_app.wait();
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An applicationt type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::ui::application>::value);
        })
     AND ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::ui::application>::value);
        })
     AND ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::ui::application>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::ui::application>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::ui::application>::value);
        })

    SCENARIO("An application terminates upon request")
      GIVEN("A running application")
      WHEN ("The application is requested to terminate")
      THEN ("The application is terminated")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
