#include "mud/event/event_loop.h"
#include "mud/io/tcp.h"
#include <cstring>
#include <iostream>
#include <string>

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
    // The handler when connecting
    void on_connect(mud::io::tcp::socket&&);

    // The handler when receiving
    void on_receive();

    // The client socket
    mud::io::tcp::connector _connector;
    mud::io::tcp::communicator _communicator;
};

client::client()
{
    _connector.on_connect(
        std::bind(&client::on_connect, this, std::placeholders::_1));
    _communicator.on_receive(std::bind(&client::on_receive, this));
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
client::on_connect(mud::io::tcp::socket&& socket)
{
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
client::on_receive()
{
    // Receive
    std::string msg;
    _communicator.istr() >> msg;
    if (_communicator.istr().fail()) {
        _communicator.close();
        std::cout << "Connection closed" << std::endl;
        mud::event::event_loop::global().terminate();
    } else {
        std::cout << "Receiving: " << msg << std::endl;
    }

    mud::event::event_loop::global().terminate();
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
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
