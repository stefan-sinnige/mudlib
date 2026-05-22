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
#include "mud/io/tcp.h"
#include "mud/test.h"
#include "mock.h"
#include <chrono>
#include <string>
#include <thread>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        /* Start the event loop */
        thr = std::thread([]() {
            mud::core::event_loop::global().loop();
        });
        mud::core::event_loop::global().ready().wait();
        channel = std::make_shared<mock::channel>();
    }

    /* Destructor after each scenario */
    ~context() {
        client.close();
        server.close();
        mud::core::event_loop::global().terminate();
        if (thr.joinable())
        {
            thr.join();
        }
    }

    /** Thread to run the event loop */
    std::thread thr;

    /* The TLS server */
    mock::server server;

    /* An TLS client */
    mock::client client;

    /* The communication channel */
    std::shared_ptr<mock::channel> channel;
END_CONTEXT()

FEATURE("TLS v1.3 Protocol")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("A TLS server is listening for inbound connection",
      [](context& ctx) {
        ctx.server.open(ctx.channel);
        ctx.server.listen();
      })
  DEFINE_WHEN("A TLS client connects",
      [](context& ctx) {
        ctx.client.open(ctx.channel);
        ctx.client.connect();
      })
  DEFINE_THEN("The connection is established",
      [](context& ctx) {
        auto server_future = ctx.server.connected_future();
        ASSERT(std::future_status::ready,
               server_future.wait_for(std::chrono::milliseconds(100)));
        auto client_future = ctx.client.connected_future();
        ASSERT(std::future_status::ready,
               client_future.wait_for(std::chrono::milliseconds(100)));

        ctx.client.send();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ctx.server.receive();

        ctx.server.send();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ctx.client.receive();
      })
  DEFINE_THEN("The connection is closed",
      [](context& ctx) {
        auto server_future = ctx.server.disconnected_future();
        ASSERT(std::future_status::ready,
               server_future.wait_for(std::chrono::milliseconds(100)));
        auto client_future = ctx.client.disconnected_future();
        ASSERT(std::future_status::ready,
               client_future.wait_for(std::chrono::milliseconds(100)));
      })
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("TLS connection can be established")
    GIVEN("A TLS server is listening for inbound connection")
    WHEN("A TLS client connects")
    THEN("The connection is established")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
