#include "mud/test.h"
#include "mud/html/char_data.h"
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

    /* The char_data */
    mud::html::char_data doc;

END_CONTEXT()

FEATURE("Character Data")
      DEFINE_GIVEN("An empty char_data",
        [](context& ctx) {
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Character Data type traits")
    GIVEN("A char_data type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::html::char_data>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::html::char_data>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::html::char_data>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::html::char_data>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::html::char_data>::value);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
