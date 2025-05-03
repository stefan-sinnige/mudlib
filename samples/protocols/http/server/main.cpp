#include <iostream>
#include <mud/event/event_loop.h>
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
    void on_request(
            const mud::http::request& req,
            mud::http::response& resp) override;
};

server::server() {}

server::~server() {}

void
server::run(const std::string& host, uint16_t port)
{
    mud::io::tcp::endpoint endpoint(host, port);
    start(endpoint);
}

void
server::on_request(
            const mud::http::request& req,
            mud::http::response& resp)
{
    std::cout << "Received request, replying wih response" << std::endl;
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
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
