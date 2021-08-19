#include "mud/ui/application.h"
#include "mud/ui/window.h"
#include "mud/test.h"
#include <future>
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

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
          ctx.future_app = std::async(std::launch::async, [&ctx]() {
              mud::ui::application::instance().loop();
          });
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

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
