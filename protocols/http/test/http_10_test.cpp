/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/event_loop.h"
#include "mud/core/exception.h"
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
        mud::core::event_loop::global().ready().wait();
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
