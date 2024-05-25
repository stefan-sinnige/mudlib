#include "mud/core/exception.h"
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
  DEFINE_THEN("The connection is closed",
      [](context& ctx) {
          mud::http::request req;
          ASSERT_THROW(mud::core::not_owner, ctx.resp_future = ctx.client.request(ctx.endpoint, req));
      })

  END_DEFINES()

  /*
   * The scenarios.
   */

    SCENARIO("HTTP server closes a connection when no Connection field is present")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request")
        THEN ("The client receives a response")
         AND ("The connection is closed")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
