#include <iostream>
#include <mud/event/event_loop.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/http/server.h>

// ===========================================================================
// Server
// ===========================================================================

class server
{
public:
    // Construction
    server();
    ~server();

    // Run the server
    void run(const std::string& host, uint16_t port);

private:
    // The handler an HTTP request has been received
    mud::http::response on_request(const mud::http::request& request);

    // The acceptor.
    mud::http::server _server;
};

server::server()
{
    _server.on_request(
        std::bind(&server::on_request, this, std::placeholders::_1));
}

server::~server() {}

void
server::run(const std::string& host, uint16_t port)
{
    mud::io::tcp::endpoint endpoint(host, port);
    _server.start(endpoint);
}

mud::http::response
server::on_request(const mud::http::request& request)
{
    std::cout << "Received request, replying wih response" << std::endl;
    mud::http::response response;
    response.version(request.version());
    response.status_code(mud::http::status_code_e::OK);
    response.reason_phrase(mud::http::reason_phrase_e::OK);
    std::string reply = "<html>\r\n"
                        "<body>\r\n"
                        "  <h1>Hello</h1>\r\n"
                        "</body>\r\n"
                        "</html>\r\n";
    response.field<mud::http::content_length>(reply.size());
    response.entity_body(reply);
    return response;
}

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    // Command line processing
    std::string host = "127.0.0.1";
    uint16_t port = 80;
    while (--argc > 0 && *(++argv)[0] == '-') {
        if (strcmp(*argv, "--host") == 0) {
            --argc, ++argv;
            if (argc <= 0) {
                std::cerr << "Error: Option --host requires an argument."
                          << std::endl;
                return 1;
            }
            host = *argv;
        }
        if (strcmp(*argv, "--port") == 0) {
            --argc, ++argv;
            if (argc <= 0) {
                std::cerr << "Error: Option --port requires an argument."
                          << std::endl;
                return 1;
            }
            port = atoi(*argv);
        }
    }

    // Create the server
    server server;
    try {
        server.run(host, port);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the global event loop
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
