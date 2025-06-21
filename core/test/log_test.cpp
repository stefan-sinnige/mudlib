#include "mud/core/log.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Placeholders for application log severity and target information */
    mud::core::log::severity_t  app_severity;
    mud::core::log::target_t    app_targets;
    std::filesystem::path       app_file_path;

    /* Placeholders for internal log severity and target information */
    mud::core::log::severity_t  internal_severity;
    mud::core::log::target_t    internal_targets;
    std::filesystem::path       internal_file_path;
END_CONTEXT()

FEATURE("Log")
    DEFINE_GIVEN("A default setting", [](context& ctx) {})
    DEFINE_WHEN ("The application settings are examined",
        [](context& ctx) {
            ctx.app_severity  = mud::core::log::severity();
            ctx.app_targets   = mud::core::log::targets();
            ctx.app_file_path = mud::core::log::file_path();
    })
    DEFINE_WHEN ("The internal settings are examined",
        [](context& ctx) {
            ctx.internal_severity  = mud::core::log::internal_severity();
            ctx.internal_targets   = mud::core::log::internal_targets();
            ctx.internal_file_path = mud::core::log::internal_file_path();
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Default application settings")
    GIVEN("A default setting")
     WHEN("The application settings are examined")
     THEN("The log level is info",
        [](context& ctx){
          ASSERT(mud::core::log::severity_t::info, ctx.app_severity);
        })
      AND("The log target is none",
        [](context& ctx){
          ASSERT(mud::core::log::target_t::none, ctx.app_targets);
        })
      AND("The log file is empty",
        [](context& ctx){
          ASSERT(std::filesystem::path(), ctx.app_file_path);
        })

  SCENARIO("Default internal settings")
    GIVEN("A default setting")
     WHEN("The internal settings are examined")
     THEN("The log level is info",
        [](context& ctx){
          ASSERT(mud::core::log::severity_t::info, ctx.internal_severity);
        })
     AND("The log target is none",
        [](context& ctx){
          ASSERT(mud::core::log::target_t::none, ctx.internal_targets);
        })
     AND("The log file is empty",
        [](context& ctx){
          ASSERT(std::filesystem::path(), ctx.internal_file_path);
        })

  SCENARIO("Setting application settings")
    GIVEN("Specific application settings",
        [](context& ctx){
            mud::core::log::severity(mud::core::log::severity_t::debug);
            mud::core::log::targets(
                    mud::core::log::target_t::console |
                    mud::core::log::target_t::file,
                    std::filesystem::path("/tmp/test.log"));
        })
     WHEN("The application settings are examined")
     THEN("The log level is debug",
        [](context& ctx){
          ASSERT(mud::core::log::severity_t::debug, ctx.app_severity);
        })
      AND("The log target is console and file",
        [](context& ctx){
          mud::core::log::target_t expected = 
                  mud::core::log::target_t::console | 
                  mud::core::log::target_t::file;
          ASSERT(expected, ctx.app_targets);
        })
      AND("The log file is set",
        [](context& ctx){
          ASSERT(std::filesystem::path("/tmp/test.log"), ctx.app_file_path);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
