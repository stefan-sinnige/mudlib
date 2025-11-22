#include "mud/io/ip.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* clang-format off */

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
            std::string str = ctx.sample<std::string>("dotted-decimal");
            ctx.addr = mud::io::ip::address(str); })
     WHEN ("Converted to an IP address", [](context&){})
     THEN ("The value is converted to an <address>", [](context& ctx) {
            uint32_t expected = mud::io::ip::to_network_order(ctx.sample<uint32_t>("address"));
            ASSERT((uint32_t)ctx.addr , expected); })
     SAMPLES(std::string, uint32_t)
         HEADINGS("dotted-decimal", "address")
         SAMPLE("0.0.0.0", 0x00000000)
         SAMPLE("1.2.3.4", 0x01020304)
         SAMPLE("192.168.255.254", 0xC0A8FFFE)
     END_SAMPLES()

  SCENARIO("Conversion from network-order address to dotted-decimal")
    GIVEN ("An <address>", [](context& ctx) {
            uint32_t addr = mud::io::ip::to_network_order(ctx.sample<uint32_t>("address"));
            ctx.addr = mud::io::ip::address(addr); })
     WHEN ("Converted to an IP address", [](context&){})
     THEN ("The value is converted to a <dotted-decimal>", [](context& ctx) {
            std::string exp = ctx.sample<std::string>("dotted-decimal");
            ASSERT(ctx.addr.str(), exp); })
     SAMPLES(uint32_t, std::string)
         HEADINGS("address", "dotted-decimal")
         SAMPLE(0x00000000, "0.0.0.0")
         SAMPLE(0x01020304, "1.2.3.4")
         SAMPLE(0xC0A8FFFE, "192.168.255.254")
     END_SAMPLES()

  SCENARIO("Conversion from node name to network-order address")
    GIVEN ("A <node-name>", [](context& ctx) {
            std::string node = ctx.sample<std::string>("node-name");
            ctx.addr = mud::io::ip::address(node); })
     WHEN ("Converted to an IP address", [](context&){})
     THEN ("The value is converted to an <address>", [](context& ctx) {
            std::string exp = ctx.sample<std::string>("dotted-decimal");
            ASSERT(ctx.addr.str(), exp); })
     SAMPLES(std::string, std::string)
         HEADINGS("node-name", "dotted-decimal")
         /* These should be single public static IP addresses for the tests to
          * have longevity. Note that these will pass through a DNS lookup. */
         SAMPLE("time-a-g.nist.gov", "129.6.15.28")
     END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
