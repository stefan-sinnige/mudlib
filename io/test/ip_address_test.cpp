#include "mud/io/ip.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    mud::io::ip::address addr;
END_CONTEXT()

FEATURE("IP Address")

  // Pre-defined steps
  DEFINE_GIVEN("Any address", [](context& ctx) {
        ctx.addr = mud::io::ip::address(); })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An IP address type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::ip::address>::value);
        })
    THEN ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::io::ip::address>::value);
        })
    THEN ("The type is assignable",
        [](context& ctx) {
            ASSERT(true, std::is_assignable<
                  mud::io::ip::address,
                  mud::io::ip::address>::value);
        })

  SCENARIO("Conversion from dotted-decimal to network-order address")
    GIVEN ("A <dotted-decimal>", [](context& ctx) {
            std::string str = ctx.sample().entry<std::string>("dotted-decimal");
            ctx.addr = mud::io::ip::address(str); })
     WHEN ("Converted to an IP address", [](context&){})
     THEN ("The value is converted to an <address>", [](context& ctx) {
            in_addr_t expected = htonl(ctx.sample().entry<uint32_t>("address"));
            ASSERT((in_addr_t)ctx.addr , expected); })
     SAMPLES("dotted-decimal", "address")
         SAMPLE("0.0.0.0", 0x00000000)
         SAMPLE("1.2.3.4", 0x01020304)
         SAMPLE("192.168.255.254", 0xC0A8FFFE)
     END_SAMPLES()

  SCENARIO("Conversion from network-order address to dotted-decimal")
    GIVEN ("An <address>", [](context& ctx) {
            in_addr_t addr = htonl(ctx.sample().entry<uint32_t>("address"));
            ctx.addr = mud::io::ip::address(addr); })
     WHEN ("Converted to an IP address", [](context&){})
     THEN ("The value is converted to a <dotted-decimal>", [](context& ctx) {
            std::string exp = ctx.sample().entry<std::string>("dotted-decimal");
            ASSERT(ctx.addr.str(), exp); })
     SAMPLES("address", "dotted-decimal")
         SAMPLE(0x00000000, "0.0.0.0")
         SAMPLE(0x01020304, "1.2.3.4")
         SAMPLE(0xC0A8FFFE, "192.168.255.254")
     END_SAMPLES()

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
