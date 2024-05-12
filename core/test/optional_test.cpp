#include "mud/core/optional.h"
#include "mud/test.h"
#include <type_traits>

/* clang-format off */

/*
 * Sample class for testing with an optional reference.
 */
class Sample
{
};

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {}

    /* Destructor after each scenario */
    ~context() {}
END_CONTEXT()

FEATURE("Optional Reference")
  END_DEFINES()

  /*
   * The scenarios
   */
  SCENARIO("Optional reference type traits")
    GIVEN("An optional reference type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context&) {
            ASSERT(true, std::is_default_constructible<
                    mud::core::optional_ref<Sample>>::value);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
