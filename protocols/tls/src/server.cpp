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

#include "mud/protocols/tls/server.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_TLS_NS

class server::communicator : public mud::io::tcp::communicator
{
public:
    /**
     * @brief Constructor.
     */
    communicator();

    /**
     * @brief Move constructor.
     # @param[in] rhs  The object to move from.
     */
    communicator(communicator&& rhs) = default;

    /**
     * @brief Move assignment.
     # @param[in] rhs  The object to move from.
     */
    communicator& operator=(communicator&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~communicator() = default;

    /**
     * Non-copyable.
     */
    communicator(const communicator&) = delete;
    communicator& operator=(const communicator&) = delete;

private:
    /**
     * Generic TCP receive handler.
     */
    void on_received(const mud::core::message& msg);
};

server::communicator::communicator()
{
    // TODO: attach(...)
}

void
server::communicator::on_received(const mud::core::message& msg)
{
}

/**
 * @brief Implementation of the TLS Server.
 */
class server::impl: public mud::core::object
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor
     */
    ~impl();

    /**
     * Start the server.
     */
    void start(const mud::io::tcp::endpoint& endpoint);

    /**
     * Stop the server.
     */
    void stop();

private:
    /** The TCP acceptor to listen for incoming connections */
    mud::io::tcp::acceptor _acceptor;

    /** The list of TLS communicators. */
    std::list<server::communicator> _communicators;

    /** The handler when accepting new connections. */
    void on_accepted(const mud::core::message& msg);
};

server::impl::impl()
{
    attach(_acceptor.accepted(),  &server::impl::on_accepted);
}

server::impl::~impl()
{
    detach();
    stop();
}

void
server::impl::start(const mud::io::tcp::endpoint& endpoint)
{
    _acceptor.open(endpoint);
}

void
server::impl::stop()
{
    _acceptor.close();
    _communicators.clear();
}

void
server::impl::on_accepted(const mud::core::message& msg)
{
    // Add a peer-communicator handling this new connection
    auto socket = _acceptor.connection();
    if (socket.handle()) {
        // TODO:
        // peer comm(socket, _handler);
        // _communicators.push_back(std::move(comm));
    }
}

void
server::impl_deleter::operator()(server::impl* ptr) const
{
    delete ptr;
}

server::server()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
