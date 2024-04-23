#include <cstring>
#include <iostream>
#include <mud/event/event_loop.h>
#include <mud/http/message.h>
#include <mud/io/tcp.h>

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

    // Set-up the client connector
    mud::io::tcp::connector connector;
    mud::io::tcp::communicator communicator;
    connector.on_connect([&](mud::io::tcp::socket&& socket) {
        try {
            // Send the request
            std::cout << "Sending request ... " << std::flush;
            communicator.open(std::move(socket));
            mud::http::message request;
            request.type(mud::http::message::type_t::REQUEST);
            request.field<mud::http::version>(mud::http::version::HTTP10);
            request.field<mud::http::method>(mud::http::method::GET);
            request.field<mud::http::uri>("/");
            communicator.ostr() << request << std::flush;
            std::cout << "ok" << std::endl;
        } catch (std::exception& ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
            socket.close();
        } catch (...) {
            std::cerr << "Exception: any\n";
            socket.close();
        }
    });
    communicator.on_receive([&]() {
        try {
            // Receive the response and close
            std::cout << "Receiving response ... " << std::flush;
            mud::http::message response;
            communicator.istr() >> response;
            std::cout << "ok" << std::endl;

            if (response.exists<mud::http::entity_body>()) {
                std::cout << response.field<mud::http::entity_body>().value()
                          << std::endl;
            }
        } catch (std::exception& ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "Exception: any\n";
        }
        communicator.close();

        // Stop the application
        mud::event::event_loop::global().terminate();
    });
    connector.open(mud::io::tcp::endpoint(host, port));

    // Start the event loop
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
