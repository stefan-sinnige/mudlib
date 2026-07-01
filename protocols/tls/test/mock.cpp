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

#include "mock.h"
#include <mud/core/endian.h>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

namespace mock {

std::istream&
operator>>(std::istream& istr, message& msg)
{
    uint16_t length;
    istr.read((char*)&length, 2);
    length = endian::convert(endian_t::big, endian::native(), length);
    std::string str;
    str.resize(length);
    istr.read(str.data(), length);
    msg.contents(str);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const message& msg)
{
    uint16_t length = endian::convert(endian::native(), endian_t::big,
            (uint16_t)msg.contents().size());
    ostr.write((const char*)&length, 2);
    ostr.write(msg.contents().data(), msg.contents().size());
    return ostr;
}

/* ======================================================================
 * Server
 * ====================================================================== */

server::server()
    : _connected(false)
{
}

void
server::open(std::shared_ptr<channel> comms)
{
    /* Create the layered communicators */
    _physical = std::make_unique<physical_communicator>();
    _tls = std::make_unique<tls_communicator_server>(*_physical);
    _protocol = std::make_unique<protocol_communicator>(*_tls);

    /* Setup the event notifications */
    attach(_protocol->connected(), &server::on_connected);
    attach(_protocol->disconnected(), &server::on_disconnected);
    attach(_protocol->received(), &server::on_received);

    /* Create the physical device and pass it to the communicators */
    mock::device device(channel::entity::entity_a, comms);
    _protocol->open(std::move(device));
}

void
server::listen()
{
    _physical->listen();    
}

void
server::close()
{
}

void
server::send(const mock::message& msg)
{
    _protocol->ostr() << msg << std::flush;
}

bool
server::connected() const
{
    return _connected;
}

std::future<void>
server::connected_future()
{
    return _connected_promise.get_future();
}

std::future<mock::message>
server::received_future()
{
    return _received_promise.get_future();
}

std::future<void>
server::disconnected_future()
{
    return _disconnected_promise.get_future();
}

void
server::on_connected(const mud::core::message&)
{
    _connected = true;
    _connected_promise.set_value();
}

void
server::on_disconnected(const mud::core::message&)
{
    _connected = false;
    _disconnected_promise.set_value();
}

void
server::on_received(const mud::core::message&)
{
    mock::message mock_msg;
    _protocol->istr() >> mock_msg;
    _received_promise.set_value(mock_msg);
}

/* ======================================================================
 * Client
 * ====================================================================== */

client::client()
    : _connected(false)
{
}

void
client::open(std::shared_ptr<channel> comms)
{
    /* Create the layered communicators */
    _physical = std::make_unique<physical_communicator>();
    _tls = std::make_unique<tls_communicator_client>(*_physical);
    _protocol = std::make_unique<protocol_communicator>(*_tls);

    /* Setup the event notifications */
    attach(_protocol->connected(), &client::on_connected);
    attach(_protocol->disconnected(), &client::on_disconnected);
    attach(_protocol->received(), &client::on_received);

    /* Create the physical device and pass it to the communicators */
    mock::device device(channel::entity::entity_b, comms);
    _protocol->open(std::move(device));
}

void
client::connect()
{
    _physical->connect();    
}

void
client::close()
{
}

void
client::send(const mock::message& msg)
{
    _protocol->ostr() << msg << std::flush;
}

bool
client::connected() const
{
    return _connected;
}

std::future<void>
client::connected_future()
{
    return _connected_promise.get_future();
}

std::future<void>
client::disconnected_future()
{
    return _disconnected_promise.get_future();
}

std::future<mock::message>
client::received_future()
{
    return _received_promise.get_future();
}

void
client::on_connected(const mud::core::message&)
{
    _connected = true;
    _connected_promise.set_value();
}

void
client::on_disconnected(const mud::core::message&)
{
    _connected = false;
    _disconnected_promise.set_value();
}

void
client::on_received(const mud::core::message&)
{
    mock::message mock_msg;
    _protocol->istr() >> mock_msg;
    _received_promise.set_value(mock_msg);
}

} // namespace mock
