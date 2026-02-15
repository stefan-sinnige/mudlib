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
#include <mud/core/event_loop.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/http/server.h>
#include <string.h>

// ===========================================================================
// Server
// ===========================================================================

class server : public mud::http::server
{
public:
    // Construction
    server();
    ~server();

    // Run the server
    void run(const std::string& host, uint16_t port);

private:
    // The handler an HTTP request has been received
    mud::http::response request(const mud::http::request& req) override;
};

server::server() {}

server::~server() {}

void
server::run(const std::string& host, uint16_t port)
{
    mud::io::tcp::endpoint endpoint(host, port);
    start(endpoint);
}

mud::http::response
server::request(const mud::http::request& req)
{
    std::cout << "Received request, replying wih response" << std::endl;
    mud::http::response resp;
    resp.clear();
    resp.version(req.version());
    resp.status_code(mud::http::status_code_e::OK);
    resp.reason_phrase(mud::http::reason_phrase_e::OK);
    std::string reply = "<html>\r\n"
                        "<body>\r\n"
                        "  <h1>Hello</h1>\r\n"
                        "</body>\r\n"
                        "</html>\r\n";
    resp.field<mud::http::content_length>(reply.size());
    resp.entity_body(reply);
    return resp;
}

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    // Command line processing
    mud::core::uri uri("http://127.0.0.1");
    while (--argc > 0 && *(++argv)[0] == '-') {
        if (strcmp(*argv, "--uri") == 0) {
            --argc, ++argv;
            if (argc <= 0) { 
                std::cerr << "Error: Option --uri requires an argument."
                          << std::endl;
                return 1;
            }
            try {
                uri = mud::core::uri(*argv);
            }   
            catch (std::exception& ex) {
                std::cerr << ex.what() << std::endl;
                return 1;
            }
        }
        else
        if (strcmp(*argv, "--help") == 0) {
            std::cout << "Option:" << std::endl
                      << "    --uri URI" << std::endl;
            return 0;
        }
        else {
            std::cerr << "Unknown option '" << *argv << "'" << std::endl;
            return 1;
        }
    }

    // Create the server
    server server;
    try {
        server.run(uri.host(), uri.port());
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the global event loop
    mud::core::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
