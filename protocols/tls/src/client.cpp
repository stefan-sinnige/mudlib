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

#include "mud/tls/client.h"
#include <memory>

BEGIN_MUDLIB_TLS_NS

class client::communicator : public mud::io::tcp::communicator
{
public:
    /**
     * @brief Constructor.
     * @param[in] event_loop  The event-loop to register the socket to.
     */
    communicator(mud::event::event_loop& event_loop =
                                            mud::event::event_loop::global());

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
    void on_receive();
};

client::communicator::communicator(mud::event::event_loop& event_loop)
  : mud::io::tcp::communicator(event_loop)
{
    mud::io::tcp::communicator::on_receive(
        std::bind(&client::communicator::on_receive, this));
}

void
client::communicator::on_receive()
{
}

/**
 * @brief Implementation of the TLS Client.
 */
class client::impl
{
public:
    /**
     * Constructor
     */
    impl(mud::event::event_loop& event_loop);

    /**
     * Destructor
     */
    ~impl();

private:
    /** The event-loop */
    mud::event::event_loop& _event_loop;

    /** The connector that establishes a connection */
    mud::io::tcp::connector _connector;

    /** The communicator once a connection has been established */
    client::communicator _communicator;

    /** The handler when a connection has been established */
    void on_connect(mud::io::tcp::socket&&);
};

client::impl::impl(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _connector(_event_loop),
    _communicator(_event_loop)
{
    _connector.on_connect(
        std::bind(&client::impl::on_connect, this, std::placeholders::_1));
}

client::impl::~impl() {}

void
client::impl::on_connect(mud::io::tcp::socket&& socket)
{
    _communicator.open(std::move(socket));
}

void
client::impl_deleter::operator()(client::impl* ptr) const
{
    delete ptr;
}

client::client(mud::event::event_loop& event_loop)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(event_loop));
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
