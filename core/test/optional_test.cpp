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

/*
int
main(int argc, char** argv)
{
    std::cout << "Valid object\n";
    optional_ref<A> o;
    o = func_object();
    print(o);

    std::cout << "Copying a valid object\n";
    optional_ref<A> copied = o;
    print(copied);
    print(o);

    std::cout << "Moving a valid object\n";
    optional_ref<A> moved = std::move(o);
    print(moved);
    print(o);

    std::cout << "No object\n";
    o = func_none();
    print(o);
    return 0;
}
*/

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
