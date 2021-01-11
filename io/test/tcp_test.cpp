#include "mud/io/kernel_event_loop.h"
#include "mud/io/tcp.h"
#include "mud/test.h"
#include <future>
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    // Constructor, executed before each scenario run
    context()
        : accepted(false), connected(false)
    {
    }

    // Destructor, executed after each scenario run
    ~context() {
    }

    mud::io::tcp::socket server;
    mud::io::tcp::socket client;
    mud::io::tcp::acceptor acceptor;
    mud::io::tcp::connector connector;
    bool accepted;
    bool connected;
END_CONTEXT()

FEATURE("TCP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A TCP server is listening for inbound connection",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.acceptor.on_accept([&ctx](mud::io::tcp::socket&& socket) {
            ctx.server = std::move(socket);
            ctx.accepted = true;
        });
        ctx.acceptor.open(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_WHEN("The TCP client connects",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.connector.on_connect([&ctx](mud::io::tcp::socket&& socket) {
            ctx.client = std::move(socket);
            ctx.client.option<bool, mud::io::ip::nonblocking>(false);
            ctx.connected = true;
        });
        ctx.connector.open(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_WHEN("And the TCP server accepts the connection",
    [](context& ctx){
        // Run the global event loop for 50ms to establish a connection.
        std::future<void> future = std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            mud::io::kernel_event_loop::global().terminate();
        });
        mud::io::kernel_event_loop::global().loop();
    })
  DEFINE_THEN("A connection is established",
    [](context& ctx){
        ASSERT(true, ctx.accepted);
        ASSERT(true, ctx.connected);
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits (socket)")
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
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::tcp::socket>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::tcp::socket>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::tcp::socket>::value);
        })

  SCENARIO("Type traits (acceptor)")
    GIVEN("A socket acceptor type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::tcp::acceptor>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::tcp::acceptor>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::tcp::acceptor>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::tcp::acceptor>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::tcp::acceptor>::value);
        })

  SCENARIO("Type traits (connector)")
    GIVEN("A socket connector type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::tcp::connector>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::tcp::connector>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::tcp::connector>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::tcp::connector>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::tcp::connector>::value);
        })

  SCENARIO("Type traits (communicator)")
    GIVEN("A socket communicator type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::tcp::communicator>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::tcp::communicator>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::tcp::communicator>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::tcp::communicator>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::tcp::communicator>::value);
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
              ctx.server.option<bool, mud::io::ip::nonblocking>(false);
              ctx.server.ostr().write((const char*)block, sizeof(block))
                  << std::flush;
          })
      THEN ("The same binary data can be read from the other endpoint",
          [](context& ctx) {
              uint8_t block[4];
              memset(block, 0, sizeof(block));
              ctx.client.option<bool, mud::io::ip::nonblocking>(false);
              ctx.client.istr().read((char*)block, sizeof(block));
              ASSERT((uint8_t)0x01, block[0]);
              ASSERT((uint8_t)0x92, block[1]);
              ASSERT((uint8_t)0x00, block[2]);
              ASSERT((uint8_t)0xF4, block[3]);
          })

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
