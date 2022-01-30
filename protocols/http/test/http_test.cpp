#include "mud/event/event_loop.h"
#include "mud/http/message.h"
#include "mud/http/server.h"
#include "mud/io/tcp.h"
#include "mud/test.h"
#include <condition_variable>
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context()
        : connected(false)
    {
        thr = std::thread([]() {
            mud::event::event_loop::global().loop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    /* Destructor after each scenario */
    ~context() {
        mud::event::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }
    }

    /** Thread to run the ecebt loop */
    std::thread thr;

    /* The HTTP server */
    mud::http::server server;

    /* An HTTP client */
    mud::io::tcp::socket client;
    mud::io::tcp::connector connector;

    /* Flag when a connection has been established from a client perspective */
    std::mutex connected_lock;
    std::condition_variable connected_cv;
    bool connected;
END_CONTEXT()

FEATURE("HTTP/1.0 Protocol")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An HTTP server is listening for inbound connections",
      [](context& ctx) {
          std::string localhost("127.0.0.1");
          ctx.server.start(localhost, 52618);
          ctx.server.on_request([](const mud::http::message& request) {
              mud::http::message response;
              response.type(mud::http::message::type_t::RESPONSE);
              response.field<mud::http::version>(
                  request.field<mud::http::version>());
              response.field<mud::http::status_code>(
                  mud::http::status_code::OK);
              response.field<mud::http::reason_phrase>(
                  mud::http::reason_phrase::OK);
              return response;
          });
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      })
  DEFINE_WHEN ("The server is stopped",
      [](context& ctx) {
          ctx.server.stop();
      })
  DEFINE_WHEN ("The client connects",
      [](context& ctx) {
          std::string localhost("127.0.0.1");
          ctx.connector.on_connect([&ctx](mud::io::tcp::socket&& socket) {
              ctx.client = std::move(socket);
              ctx.client.option<bool, mud::io::ip::nonblocking>(false);
              {
                  std::lock_guard<std::mutex> lock(ctx.connected_lock);
                  ctx.connected = true;
              }
              ctx.connected_cv.notify_all();
          });
          ctx.connector.open(mud::io::tcp::endpoint(localhost, 52618));
          std::unique_lock<std::mutex> lock(ctx.connected_lock);
          ctx.connected_cv.wait_for(lock, std::chrono::milliseconds(10),
              [&ctx]{ return ctx.connected; });
          ASSERT(true, ctx.connected);
      })
  DEFINE_WHEN("A request is sent",
      [](context& ctx) {
          mud::http::message req;
          req.type(mud::http::message::type_t::REQUEST);
          req.field<mud::http::version>(mud::http::version::Version::HTTP10);
          req.field<mud::http::method>(mud::http::method::GET);
          req.field<mud::http::uri>("http://www.example.com/index.html");
          ctx.client.ostr() << req << std::flush;
      })
  DEFINE_THEN("A response is received",
      [](context& ctx) {
          mud::http::message resp;
          ctx.client.istr() >> resp;
          ctx.client.close();
      })

  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("An HTTP Server type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::http::server>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::http::server>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::http::server,
                  mud::http::server>::value);
        })

    SCENARIO("HTTP server can send a response")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("The client connects")
         AND ("A request is sent")
        THEN ("A response is received")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
