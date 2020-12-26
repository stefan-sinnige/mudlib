#include <iostream>
#include <string>
#include "mud/io/udp.h"
#include "mud/io/kernel_event_loop.h"

std::string host = "127.0.0.1";
uint16_t port = 12345;

// ===========================================================================
// The client class
// ===========================================================================

class client
{
public:
    // Construction.
    client();
    ~client();

    // Run the client
    void run(const std::string& host, uint16_t port);

private:
    // The handler when receiving
    void on_receive();

    // The client socket
    mud::io::udp::socket _socket;
};

client::client()
{
}

client::~client()
{
}

void
client::run(const std::string& host, uint16_t port)
{
    // Register a handler and register a handler for receiving
    mud::io::kernel_event_loop::global().register_handler(
            _socket.handle(),
            mud::io::kernel_event_loop::readiness_t::READING,
            std::bind(&client::on_receive, this) );

    // Send a message
    std::string msg = "Hello";
    std::cout << "Sending  to " << host << ":" << port << ": "
            << msg <<  std::endl;
    _socket.ostr(mud::io::udp::endpoint(host, port)) << msg << std::endl;
}

void
client::on_receive()
{
    // Receive
    std::string msg;
    _socket.istr() >> msg;
    if (_socket.istr().fail()) {
        std::cout << "Connection closed" <<std::endl;
        mud::io::kernel_event_loop::global().deregister_handler(
                _socket.handle(),
                mud::io::kernel_event_loop::readiness_t::READING);
    }
    else
    {
        std::cout << "Connected ["
                << "local: "  << _socket.source_endpoint().address().str()
                << ":"        << _socket.source_endpoint().port() << " <-> "
                << "remote: " << _socket.destination_endpoint().address().str()
                << ":"        << _socket.destination_endpoint().port() << "]"
                << std::endl;
        std::cout << "Receiving: "<<  msg << std::endl;
    }

    // Stop the event-loop, that will exit the application.
    mud::io::kernel_event_loop::global().terminate();
}

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

    // Create the client
    client client;
    try
    {
        client.run(host, port);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    // Run the global event loop.
    mud::io::kernel_event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
