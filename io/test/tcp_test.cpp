#include "mud/io/tcp.h"
#include "mud/test.h"
#include <mud/core/event_loop.h>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <future>
#include <memory>
#include <type_traits>

/*
 * A very rudimentary protocol is used as a test-bed and consists of data
 * packets that are described as
 *   Length:  uint16_t
 *   Data:    uint8_t * Length
 */

/* clang-format off */

CONTEXT(public mud::core::object)
    // Constructor, executed before each scenario run
    context()
        : send_value(nullptr), recv_value(nullptr)
        , accepted(false), connected(false), received(false)
    {
        thr = std::thread([]() {
            mud::core::event_loop::global().loop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Destructor, executed after each scenario run
    ~context() {
        detach();
        mud::core::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }
        delete [] send_value;
        delete [] recv_value;
    }

    // The effector when a connection is accepted (server)
    void on_accepted(const mud::core::message&) {
        std::lock_guard<std::mutex> lock(accepted_lock);
        server = acceptor.connection();
        accepted = true;
        accepted_cv.notify_all();
    }

    // The effector when a connection is established (client)
    void on_connected(const mud::core::message&) {
        std::lock_guard<std::mutex> lock(connected_lock);
        attach(communicator.received(), &context::on_received);
        communicator.open(std::move(connector.connection()));
        connected = true;
        connected_cv.notify_all();
    }

    // The effector when data is available (client)
    void on_received(const mud::core::message&) {
        // mud::io::tcp::socket& socket) {
        // Read 2 bytes indicating the length
        uint16_t two_bytes;
        communicator.istr().read((char*)&two_bytes, 2);
        recv_length = mud::core::endian::convert(
                    mud::core::endian::endian_t::big,
                    mud::core::endian::native(),
                    two_bytes);

        // Allocate the placeholder for the data to be received
        recv_value = new uint8_t[recv_length];
        memset(recv_value, 0, recv_length);

        // Read the data
        communicator.istr().read((char*)recv_value, recv_length);
        {
            std::lock_guard<std::mutex> lock(received_lock);
            received = true;
        }
        received_cv.notify_all();
    }

    // Send the data in one block. This should appear in one fragment if the
    // data is not too big.
    void send(uint16_t mtu = 1460, uint16_t ms = 0) {
        // Write the 2 bytes indicating the length
        uint16_t length = mud::core::endian::convert(
                    mud::core::endian::native(),
                    mud::core::endian::endian_t::big,
                    (uint16_t)send_length);
        server.ostr().write((const char*)&length, 2);

        // Writing the data in blocks
        uint16_t offset = 0;
        while (offset < send_length) {
            // Determine the block size to send.
            uint16_t blocksize = send_length - offset;
            if (blocksize > mtu) {
                blocksize = mtu;
            }

            // Send it
            server.ostr().write((const char*)send_value+offset, blocksize);
            server.ostr().flush();

            // Wait for sending next packet.
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));

            // Update to send the next block
            offset += blocksize;
        }
    }

    /* Thread to run the event-loop */
    std::thread thr;

    /* The TCP server */
    mud::io::tcp::socket server;
    mud::io::tcp::acceptor acceptor;

    /* The TCP client */
    mud::io::tcp::connector connector;
    mud::io::tcp::communicator communicator;

    /* The data to send */
    size_t send_length;
    uint8_t *send_value;

    /* The data received */
    size_t recv_length;
    uint8_t *recv_value;

    /* Flag when a connection has been accepted from a server perspective */
    std::mutex accepted_lock;
    std::condition_variable accepted_cv;
    bool accepted;

    /* Flag when a connection has been established from a client perspective */
    std::mutex connected_lock;
    std::condition_variable connected_cv;
    bool connected;

    /* Flag when data has been reveived by the peer */
    std::mutex received_lock;
    std::condition_variable received_cv;
    bool received;
END_CONTEXT()

void
static_sample(mud::io::tcp::socket& socket)
{
}

FEATURE("TCP sockets")

  // Pre-defined steps
  DEFINE_GIVEN("A TCP server",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.attach(ctx.acceptor.accepted(), &context::on_accepted);
        ctx.acceptor.open(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_GIVEN("A connected TCP client",
    [](context& ctx){
        /* Create a connecting client */
        std::string localhost("127.0.0.1");
        ctx.attach(ctx.connector.connected(), &context::on_connected);
        ctx.connector.open(mud::io::tcp::endpoint(localhost, 52618));

        /* wait for the client to be connected */
        {
            std::unique_lock<std::mutex> lock(ctx.connected_lock);
            ctx.connected_cv.wait_for(lock, std::chrono::milliseconds(10),
                [&ctx]{ return ctx.connected; });
            ASSERT(true, ctx.connected);
        }

        /* Wait for the server to be accepted */
        {
            std::unique_lock<std::mutex> lock(ctx.accepted_lock);
            ctx.accepted_cv.wait_for(lock, std::chrono::milliseconds(10),
                [&ctx]{ return ctx.accepted; });
            ASSERT(true, ctx.accepted);
        }
    })
  DEFINE_GIVEN("A TCP server is listening for inbound connection",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.attach(ctx.acceptor.accepted(), &context::on_accepted);
        ctx.acceptor.open(mud::io::tcp::endpoint(localhost, 52618));

    })
  DEFINE_GIVEN("There is no TCP server listening for inbound connections",
        [](context&){})
  DEFINE_WHEN("The TCP client connects",
    [](context& ctx){
        std::string localhost("127.0.0.1");
        ctx.attach(ctx.connector.connected(), &context::on_connected);
        ctx.connector.open(mud::io::tcp::endpoint(localhost, 52618));
    })
  DEFINE_WHEN("The TCP server accepts the connection",
    [](context& ctx){
        std::unique_lock<std::mutex> lock(ctx.accepted_lock);
        ctx.accepted_cv.wait_for(lock, std::chrono::milliseconds(10),
            [&ctx]{ return ctx.accepted; });
        ASSERT(true, ctx.accepted);
    })
  DEFINE_WHEN("The data has been transmitted",
    [](context& ctx){
        std::unique_lock<std::mutex> lock(ctx.received_lock);
        ctx.received_cv.wait_for(lock, std::chrono::seconds(1),
            [&ctx]{ return ctx.received; });
        ASSERT(true, ctx.received);
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
     AND ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::io::tcp::acceptor>::value);
        })
     AND ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
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
              ctx.send_length = 4;
              ctx.send_value = new uint8_t[ctx.send_length];
              ctx.send_value[0] = 0x01;
              ctx.send_value[1] = 0x92;
              ctx.send_value[2] = 0x00;
              ctx.send_value[3] = 0xF4;
              ctx.send();
          })
       AND ("The data has been transmitted")
      THEN ("The same binary data can be read from the other endpoint",
          [](context& ctx) {
              ASSERT(4, ctx.recv_length);
              ASSERT((uint8_t)0x01, ctx.recv_value[0]);
              ASSERT((uint8_t)0x92, ctx.recv_value[1]);
              ASSERT((uint8_t)0x00, ctx.recv_value[2]);
              ASSERT((uint8_t)0xF4, ctx.recv_value[3]);
          })

    SCENARIO("Writing and reading fragmented binary data")
      GIVEN("A TCP server is listening for inbound connection")
      WHEN ("The TCP client connects")
       AND ("The TCP server accepts the connection")
       AND ("Binary data is written from one endpoint",
          [](context& ctx) {
              ctx.send_length = 16*1024;
              ctx.send_value = new uint8_t[ctx.send_length];
              for (int i = 0; i < ctx.send_length; ++i) {
                  ctx.send_value[i] = i % 255;
              }
              ctx.send(1460, 5);
          })
       AND ("The data has been transmitted")
      THEN ("The same binary data can be read from the other endpoint",
          [](context& ctx) {
              ASSERT(16*1024, ctx.recv_length);
              for (int i = 0; i < ctx.recv_length; ++i) {
                  ASSERT(i % 255, ctx.recv_value[i]);
              }
          })

    SCENARIO("Bla")
      GIVEN("A TCP server")
        AND("A connected TCP client")
       WHEN("nothing", [](context& ctx) {})
       THEN("nothing", [](context& ctx) {})
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
