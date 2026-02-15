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

#include <iostream>
#include <string>
#include <string.h>
#include <utility>
#include <vector>
#include <mud/core/event_loop.h>
#include <mud/core/uri.h>
#include <mud/http/client.h>
#include <mud/http/request.h>
#include <mud/http/response.h>

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    // Any user-defined header fields
    std::vector<std::pair<std::string, std::string>> fields;

    // Command line processing
    while (--argc > 0 && *(++argv)[0] == '-') {
        if ((strcmp(*argv, "--help") == 0) || (strcmp(*argv, "-h") == 0)) {
            std::cout << 
R"(Usage: sample_http_client [options] URL" << std::endl
Options:
   --header FIELD:VALUE  Add a header FIELD with a VALUE
   --help, -h            Show this help)"
                      << std::endl;
            return 0;
        }
        else
        if (strcmp(*argv, "--header") == 0) {
            --argc, ++argv;
            if (argc < 2) {
                std::cerr << "Missing argument for '--header'\n";
                return 1;
            }
            const char* sep = strchr(*argv, ':');
            if (sep == nullptr) {
                std::cerr << "Missing ':' separator for '--header'\n";
                return 1;
            }
            fields.emplace_back(std::make_pair(
                std::string(*argv, sep - *argv),
                std::string(sep+1)));
        }
        else {
            std::cerr << "Unknown option '" << *argv << "'\n";
            return 1;
        }
    }

    // We should have one option left, the request itself
    if (argc != 1) {
        std::cerr << "Expected request URL" << std::endl;
        return 1;
    }
    mud::core::uri uri(*argv);

    // Create the request to send
    mud::http::request request;
    request.method(mud::http::method_e::GET);
    request.uri(uri);
    for (auto field: fields) {
        mud::http::field_ext ext(field.first);
        ext.value(field.second);
        request.field(ext);
    }

    // Create the event-loop
    std::thread event_thread = std::thread([]() {
        mud::core::event_loop::global().loop();
    });
    mud::core::event_loop::global().ready().wait();

    // Create the client to send the request and receive the response.
    mud::http::client client;
    mud::io::tcp::endpoint endpoint(uri.host(), uri.port());
    std::future<mud::http::response> response = client.request(
            endpoint, request);

    // Expect a respons within 5 seconds and print it out.
    if (std::future_status::ready !=
        response.wait_for(std::chrono::milliseconds(5000)))
    {
        std::cerr << "Unable to get a response within 5 seconds\n";
    }
    else
    {
        std::cout << response.get() << std::endl;
    }

    // Stop the event loop and exit
    mud::core::event_loop::global().terminate();
    event_thread.join();
    return 0;
}

/* vi: set ai ts=4 expandtab: */
