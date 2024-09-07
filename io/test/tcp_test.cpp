#include "mud/io/tcp.h"
#include "mud/test.h"
#include <mud/event/event_loop.h>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <future>
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT_1(mud::core::object)
    // Constructor, executed before each scenario run
    context()
        : accepted(false), connected(false)
    {
        thr = std::thread([]() {
            mud::event::event_loop::global().loop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Destructor, executed after each scenario run
    ~context() {
        mud::event::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }

    }

    // The effector when a connection is accepted (server)
    void on_accept(mud::io::tcp::socket& socket) {
        server = std::move(socket);
        {
            std::lock_guard<std::mutex> lock(accepted_lock);
            accepted = true;
        }
        accepted_cv.notify_all();
    }

    // The effector when a connection is established (client)
    void on_connect(mud::io::tcp::socket& socket) {
        client = std::move(socket);
        client.option<bool, mud::io::ip::nonblocking>(false);
        {
                std::lock_guard<std::mutex> lock(connected_lock);
                connected = true;
        }
        connected_cv.notify_all();
    }

    /* Thread to run the event-loop */
    std::thread thr;

    /* The TCP server */
    mud::io::tcp::socket server;
    mud::io::tcp::acceptor acceptor;

    /* The TCP client */
    mud::io::tcp::socket client;
    mud::io::tcp::connector connector;

    /* Flag when a connection has been accepted from a server perspective */
    std::mutex accepted_lock;
    std::condition_variable accepted_cv;
    bool accepted;

    /* Flag when a connection has been established from a client perspective */
    std::mutex connected_lock;
    std::condition_variable connected_cv;
    bool connected;
END_CONTEXT()

void
static_sample(mud::io::tcp::socket& socket)
{
}

FEATURE("TCP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A TCP server is listening for inbound connection",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.acceptor.accept_impulse()->attach(&ctx, &context::on_accept);
        ctx.acceptor.open(mud::io::tcp::endpoint(localhost, 52618));
        mud::event::event_loop::global().register_handler(ctx.acceptor.event());

    })
  DEFINE_GIVEN("There is no TCP server listening for inbound connections",
        [](context&){})
  DEFINE_WHEN("The TCP client connects",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.connector.connect_impulse()->attach(&ctx, &context::on_connect);
        ctx.connector.open(mud::io::tcp::endpoint(localhost, 52618));
        mud::event::event_loop::global().register_handler(ctx.connector.event());
    })
  DEFINE_WHEN("The TCP server accepts the connection",
    [](context& ctx){
        std::unique_lock<std::mutex> lock(ctx.accepted_lock);
        ctx.accepted_cv.wait_for(lock, std::chrono::milliseconds(10),
            [&ctx]{ return ctx.accepted; });
        ASSERT(true, ctx.accepted);
    })
  DEFINE_THEN("A connection is established",
    [](context& ctx){
        std::unique_lock<std::mutex> lock(ctx.connected_lock);
        ctx.connected_cv.wait_for(lock, std::chrono::milliseconds(10),
            [&ctx]{ return ctx.connected; });
        ASSERT(true, ctx.connected);
    })
  DEFINE_THEN("A connection is pending",
    [](context& ctx){
        std::unique_lock<std::mutex> lock(ctx.connected_lock);
        ASSERT(
            std::cv_status::timeout,
            ctx.connected_cv.wait_for(lock, std::chrono::milliseconds(10)));
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
       AND ("The TCP server accepts the connection")
      THEN ("A connection is established")

    SCENARIO("Waiting for a connection when there is no listening socket")
      GIVEN("There is no TCP server listening for inbound connections")
      WHEN ("The TCP client connects")
      THEN ("A connection is pending")

    SCENARIO("Writing and reading binary data")
      GIVEN("A TCP server is listening for inbound connection")
      WHEN ("The TCP client connects")
       AND ("The TCP server accepts the connection")
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

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
