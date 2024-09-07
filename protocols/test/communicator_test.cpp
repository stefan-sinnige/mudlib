#include "mud/protocols/communicator.h"
#include "mud/test.h"
#include "mock_communicators.h"
#include "mock_device.h"
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

FEATURE("Communicator")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An end-communicator", [](context&){ });
  DEFINE_GIVEN("A layered-communicator", [](context&){ });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits of communicator")
    GIVEN("An communicator type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::protocols::communicator<mock::device>>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::protocols::communicator<mock::device>>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::protocols::communicator<mock::device>,
                  mud::protocols::communicator<mock::device>>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::protocols::communicator<mock::device>>::value);
        })
    THEN ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::protocols::communicator<mock::device>>::value);
        })

  SCENARIO("Type traits of end_communicator")
    GIVEN("An communicator type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::protocols::end_communicator<mock::device>>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::protocols::end_communicator<mock::device>>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::protocols::end_communicator<mock::device>,
                  mud::protocols::end_communicator<mock::device>>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::protocols::end_communicator<mock::device>>::value);
        })
    THEN ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::protocols::end_communicator<mock::device>>::value);
        })

  SCENARIO("Type traits of layered_communicator")
    GIVEN("An communicator type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::protocols::layered_communicator<mock::device>>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::protocols::layered_communicator<mock::device>>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::protocols::layered_communicator<mock::device>,
                  mud::protocols::layered_communicator<mock::device>>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::protocols::layered_communicator<mock::device>>::value);
        })
    THEN ("The type is move-assignable",
        [](context& ctx) {
            /*
            ASSERT(true, std::is_move_assignable<
                  mud::protocols::layered_communicator<mock::device>>::value);
            */
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
