#include "mud/core/endian.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The integral values */
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
END_CONTEXT()

FEATURE("Endian")
      DEFINE_GIVEN("A native integral value",
        [](context& ctx) {
            ctx.u16 = 0x0123;
            ctx.u32 = 0x01234567;
            ctx.u64 = 0x0123456789ABCDEF;
        })
     DEFINE_THEN ("The values are in big-endian",
        [](context& ctx) {
            uint8_t* value;
            value = (uint8_t*)&ctx.u16;
            ASSERT(0x01, value[0]);
            ASSERT(0x23, value[1]);
            value = (uint8_t*)&ctx.u32;
            ASSERT(0x01, value[0]);
            ASSERT(0x23, value[1]);
            ASSERT(0x45, value[2]);
            ASSERT(0x67, value[3]);
            value = (uint8_t*)&ctx.u64;
            ASSERT(0x01, value[0]);
            ASSERT(0x23, value[1]);
            ASSERT(0x45, value[2]);
            ASSERT(0x67, value[3]);
            ASSERT(0x89, value[4]);
            ASSERT(0xAB, value[5]);
            ASSERT(0xCD, value[6]);
            ASSERT(0xEF, value[7]);
        })
     DEFINE_THEN ("The values are in little-endian",
        [](context& ctx) {
            uint8_t* value;
            value = (uint8_t*)&ctx.u16;
            ASSERT(0x23, value[0]);
            ASSERT(0x01, value[1]);
            value = (uint8_t*)&ctx.u32;
            ASSERT(0x67, value[0]);
            ASSERT(0x45, value[1]);
            ASSERT(0x23, value[2]);
            ASSERT(0x01, value[3]);
            value = (uint8_t*)&ctx.u64;
            ASSERT(0xEF, value[0]);
            ASSERT(0xCD, value[1]);
            ASSERT(0xAB, value[2]);
            ASSERT(0x89, value[3]);
            ASSERT(0x67, value[4]);
            ASSERT(0x45, value[5]);
            ASSERT(0x23, value[6]);
            ASSERT(0x01, value[7]);
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Conversion from native to big endian")
    GIVEN("A native integral value")
    WHEN ("Converting to big-endian",
        [](context& ctx){
          {
            uint16_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::big,
                    ctx.u16);
            ctx.u16 = value;
          }
          {
            uint32_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::big,
                    ctx.u32);
            ctx.u32 = value;
          }
          {
            uint64_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::big,
                    ctx.u64);
            ctx.u64 = value;
          }
        })
    THEN ("The values are in big-endian")

  SCENARIO("Conversion from native to little endian")
    GIVEN("A native integral value")
    WHEN ("Converting to little-endian",
        [](context& ctx){
          {
            uint16_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::little,
                    ctx.u16);
            ctx.u16 = value;
          }
          {
            uint32_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::little,
                    ctx.u32);
            ctx.u32 = value;
          }
          {
            uint64_t value = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::little,
                    ctx.u64);
            ctx.u64 = value;
          }
        })
    THEN ("The values are in little-endian")

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
