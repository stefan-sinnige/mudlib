#include <iostream>
#include <string>
#include <list>
#include "mud/event/event_loop.h"
#include "mud/io/udp.h"

// Forward declarations
class server;

// ===========================================================================
// The server class
// ===========================================================================

class server
{
public:
    // Construction.
    server();
    ~server();

    // Run the server.
    void run(const std::string& host, uint16_t port);

private:
    // The handler when data is ready to be received.
    void on_receive();

    // The communicator
    mud::io::udp::communicator _communicator;
};

server::server()
{
    // Open the communication channel
    _communicator.on_receive(std::bind(&server::on_receive, this));
}

server::~server()
{
}

void
server::run(const std::string& host, uint16_t port)
{
    // Setup the comunicator to listen for data
    mud::io::udp::socket socket;
    socket.bind(mud::io::udp::endpoint(host, port));
    socket.option<bool, mud::io::ip::nonblocking>(true);
    _communicator.open(std::move(socket));
    std::cout << "Waiting for clients to send data to "
            << host << ":" << port << std::endl;
}

void
server::on_receive()
{
    // Receive
    std::string msg;
    _communicator.istr() >> msg;
    if (_communicator.istr().fail()) {
        std::cout << "Connection closed" <<std::endl;
        _communicator.close();
        return;
    }

    std::cout
            << "Connected ["
                    << "local: "  << _communicator.source_endpoint().address().str()
                    << ":"        << _communicator.source_endpoint().port() << " <-> "
                    << "remote: " << _communicator.destination_endpoint().address().str()
                    << ":"        << _communicator.destination_endpoint().port() << "]"
                    << std::endl;
    std::cout << "Receiving: " << msg << std::endl;

    // Response
    msg = "Goodbye";
    std::cout << "Sending  : " << msg <<  std::endl;
    _communicator.ostr() << msg << std::endl;
}

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    // Command line arguments processing.
    std::string host = "127.0.0.1";
    uint16_t port = 12345;
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

    // Create the server.
    server server;
    try
    {
        server.run(host, port);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the global event loop.
    mud::event::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
