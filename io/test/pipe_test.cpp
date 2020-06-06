#include "mud/io/pipe.h"
#include "mud/test.h"
#include <string>
#include <type_traits>
#include <unistd.h>

/* *INDENT-OFF* */

namespace /* unnamed */ {

struct context
{
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* A pipe */
    mud::io::pipe pipe;
};

}

FEATURE("Uni-directional pipes", context)

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("A pipe",
      [](context& ctx) {})

  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("A pipe type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::pipe>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::pipe>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::io::pipe,
                  mud::io::pipe>::value);
        })

  SCENARIO("Writing and reading formatted data")
    GIVEN("A pipe")
    WHEN ("Formatted data is written to the pipe",
        [](context& ctx) {
            char     c = 'F';
            uint32_t i = 73618;
            ctx.pipe.ostr() << c << i << std::flush;
        })
    THEN ("The same formatted data can be read from the pipe",
        [](context& ctx) {
            char        c = 0;
            uint32_t    i  = 0;
            ctx.pipe.istr() >> c >> i;
            ASSERT('F', c);
            ASSERT((uint32_t)73618, i);
        })

  SCENARIO("Writing and reading binary data")
    GIVEN("A pipe")
    WHEN ("Binary data is written to the pipe",
        [](context& ctx) {
            uint8_t block[] = {0x01, 0x92, 0x00, 0xF4};
            ctx.pipe.ostr().write((const char*)block, sizeof(block))
                << std::flush;
        })
    THEN ("The same binary data can be read from the pipe",
        [](context& ctx) {
            uint8_t block[4];
            memset(block, 0, sizeof(block));
            ctx.pipe.istr().read((char*)block, sizeof(block));
            ASSERT((uint8_t)0x01, block[0]);
            ASSERT((uint8_t)0x92, block[1]);
            ASSERT((uint8_t)0x00, block[2]);
            ASSERT((uint8_t)0xF4, block[3]);
        })

  SCENARIO("Nothing to read on a non-blocking pipe")
    GIVEN("A pipe")
    WHEN ("There is no data available",
        [](context& ctx) {})
    THEN ("Reading from the pipe will fail",
        [](context& ctx) {
          char c;
          ctx.pipe.istr().read(&c, 1);
          ASSERT(true, ctx.pipe.istr().eof());
        })
END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
