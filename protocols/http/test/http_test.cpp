#include "mud/event/event_loop.h"
#include "mud/http/client.h"
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
        : endpoint(std::string("127.0.0.1"), 52618)
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

    /* The endpoint */
    mud::io::tcp::endpoint endpoint;

    /* The HTTP server */
    mud::http::server server;

    /* An HTTP client */
    mud::http::client client;

    /* The request */
    mud::http::request req;

    /* The response */
    mud::http::response resp;

    /* The response future */
    std::future<mud::http::response> resp_future;
END_CONTEXT()

FEATURE("HTTP/1.0 Protocol")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An HTTP server is listening for inbound connections",
      [](context& ctx) {
          ctx.server.start(ctx.endpoint);
          ctx.server.on_request([](const mud::http::request& request) {
              mud::http::response response;
              response.version(request.version());
              response.status_code(mud::http::status_code_e::OK);
              response.reason_phrase(mud::http::reason_phrase_e::OK);
              return response;
          });
      })
  DEFINE_WHEN ("A client sends a request",
      [](context& ctx) {
          mud::http::request req;
          req.version(mud::http::version_e::HTTP10);
          req.method(mud::http::method_e::GET);
          req.uri("http://www.example.com/index.html");
          ctx.resp_future = ctx.client.request(ctx.endpoint, req);
      })
  DEFINE_WHEN ("The server is stopped",
      [](context& ctx) {
          ctx.server.stop();
      })
  DEFINE_THEN("The client receives a response",
      [](context& ctx) {
          ASSERT(std::future_status::ready, 
                 ctx.resp_future.wait_for(std::chrono::milliseconds(1000)));
          mud::http::response resp = ctx.resp_future.get();
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

    SCENARIO("HTTP server can process a request without connection field")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request")
        THEN ("The client receives a response")

/*
    SCENARIO("HTTP server ignores a disconnected client")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client closes before sending a response",
            [](context& ctx) {
                // Connect to the server
                mud::io::tcp::socket client;

                mud::io::tcp::connector connector;
                struct cv_t {
                    std::mutex connected_lock;
                    std::condition_variable connected_cv;
                    bool connected;
                } cv;
                connector.on_connect([&client, &cv](
                        mud::io::tcp::socket&& socket) {
                    client = std::move(socket);
                    client.option<bool, mud::io::ip::nonblocking>(false);
                    {
                        std::lock_guard<std::mutex> lock(cv.connected_lock);
                        cv.connected = true;
                    }
                    cv.connected_cv.notify_all();
                });
                connector.open(ctx.endpoint);
                std::unique_lock<std::mutex> lock(cv.connected_lock);
                cv.connected_cv.wait_for(lock, std::chrono::milliseconds(10),
                    [&cv]{ return cv.connected; });
                ASSERT(true, cv.connected);

                // Send a request
                mud::http::request req;
                req.version(mud::http::version_e::HTTP10);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                client.ostr() << req << std::flush;

                // Disconnect
                // TBD: This client socket should *not* be using the global
                // event loop as it may result in EBADF when trying to close
                // it in random runs.
                //client.close();
            })
        THEN ("No exception is thrown", [](context& ctx) {})
*/

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
