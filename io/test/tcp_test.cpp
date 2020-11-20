#include "mud/io/tcp.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    // Constructor, executed before each scenario run
    context()
        : accept(nullptr), client(nullptr), client_connected(nullptr),
          acceptor(nullptr)
    {
    }

    // Destructor, executed after each scenario run
    ~context() {
        delete accept;
        delete client;
        delete client_connected;
        delete acceptor;
    }

    mud::io::tcp::socket *accept;
    mud::io::tcp::socket *client;
    mud::io::tcp::socket *client_connected;
    mud::io::tcp::acceptor *acceptor;
END_CONTEXT()

FEATURE("TCP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A TCP server is listening for inbound connection",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.accept = new mud::io::tcp::socket;
        ctx.acceptor = new mud::io::tcp::acceptor(*ctx.accept);
        ctx.acceptor->open(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_WHEN("The TCP client connects",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.client = new mud::io::tcp::socket;
        mud::io::tcp::connector connector(*ctx.client);
        connector.connect(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_WHEN("And the TCP server accepts the connection",
    [](context& ctx){
        mud::io::tcp::socket client = ctx.acceptor->accept();
        ctx.client_connected = new mud::io::tcp::socket(std::move(client));
    })
  DEFINE_THEN("A connection is established",
    [](context& ctx){
        ASSERT(true, ctx.client != nullptr);
        ASSERT(true, ctx.client_connected != nullptr);
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("A TCP socket type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::tcp::socket>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::tcp::socket>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::tcp::socket>::value);
        })
     AND ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::io::tcp::socket,
                  mud::io::tcp::socket>::value);
        })

    SCENARIO("Accepting connection")
      GIVEN("A TCP server is listening for inbound connection")
      WHEN ("The TCP client connects")
       AND ("And the TCP server accepts the connection")
      THEN ("A connection is established")

    SCENARIO("Writing and reading binary data")
      GIVEN("A TCP server is listening for inbound connection")
      WHEN ("The TCP client connects")
       AND ("And the TCP server accepts the connection")
       AND ("Binary data is written from one endpoint",
          [](context& ctx) {
              uint8_t block[] = {0x01, 0x92, 0x00, 0xF4};
              ctx.client->ostr().write((const char*)block, sizeof(block))
                  << std::flush;
          })
      THEN ("The same binary data can be read from the other endpoint",
          [](context& ctx) {
              uint8_t block[4];
              memset(block, 0, sizeof(block));
              ctx.client_connected->istr().read((char*)block, sizeof(block));
              ASSERT((uint8_t)0x01, block[0]);
              ASSERT((uint8_t)0x92, block[1]);
              ASSERT((uint8_t)0x00, block[2]);
              ASSERT((uint8_t)0xF4, block[3]);
          })

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
