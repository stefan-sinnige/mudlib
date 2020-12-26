#include "mud/io/udp.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    // Constructor, executed before each scenario run
    context()
        : server(nullptr), client(nullptr)
    {
    }

    // Destructor, executed after each scenario run
    ~context() {
        delete server;
        delete client;
    }

    mud::io::udp::socket *server;
    mud::io::udp::socket *client;
END_CONTEXT()

FEATURE("UDP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A UDP server is waiting for inbound connection",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.server = new mud::io::udp::socket;
        ctx.server->bind(mud::io::udp::endpoint(localhost, 52618));
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("A UDP socket type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::udp::socket>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::udp::socket>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::udp::socket>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::udp::socket>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::udp::socket>::value);
        })

    SCENARIO("Writing and reading binary data")
      GIVEN("A UDP server is waiting for inbound connection")
      WHEN ("A client writes binary data",
          [](context& ctx) {
              std::string localhost("127.0.0.1");
              uint8_t block[] = {0x01, 0x92, 0x00, 0xF4};
              ctx.client = new mud::io::udp::socket();
              ctx.client->ostr(mud::io::udp::endpoint(localhost, 52618))
                        .write((const char*)block, sizeof(block))
                  << std::flush;
          })
      THEN ("The same binary data can be read by the server",
          [](context& ctx) {
              uint8_t block[4];
              memset(block, 0, sizeof(block));
              ctx.server->istr().read((char*)block, sizeof(block));
              ASSERT((uint8_t)0x01, block[0]);
              ASSERT((uint8_t)0x92, block[1]);
              ASSERT((uint8_t)0x00, block[2]);
              ASSERT((uint8_t)0xF4, block[3]);
          })
END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
