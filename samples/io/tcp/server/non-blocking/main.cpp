#include <iostream>
#include <string>
#include <list>
#include "mud/io/kernel_event_loop.h"
#include "mud/io/tcp.h"

// Forward declarations
class server;
class client;

// Global event-handler and list of clients.
mud::io::kernel_event_loop event_loop;
std::list<client*> clients;

// ===========================================================================
// The client class
// ===========================================================================

class client
{
public:
    // Construction
    client(mud::io::tcp::socket&& socket);
    client(client&& rhs) = default;
    ~client();

    // Flag to indicate if client is still connected
    bool
    connected() const {
        return _connected;
    }

private:
    // The handler when receiving data
    void on_receive();

    // The client socket
    mud::io::tcp::socket _socket;

    // Flag to indicate if client is still connected
    bool _connected;
};

client::client(mud::io::tcp::socket&& socket)
    : _socket(std::move(socket)), _connected(true)
{
    // Register the client in the event handler.
    event_loop.register_handler(_socket.handle(),
            std::bind(&client::on_receive, this) );
}

client::~client()
{
    // Deregister the client from the event handler.
    event_loop.deregister_handler(_socket.handle());
}

void
client::on_receive()
{
    // Receive
    std::string msg;
    _socket.istr() >> msg;
    if (_socket.istr().fail()) {
        std::cout << "Connection closed" <<std::endl;
        _connected = false;
        event_loop.deregister_handler(_socket.handle());
        return;
    }
    std::cout << "Read message ... " << std::flush;
    std::cout << msg << std::endl;

    // Response
    std::cout << "Sending response ... " << std::flush;
    _socket.ostr() << "Goodbye" << std::endl;
    std::cout << "done" << std::endl;
}

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
    // The handler when accepting a new connection.
    void on_accept();

    // The listening socket.
    mud::io::tcp::socket _listen;
    mud::io::tcp::acceptor _acceptor;
};

server::server()
    : _acceptor(_listen)
{
    _listen.option<bool, mud::io::ip::nonblocking>(true);
}

server::~server()
{
    // Deregister the listening socket from the event loop.
    event_loop.deregister_handler(_listen.handle());
}

void
server::run(const std::string& host, uint16_t port)
{
    // Setup the acceptor.
    _acceptor.open(mud::io::tcp::endpoint(host, port));
    std::cout << "Waiting for connections on " << host << ":" << port
            << std::endl;

    // Register the handler in the event loop.
    event_loop.register_handler(_listen.handle(),
            std::bind(&server::on_accept, this) );
}

void
server::on_accept()
{
    // Accepting the client that is currently waiting.
    std::cout << "Accepting a waiting client .. " << std::flush;
    mud::io::tcp::socket socket = _acceptor.accept();
    std::cout << "done" << std::endl;

    // Add the client to the list.
    clients.push_back(new client(std::move(socket)));

    // Remove any disconnected clients
    for (auto iter = clients.begin();
            iter != clients.end();
            /* iterator progressed inside loop */)
    {
        if (!(*iter)->connected())
        {
            delete *iter;
            iter = clients.erase(iter);
        }
        else
        {
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
    try
    {
        server.run(host, port);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    // Run the event loop.
    event_loop.loop();
    return 0;
}

/* vi: set ai ts=4 expandtab: */
