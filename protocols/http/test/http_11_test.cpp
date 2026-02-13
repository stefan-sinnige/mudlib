#include "mud/core/event_loop.h"
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
            mud::core::event_loop::global().loop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    /* Destructor after each scenario */
    ~context() {
        server.stop();
        mud::core::event_loop::global().terminate();
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

FEATURE("HTTP/1.1 Protocol")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An HTTP server is listening for inbound connections",
      [](context& ctx) {
          mud::http::response response;
          response.version(mud::http::version_e::HTTP11);
          response.status_code(mud::http::status_code_e::OK);
          response.reason_phrase(mud::http::reason_phrase_e::OK);
          response.field<mud::http::content_length>(0);
          ctx.server.response(response);
          ctx.server.start(ctx.endpoint);
      })
  DEFINE_WHEN ("A client sends a request",
      [](context& ctx) {
          mud::http::request req;
          req.version(mud::http::version_e::HTTP11);
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
          ctx.resp = ctx.resp_future.get();
      })
  DEFINE_THEN("The connection is kept alive",
      [](context& ctx) {
        ASSERT(mud::http::connection_e::KeepAlive,
               ctx.resp.field<mud::http::connection>());
      })
  DEFINE_THEN("The connection is closed",
      [](context& ctx) {
        ASSERT(mud::http::connection_e::Close,
               ctx.resp.field<mud::http::connection>());
      })

  END_DEFINES()

  /*
   * The scenarios.
   */

    SCENARIO("HTTP connection responds to 'Connection' field in request")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request with a Connection field",
            [](context& ctx) {
                mud::http::request req;
                req.version(mud::http::version_e::HTTP11);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                req.field<mud::http::connection>(
                    ctx.sample<mud::http::connection>("request"));
                ctx.resp_future = ctx.client.request(ctx.endpoint, req);
            })
        THEN ("The client receives a response")
         AND ("The response contains the associated Connection field",
            [](context& ctx) {
                ASSERT(ctx.sample<mud::http::connection>("response"),
                       ctx.resp.field<mud::http::connection>().value());
            })
        SAMPLES(mud::http::connection, mud::http::connection)
            HEADINGS("request", "response")
            SAMPLE(mud::http::connection_e::Close,
                   mud::http::connection_e::Close)
            SAMPLE(mud::http::connection_e::KeepAlive,
                   mud::http::connection_e::KeepAlive)
        END_SAMPLES()

    SCENARIO("HTTP connection persists when 'Connection: Keep-Alive' is present")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request with a keep-alive Connection field",
            [](context& ctx) {
                mud::http::request req;
                req.version(mud::http::version_e::HTTP11);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                req.field<mud::http::connection>(
                    mud::http::connection_e::KeepAlive);
                ctx.resp_future = ctx.client.request(ctx.endpoint, req);
            })
        THEN ("The client receives a response")
         AND ("The connection is kept alive")

    SCENARIO("HTTP connection closes when 'Connection: Close' is present")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends a request with a close Connection field",
            [](context& ctx) {
                mud::http::request req;
                req.version(mud::http::version_e::HTTP11);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                req.field<mud::http::connection>(
                    mud::http::connection_e::Close);
                ctx.resp_future = ctx.client.request(ctx.endpoint, req);
            })
        THEN ("The client receives a response")
         AND ("The connection is closed")

    SCENARIO("HTTP server can handle multiple requests on the same connection")
        GIVEN("An HTTP server is listening for inbound connections")
        WHEN ("A client sends and receives one request",
            [](context& ctx) {
                mud::http::request req;
                req.version(mud::http::version_e::HTTP11);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                req.field<mud::http::connection>(
                    mud::http::connection_e::KeepAlive);
                ctx.resp_future = ctx.client.request(ctx.endpoint, req);
                ASSERT(std::future_status::ready, 
                     ctx.resp_future.wait_for(std::chrono::milliseconds(1000)));
                ctx.resp = ctx.resp_future.get();
            })
          AND("A client sends another request on the same connection",
            [](context& ctx) {
                mud::http::request req;
                req.version(mud::http::version_e::HTTP11);
                req.method(mud::http::method_e::GET);
                req.uri("http://www.example.com/index.html");
                req.field<mud::http::connection>(
                    mud::http::connection_e::KeepAlive);
                ctx.resp_future = ctx.client.request(ctx.endpoint, req);
                ASSERT(std::future_status::ready, 
                     ctx.resp_future.wait_for(std::chrono::milliseconds(1000)));
                ctx.resp = ctx.resp_future.get();
            })
        THEN ("The connection is kept alive")

    SCENARIO("HTTP 'Content-Length' is remediated when not supplied")
        GIVEN("An HTTP server is listening for inbound connections",
            [](context& ctx) {
                mud::http::response response;
                response.version(mud::http::version_e::HTTP11);
                response.status_code(mud::http::status_code_e::OK);
                response.reason_phrase(mud::http::reason_phrase_e::OK);
                response.entity_body("Hello World");
                ctx.server.response(response);
                ctx.server.start(ctx.endpoint);
            })
        WHEN ("A client sends a request")
        THEN ("The client receives a response")
         AND ("The response contains a 'Content-Length'",
            [](context& ctx) {
                ASSERT(11, ctx.resp.field<mud::http::content_length>());
            })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
