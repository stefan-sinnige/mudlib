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

namespace mock {

server::server()
    : _connected(false)
{
}

void
server::open(std::shared_ptr<channel> comms)
{
    /* Create the layered communicators */
    _physical = std::make_unique<physical_communicator>();
    _tls = std::make_unique<tls_server>(*_physical);
    _protocol = std::make_unique<protocol_communicator>(*_tls);

    /* Setup the event notifications */
    attach(_protocol->connected(), &server::on_connected);
    attach(_protocol->disconnected(), &server::on_disconnected);

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
server::send()
{
    _protocol->ostr() << "Aenean vitae dui quis leo venenatis rhoncus. Nullam diam sem, blandit ac tempus id, volutpat malesuada massa. Duis pellentesque, velit quis vulputate mollis, arcu purus pharetra velit, ac condimentum lorem metus nec metus. Proin a ipsum quis nunc vestibulum tempus. Integer fringilla ligula velit, eget porttitor risus lobortis consequat. Mauris turpis augue, efficitur faucibus mi vel, elementum posuere nunc. Pellentesque pretium turpis quis nulla fermentum, commodo dictum massa accumsan. Morbi tempus porttitor tristique. Mauris ornare at quam sed blandit. Ut a sagittis mi. In hac habitasse platea dictumst. Donec sed quam eu odio lacinia egestas ut eget dui. Nam convallis massa diam, sit amet fringilla nisl finibus ac. Integer venenatis mi eu nisi blandit, et gravida ex efficitur. Donec dui elit, semper in dui nec, viverra luctus metus. Sed eu euismod lectus, sed viverra justo. Integer vel ex congue, luctus lacus sit amet, pharetra risus. Nulla pharetra est eget velit pulvinar, nec tincidunt sem tincidunt. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla facilisi. Cras in ligula nibh. Mauris aliquet dolor massa, nec ultricies neque maximus non. Donec pellentesque in risus nec tristique. Nunc blandit accumsan diam, id elementum mauris bibendum vitae. Aenean quis mi ut arcu posuere posuere ac id nibh." << std::endl;
}

void server::receive()
{
    std::string str;
    std::getline(_protocol->istr(), str);
    std::cout << "==> Received from server: " << str << std::endl;
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

std::future<void>
server::disconnected_future()
{
    return _disconnected_promise.get_future();
}

void
server::on_connected(const mud::core::message& msg)
{
    _connected = true;
    _connected_promise.set_value();
}

void
server::on_disconnected(const mud::core::message& msg)
{
    _connected = false;
    _disconnected_promise.set_value();
}

client::client()
    : _connected(false)
{
}

void
client::open(std::shared_ptr<channel> comms)
{
    /* Create the layered communicators */
    _physical = std::make_unique<physical_communicator>();
    _tls = std::make_unique<tls_client>(*_physical);
    _protocol = std::make_unique<protocol_communicator>(*_tls);

    /* Setup the event notifications */
    attach(_protocol->connected(), &client::on_connected);
    attach(_protocol->disconnected(), &client::on_disconnected);

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
client::send()
{
    _protocol->ostr() << "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus dignissim risus sit amet odio varius, quis fermentum lorem aliquet. Aenean nec leo elit. Vivamus ante purus, lobortis id elit mollis, ultricies tempor erat. Phasellus eget ipsum sit amet mauris dignissim convallis ut in odio. Aliquam commodo tellus vitae consectetur blandit. Aenean tristique purus nec neque dapibus, ut condimentum quam convallis. Donec pharetra tincidunt tellus sed feugiat. Donec molestie lacinia placerat. Etiam in purus nec enim luctus scelerisque. Pellentesque blandit dolor vel lectus lobortis porta. Phasellus at feugiat ligula. Cras facilisis non dolor eget rutrum. Vivamus pretium quis erat convallis porta. Suspendisse potenti. Ut suscipit ultricies dapibus. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Proin et mi in lectus placerat viverra vitae eu odio. Maecenas mattis vitae justo quis efficitur. Donec sed tincidunt justo. Maecenas malesuada finibus lectus sed blandit. Nulla facilisi. Cras congue libero at libero sollicitudin ullamcorper. Donec non scelerisque massa, vitae ultricies mi." << std::endl;
}

void
client::receive()
{
    std::string str;
    std::getline(_protocol->istr(), str);
    std::cout << "==> Received from client: " << str << std::endl;
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

void
client::on_connected(const mud::core::message& msg)
{
    _connected = true;
    _connected_promise.set_value();
}

void
client::on_disconnected(const mud::core::message& msg)
{
    _connected = false;
    _disconnected_promise.set_value();
}

} // namespace mock
