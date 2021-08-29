#include "mud/ui/application.h"
#include "mud/ui/window.h"
#include "mud/test.h"
#include <future>
#include <memory>
#include <type_traits>

extern int g_delay;

/* *INDENT-OFF* */

/* The task queue for the test-cases to run the application object on the
 * main thread.  */
extern mud::core::simple_task_queue g_app_queue;

CONTEXT()
    // Constructor, executed before each scenario run
    context()
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
            std::this_thread::sleep_for(std::chrono::seconds(g_delay));
        }
        auto future = mud::ui::application::instance().terminate();
        if (future.valid())
        {
            future.wait();
        }
    }

    /* The status of the window */
    std::future<void> future_window;

    /* The root window */
    mud::ui::window root_window;
END_CONTEXT()

FEATURE("Window")

  // Pre-defined steps
  DEFINE_GIVEN("A root window",
      [](context& ctx) {
      })
  DEFINE_WHEN("The root window is shown",
      [](context& ctx) {
          ctx.future_window = ctx.root_window.show();
          ASSERT(true, ctx.future_window.valid());
          ctx.future_window.wait();
      })
  DEFINE_THEN("The root window is displayed",
      [](context& ctx) {
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("A window type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::ui::window>::value);
        })
     AND ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::ui::window>::value);
        })
     AND ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::ui::window>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::ui::window>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::ui::window>::value);
        })

    SCENARIO("A root window is created and shown")
      GIVEN("A root window")
      WHEN ("The root window is shown")
      THEN ("The root window is displayed")

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
