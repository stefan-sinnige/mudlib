#include <iostream>
#include <string>
#include "mud/io/tcp.h"

std::string host = "127.0.0.1";
uint16_t port = 12345;

int
main(int argc, char** argv)
{
    // Command line arguments
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

    // Only handle one connection
    try
    {
        // Setup the acceptor
        mud::io::tcp::socket listen;
        mud::io::tcp::acceptor acceptor(listen);
        acceptor.open(mud::io::tcp::endpoint(host, port));

        // Accepting incoming connection
        std::cout << "Waiting for client to connect to " << host << ":"
                << port << " ... " << std::flush;
        mud::io::tcp::socket client = acceptor.accept();
        std::cout << "connected" << std::endl;

        // Wait for a message
        std::string msg;
        std::cout << "Waiting to receive message ... " << std::flush;
        client.istr() >> msg;
        std::cout << msg << std::endl;

        // Respond
        std::cout << "Sending response ... " << std::flush;
        client.ostr() << "Goodbye" << std::endl;
        std::cout << "done" << std::endl;

        // That's it
        std::cout << "Closing" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

/* vi: set ai ts=4 expandtab: */
