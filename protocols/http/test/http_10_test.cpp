#include "mud/core/exception.h"
#include "mud/event/event_loop.h"
#include "mud/http/client.h"
#include "mud/http/server.h"
#include "mud/io/tcp.h"
#include "mud/test.h"
#include "mock.h"
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
        server.stop();
        mud::event::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }
    }

    /** Thread to run the event loop */
    std::thread thr;

    /* The endpoint */
    mud::io::tcp::endpoint endpoint;

    /* The HTTP server */
    mock::server server;

    /* An HTTP client */
    mock::client client;

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
          mud::http::response response;
          response.version(mud::http::version_e::HTTP10);
          response.status_code(mud::http::status_code_e::OK);
          response.reason_phrase(mud::http::reason_phrase_e::OK);
          ctx.server.response(response);
          ctx.server.start(ctx.endpoint);
      })
  DEFINE_WHEN ("A client sends a request",
      [](context& ctx) {
          mud::http::request req;
          req.version(mud::http::version_e::HTTP10);
          req.method(mud::http::method_e::GET);
          req.uri("http://www.example.com/index.html");
          ctx.resp_future = ctx.client.request(ctx.endpoint, req);
      })
  DEFINE_THEN("The client receives a response",
      [](context& ctx) {
          ASSERT(std::future_status::ready, 
                 ctx.resp_future.wait_for(std::chrono::milliseconds(1000)));
          ctx.resp = ctx.resp_future.get();
      })
  DEFINE_THEN("The connection is closed",
      [](context& ctx) {
          // Not testable
      })

  END_DEFINES()

  /*
   * The scenarios.
   */

    SCENARIO("HTTP client closes a connection after receiving a reply")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request")
        THEN ("The client receives a response")
         AND ("The connection is closed")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
