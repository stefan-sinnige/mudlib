#include "mud/core/event_loop.h"
#include "mud/io/tcp.h"
#include <cstring>
#include <iostream>
#include <string>

std::string host = "127.0.0.1";
uint16_t port = 12345;

// ===========================================================================
// The client class
// ===========================================================================

class client: public mud::core::object
{
public:
    // Construction.
    client();
    ~client();

    // Run the client
    void run(const std::string& host, uint16_t port);

private:
    // The handler when connecting
    void on_connected(const mud::core::message& msg);

    // The handler when receiving
    void on_received(const mud::core::message& msg);

    // The client socket
    mud::io::tcp::connector _connector;
    mud::io::tcp::communicator _communicator;
};

client::client()
{
    attach(_connector.connected(), &client::on_connected);
    attach(_communicator.received(), &client::on_received);
}

client::~client() {}

void
client::run(const std::string& host, uint16_t port)
{
    // Setup the connector.
    _connector.open(mud::io::tcp::endpoint(host, port));
    std::cout << "Waiting to connect to server at " << host << ":" << port
              << std::endl;
}

void
client::on_connected(const mud::core::message& /* msg */)
{
    // Get the connected socket
    auto socket = std::move(_connector.connection());

    std::cout << "Connected ["
              << "local: " << socket.source_endpoint().address().str() << ":"
              << socket.source_endpoint().port() << " <-> "
              << "remote: " << socket.destination_endpoint().address().str()
              << ":" << socket.destination_endpoint().port() << "]"
              << std::endl;

    // Open the communication channel (passing over the socket)
    _communicator.open(std::move(socket));

    // Send a message
    std::string msg = "Hello";
    std::cout << "Sending  : " << msg << std::endl;
    _communicator.ostr() << msg << std::endl;
}

void
client::on_received(const mud::core::message& /* msg */)
{
    // Receive
    std::string msg;
    _communicator.istr() >> msg;
    if (_communicator.istr().fail()) {
        _communicator.close();
        std::cout << "Connection closed" << std::endl;
        mud::core::event_loop::global().terminate();
    } else {
        std::cout << "Receiving: " << msg << std::endl;
    }

    mud::core::event_loop::global().terminate();
}

// ===========================================================================
// Main
// ===========================================================================

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
    try {
        client.run(host, port);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the event loop.
    mud::core::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
