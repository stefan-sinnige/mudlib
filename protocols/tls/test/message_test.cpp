#include "mud/tls/message.h"
#include "mud/test.h"
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Input string stream */
    std::istringstream istr;

    /* Output string stream */
    std::ostringstream ostr;
END_CONTEXT()

FEATURE("TLS Message")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An TLS Message", [](context&){ });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("An TLS Message type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::tls::message>::value);
        })
    THEN ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::tls::message>::value);
        })
    THEN ("The type is assignable",
        [](context& ctx) {
            ASSERT(true, std::is_assignable<
                  mud::tls::message,
                  mud::tls::message>::value);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
