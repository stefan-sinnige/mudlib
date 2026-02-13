#include "mud/io/udp.h"
#include "mud/core/event_loop.h"
#include "mud/test.h"
#include <cstring>
#include <memory>
#include <thread>
#include <type_traits>

/* clang-format off */

CONTEXT()
    // Constructor, executed before each scenario run
    context()
        : endpoint(std::string("127.0.0.1"), 52618)
    {
        thr = std::thread([]() {
            mud::core::event_loop::global().loop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Destructor, executed after each scenario run
    ~context() {
        mud::core::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }
    }

    
    /* Thread to run the event-loop */
    std::thread thr;

    /* The UDP communication end-point. */
    mud::io::udp::endpoint endpoint;

    /* The UDP server */
    mud::io::udp::communicator server;

    /* The UDP server */
    mud::io::udp::communicator client;
END_CONTEXT()

FEATURE("UDP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A UDP server",
    [](context& ctx){
        mud::io::udp::socket socket;
        socket.bind(ctx.endpoint);
        ctx.server.open(std::move(socket));
    })
  DEFINE_GIVEN("A UDP client",
    [](context& ctx){
        mud::io::udp::socket socket;
        ctx.client.open(std::move(socket));
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits (socket)")
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

  SCENARIO("Type traits (communicator)")
    GIVEN("A socket communicator type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::io::udp::communicator>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::udp::communicator>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::io::udp::communicator>::value);
        })
     AND ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::udp::communicator>::value);
        })
     AND ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::io::udp::communicator>::value);
        })

    SCENARIO("Writing and reading binary data")
      GIVEN("A UDP server")
       AND ("A UDP client")
      WHEN ("The client writes binary data",
          [](context& ctx) {
              uint8_t block[] = {0x01, 0x92, 0x00, 0xF4};
              ctx.client.ostr(ctx.endpoint).write(
                    (const char*)block, sizeof(block))
                  << std::flush;
          })
      THEN ("The same binary data can be read by the server",
          [](context& ctx) {
              uint8_t block[4];
              memset(block, 0, sizeof(block));
              ctx.server.istr().read((char*)block, sizeof(block));
              ASSERT((uint8_t)0x01, block[0]);
              ASSERT((uint8_t)0x92, block[1]);
              ASSERT((uint8_t)0x00, block[2]);
              ASSERT((uint8_t)0xF4, block[3]);
          })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
