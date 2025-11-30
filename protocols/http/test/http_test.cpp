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
