#include "mud/event/event_loop.h"
#include "mud/io/udp.h"
#include <cstring>
#include <iostream>
#include <string>

std::string host = "127.0.0.1";
uint16_t port = 12345;

// ===========================================================================
// The client class
// ===========================================================================

class client : public mud::core::object
{
public:
    // Construction.
    client();
    ~client();

    // Run the client
    void run(const std::string& host, uint16_t port);

private:
    // The handler when receiving
    void on_receive(mud::io::udp::socket&);

    // The communication channel
    mud::io::udp::communicator _communicator;
};

client::client()
{
    // Open the communication channel
    _communicator.receive_impulse()->attach(this, &client::on_receive);
}

client::~client() {}

void
client::run(const std::string& host, uint16_t port)
{
    // Setup the communicator
    mud::io::udp::socket socket;
    _communicator.open(std::move(socket));

    // Send a message
    std::string msg = "Hello";
    std::cout << "Sending  to " << host << ":" << port << ": " << msg
              << std::endl;
    _communicator.ostr(mud::io::udp::endpoint(host, port)) << msg << std::endl;
}

void
client::on_receive(mud::io::udp::socket& /* unused */)
{
    // Receive
    std::string msg;
    _communicator.istr() >> msg;
    if (_communicator.istr().fail()) {
        std::cout << "Connection closed" << std::endl;
        _communicator.close();
    } else {
        std::cout << "Connected ["
                  << "local: "
                  << _communicator.source_endpoint().address().str() << ":"
                  << _communicator.source_endpoint().port() << " <-> "
                  << "remote: "
                  << _communicator.destination_endpoint().address().str() << ":"
                  << _communicator.destination_endpoint().port() << "]"
                  << std::endl;
        std::cout << "Receiving: " << msg << std::endl;
    }

    // Stop the event-loop, that will exit the application.
    mud::event::event_loop::global().terminate();
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
    try {
        client.run(host, port);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    // Run the global event loop.
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
