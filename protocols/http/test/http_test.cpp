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

FEATURE("HTTP Protocol")

  /*
   * The predefined Gherkin steps.
   */

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

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
