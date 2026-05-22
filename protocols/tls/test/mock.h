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

#ifndef _MUDLIB_PROTOOCOLS_MOCK_H_
#define _MUDLIB_PROTOOCOLS_MOCK_H_

#include <condition_variable>
#include <future>
#include <memory>
#include <mud/core/object.h>
#include <mud/protocols/communicator.h>
#include <mud/protocols/tls/communicator.h>
#include "mock_device.h"
#include "mock_communicator.h"

namespace mock {

/** Type definition for a TLS communicator on a mock device */
typedef mud::tls::communicator::client<mock::device> tls_client;
typedef mud::tls::communicator::server<mock::device> tls_server;

/**
 * @brief The server side.
 * @details
 * This class defines the server-side of a mock communication protocol that is
 * enveloped by a TLS layer.
 */
class server : public mud::core::object
{
public:
    /**
     * @brief Create the server.
     */
    server();

    /**
     * @brief Open the server.
     * @param comms The communication channel.
     */
    void open(std::shared_ptr<channel> comms);

    /**
     * @brief Start to listen for a client connection.
     */
    void listen();

    /**
     * @brief Close the server
     */
     void close();

    void send();
    void receive();

    /**
     * @brief Return true if the server is connected.
     */
    bool connected() const;

    /**
     * @brief Return the connected future
     * @return The future that is set when the connected has been established.
     */
    std::future<void> connected_future();

    /**
     * @brief Return the disconnected future
     * @return The future that is set when the connected has been closed.
     */
    std::future<void> disconnected_future();

private:
    /** Event notification handlers */
    void on_connected(const mud::core::message& msg);
    void on_disconnected(const mud::core::message& msg);

    /** The server-side physical device communicator */
    std::unique_ptr<physical_communicator> _physical;

    /** The server-side TLS communicator */
    std::unique_ptr<tls_server> _tls;

    /** The server-side protocol communicator */
    std::unique_ptr<protocol_communicator> _protocol;

    /** Connection state */
    bool _connected;

    /** Connected promise */
    std::promise<void> _connected_promise;

    /** Disconnected promise */
    std::promise<void> _disconnected_promise;
};

/**
 * @brief The client side.
 * @details
 * This class defines the client-side of a mock communication protocol that is
 * enveloped by a TLS layer.
 */
class client : public mud::core::object
{
public:
    /**
     * @brief Create the client.
     */
    client();

    /**
     * @brief Open the client.
     * @param comms The communication channel.
     */
    void open(std::shared_ptr<channel> comms);

    /**
     * @brief Connect the client to the server
     */
    void connect();

    /**
     * @brief Close the client
     */
     void close();

    void send();
    void receive();

    /**
     * @brief Return true if the client is connected.
     */
    bool connected() const;

    /**
     * @brief Return the connected future
     * @return The future that is set when the connected has been established.
     */
    std::future<void> connected_future();

    /**
     * @brief Return the disconnected future
     * @return The future that is set when the connected has been closed.
     */
    std::future<void> disconnected_future();

private:
    /** Event notification handlers */
    void on_connected(const mud::core::message& msg);
    void on_disconnected(const mud::core::message& msg);

    /** The client-side physical device communicator */
    std::unique_ptr<physical_communicator> _physical;

    /** The client-side TLS communicator */
    std::unique_ptr<tls_client> _tls;

    /** The client-side protocol communicator */
    std::unique_ptr<protocol_communicator> _protocol;

    /** Connection state */
    bool _connected;

    /** Connected promise */
    std::promise<void> _connected_promise;

    /** Disconnected promise */
    std::promise<void> _disconnected_promise;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_H_ */
