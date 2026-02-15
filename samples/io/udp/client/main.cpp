/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/event_loop.h"
#include "mud/io/interface.h"
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
    void on_received(const mud::core::message& msg);

    // The communication channel
    mud::io::udp::communicator _communicator;
};

client::client()
{
    // Open the communication channel
    attach(_communicator.received(), & client::on_received);
}

client::~client() {}

void
client::run(const std::string& host, uint16_t port)
{
    // Construct the peer endpoint to connect to.
    mud::io::udp::endpoint peer(host, port);

    // If the 'host' is a broadcast address, set its socket option.
    mud::io::udp::socket socket;
    for (auto& netif: mud::io::ip::interface::interfaces()) {
        for (auto& netconf: netif.addresses()) {
            if (netconf.broadcast() == peer.address()) {
                std::cout << "Host is a broadcast address" << std::endl;
                socket.option<bool, mud::io::udp::broadcast>(true);
            }
        }
    }

    // Setup the communicator
    _communicator.open(std::move(socket));

    // Send a message
    std::string msg = "Hello";
    std::cout << "Sending  to " << host << ":" << port << ": " << msg
              << std::endl;
    _communicator.ostr(peer) << msg << std::endl;
}

void
client::on_received(const mud::core::message& /* msg */)
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
    mud::core::event_loop::global().terminate();
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
    mud::core::event_loop::global().loop();

    return 0;
}

/* vi: set ai ts=4 expandtab: */
