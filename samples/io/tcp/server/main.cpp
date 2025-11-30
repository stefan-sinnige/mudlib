#include "mud/core/event_loop.h"
#include "mud/io/tcp.h"
#include <cstring>
#include <iostream>
#include <list>
#include <string>

// Forward declarations
class server;
class client;

// Global event-handler and list of clients.
std::list<client*> clients;

// ===========================================================================
// The client class
// ===========================================================================

class client : public mud::core::object
{
public:
    // Construction
    client(mud::io::tcp::socket&& socket);
    client(client&& rhs) = default;
    ~client();

    // Flag to indicate if client is still connected
    bool connected() const { return _connected; }

private:
    // The handler when receiving data
    void on_receive(mud::io::tcp::socket&);

    // The client communication channel
    mud::io::tcp::communicator _communicator;

    // Flag to indicate if client is still connected
    bool _connected;
};

client::client(mud::io::tcp::socket&& socket) : _connected(true)
{
    // Open the communication channel
    _communicator.receive_impulse()->attach(this, &client::on_receive);
    _communicator.open(std::move(socket));
}

client::~client() {}

void
client::on_receive(mud::io::tcp::socket& /* unused */)
{
    // Receive
    std::string msg;
    _communicator.istr() >> msg;
    if (_communicator.istr().fail()) {
        std::cout << "Connection closed" << std::endl;
        _connected = false;
        _communicator.close();
        return;
    }
    std::cout << "Receiving: " << msg << std::endl;

    // Response
    msg = "Goodbye";
    std::cout << "Sending  : " << msg << std::endl;
    _communicator.ostr() << msg << std::endl;
    _communicator.close();
}

// ===========================================================================
// The server class
// ===========================================================================

class server: public mud::core::object
{
public:
    // Construction.
    server();
    ~server();

    // Run the server.
    void run(const std::string& host, uint16_t port);

private:
    // The handler when accepting a new connection.
    void on_accept(mud::io::tcp::socket&);

    // The acceptor.
    mud::io::tcp::acceptor _acceptor;
};

server::server()
{
    _acceptor.accept_impulse()->attach(this, &server::on_accept);
}

server::~server() {}

void
server::run(const std::string& host, uint16_t port)
{
    // Setup the acceptor.
    _acceptor.open(mud::io::tcp::endpoint(host, port));
    std::cout << "Waiting for clients to connect to " << host << ":" << port
              << std::endl;
}

void
server::on_accept(mud::io::tcp::socket& socket)
{
    // Accepting the client that is currently waiting.
    std::cout << "Connected ["
              << "local: " << socket.source_endpoint().address().str() << ":"
              << socket.source_endpoint().port() << " <-> "
              << "remote: " << socket.destination_endpoint().address().str()
              << ":" << socket.destination_endpoint().port() << "]"
              << std::endl;

    // Add the client to the list.
    clients.push_back(new client(std::move(socket)));

    // Remove any disconnected clients
    for (auto iter = clients.begin(); iter != clients.end();
         /* iterator progressed inside loop */) {
        if (!(*iter)->connected()) {
            delete *iter;
            iter = clients.erase(iter);
        } else {
            ++iter;
        }
    }
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
    try {
        server.run(host, port);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the global event loop.
    mud::core::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
