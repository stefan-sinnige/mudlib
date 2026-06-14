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

#include "mock_device.h"
#include "mud/core/event_loop.h"

namespace mock {

/* ==========================================================================
 * Mock device
 * ========================================================================== */

device::device(channel::entity side, std::shared_ptr<channel> comm)
    : _entity(side)
    , _channel(comm)
{
}

std::istream&
device::istr()
{
    if (_entity == channel::entity::entity_a) {
        return _channel->first().istr();
    }
    else {
        return _channel->second().istr();
    }
}

std::ostream&
device::ostr()
{
    if (_entity == channel::entity::entity_a) {
        return _channel->second().ostr();
    }
    else {
        return _channel->first().ostr();
    }
}

std::shared_ptr<mud::core::handle>
device::read_handle()
{
    if (_entity == channel::entity::entity_a) {
        return _channel->first().read_handle();
    }
    else {
        return _channel->second().read_handle();
    }
}

std::shared_ptr<mud::core::handle>
device::write_handle()
{
    if (_entity == channel::entity::entity_a) {
        return _channel->second().write_handle();
    }
    else {
        return _channel->first().write_handle();
    }
}

/* ==========================================================================
 * Physical communicator
 *
 * The end-communicator communicates directly over the physical communication
 * channel and would usually transmit data as-is. Depending on the layering,
 * the data may be encrypted.
 *
 * This physical communicator interacts directly with the mock device through
 * a rudimentary protocol and state machine. The protocol is a two-byte header
 * consisting of a one-byte command code and a one-byte length. Data packets
 * are therefore always transferred in blocks of at most 255 bytes in size.
 *
 * The state diagram of a connection supports a single bi-directional channel
 * between two peers, one is the server and the other the client.
 *
 *                                 CLOSED  ←──────┐
 *                                  │  │          │
 *                          listen  │  │  connect │
 *                                  ↓  ↓          │
 *                             LISTEN  CONNECT    │  -
 *                                  │  │          │
 *                               +  │  │  +       │
 *                                  ↓  ↓          │
 *                               ESTABLISHED ─────┘
 *                                  │  ↑
 *                                  └──┘
 *                                   *
 *
 * A dedicated stream buffer is used to implement the block-based transfer
 * for both input and output streams.
 * ========================================================================== */

/**
 * The supported commands
 */
uint8_t CMD_CONNECT = '+';
uint8_t CMD_DISCONNECT = '-';
uint8_t CMD_DATA = '*';

channel_streambuf::channel_streambuf(data_provider& provider)
    : _bufsize(255)
    , _putbacksize(4)
    , _provider(provider)
{
    /* Allocate the data structure */
    _buffer = new char[_bufsize + _putbacksize];

    /* Set the get area pointers for reading */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize);

    /* Set the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
}

channel_streambuf::~channel_streambuf()
{
    delete[] _buffer;
}

int
channel_streambuf::underflow()
{
    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Read more data from the provider */
    int nread = _provider.underflow((uint8_t*)_buffer + _putbacksize, _bufsize);
    if (nread <= 0) {
        return traits_type::eof();
    }
    
    /* Reset the get buffer pointers with the number of characters read. */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize + nread);
    return traits_type::to_int_type(*gptr());
}

int
channel_streambuf::overflow(int c)
{
    /* Insert the character into the buffer */
    if (c != traits_type::eof()) {
        *pptr() = c;
        pbump(1);
    }

    /* Send the block */
    if (sync() != 0) {
        return traits_type::eof();
    }
    return c;
}

int
channel_streambuf::sync()
{
    /* Pass the data to send to the provider */
    int len = pptr() - pbase();
    int nwritten = 0;
    while (nwritten < len) {
        int nr = _provider.overflow(
                (const uint8_t*)pbase() + nwritten,
                len - nwritten);
        if (nr <= 0) {
            break;
        }
        nwritten += nr;
    }
    if (nwritten <= 0) {
        return -1;
    }

    /* Reset the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
    return 0;
}

physical_communicator::physical_communicator()
    : _state(state_t::CLOSED)
    , _istr(nullptr)
    , _ostr(nullptr)
{
}

void
physical_communicator::open(mock::device&& device)
{
    _device = std::move(device);
    _state = state_t::CLOSED;

    /* Associate a new stream buffer to this device */
    _read_buffer = std::make_unique<channel_streambuf>(*this);
    _write_buffer = std::make_unique<channel_streambuf>(*this);
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());

    /* Register the device to the event-loop */
    _event = mud::core::event(mud::core::uuid(), _device.read_handle(),
            mud::core::event::signal_type::READING);
    attach(_event.topic(), &physical_communicator::on_ready_receive);
    mud::core::event_loop::global().add(std::move(_event));
}

void
physical_communicator::close()
{
    _state = state_t::CLOSED;
}

std::istream&
physical_communicator::istr()
{
    return _istr;
}

std::ostream&
physical_communicator::ostr()
{
    return _ostr;
}

mock::device&
physical_communicator::device()
{
    return _device;
}

mud::core::event&
physical_communicator::event()
{
    return _event;
}

void
physical_communicator::listen()
{
    if (transition(state_t::LISTEN)) {
    }
}

void
physical_communicator::connect()
{
    if (transition(state_t::CONNECT)) {
        _device.ostr().put(CMD_CONNECT).put(0).flush();;
    }
}

void
physical_communicator::disconnect()
{
    if (transition(state_t::CLOSED)) {
        _device.ostr().put(CMD_DISCONNECT).put(0).flush();;
    }
}

ssize_t
physical_communicator::underflow(uint8_t* ptr, size_t sz)
{
    /* Continue processing commands until we have received a data packet or
     * until we have detected an error. */
    while (true)
    {
        /* If not connected, bail out */
        if (_state != state_t::ESTABLISHED) {
            LOG(log);
            ERROR(log) << "no connected mock device" << std::endl;
            return -1;
        }

        /* Read the command and length bytes, bail out if none can be read. */
        int cmd = _device.istr().get();
        if (cmd == std::istream::traits_type::eof()) {
            LOG(log);
            ERROR(log) << "unable to read mock command type" << std::endl;
            disconnect();
            return -1;
        }
        int len = _device.istr().get();
        if (len == std::istream::traits_type::eof()) {
            LOG(log);
            ERROR(log) << "unable to read mock command length" << std::endl;
            disconnect();
            return -1;
        }

        /* If this is a data command, extract the data. */
        if (cmd == CMD_DATA) {
            if (len > sz) {
                LOG(log);
                ERROR(log) << "mock buffer size of " << sz << " smaller than "
                           << "available data size " << len << std::endl;
                return -1;
            }
            if (!_device.istr().read((char*)ptr, len)) {
                LOG(log);
                ERROR(log) << "unable to read mock data" << std::endl;
                return -1;
            }
            return _device.istr().gcount();
        }
    }
    return -1;
}

ssize_t
physical_communicator::overflow(const uint8_t* ptr, size_t sz)
{
    /* If not connected, we cannot send data */
    if (_state != state_t::ESTABLISHED) {
        LOG(log);
        ERROR(log) << "no connected mock device" << std::endl;
        return -1;
    }

    /* Send in multiple of 255 packets */
    int nwritten = 0;
    while (nwritten < sz) {
        int len = (sz - nwritten) < 255 ? sz - nwritten : 255;
        _device.ostr().put(CMD_DATA).put(len);
        if (!_device.ostr()) {
            LOG(log);
            ERROR(log) << "unable to send mock data" << std::endl;
            return -1;
        }
        if (!_device.ostr().write((const char*)ptr + nwritten, len)) {
            LOG(log);
            ERROR(log) << "unable to send mock data" << std::endl;
            return -1;
        }
        _device.ostr().flush();
        nwritten += len;
    }
    return nwritten;
}

int
physical_communicator::command(uint8_t cmd, uint8_t len)
{
    LOG(log);
    TRACE(log) << "mock command [" << (char)cmd << "] " <<  std::hex
               << std::setw(2) << std::setfill('0') << len << std::dec
               << std::endl;

    if (cmd == CMD_CONNECT) {
        auto previous_state = _state;
        if (transition(state_t::ESTABLISHED)) {
            if (previous_state == state_t::LISTEN) {
                _device.ostr().put(CMD_CONNECT).put(0).flush();;
            }
            mud::core::broker::publish(connected());
        }
        else {
            disconnect();
        }
    }
    else
    if (cmd == CMD_DISCONNECT) {
        if (transition(state_t::CLOSED)) {
            mud::core::broker::publish(disconnected());
        }
        else {
            disconnect();
        }
    }
    else 
    if (cmd == CMD_DATA) {
        if (_state == state_t::ESTABLISHED) {
            _device.istr().putback(len).putback(cmd);
            mud::core::broker::publish(received());
        }
        else {
            disconnect();
        }
    }
    else {
        ERROR(log) << "Unknown mock command [" << (char)cmd << "]" << std::endl;
        disconnect();
        return false;
    }
    return true;
}

void
physical_communicator::on_ready_receive(const mud::core::message& msg)
{
    /* Read the command and length bytes, bail out if none can be read. */
    int cmd = _device.istr().get();
    if (cmd == std::istream::traits_type::eof()) {
        LOG(log);
        ERROR(log) << "unable to read mock command type" << std::endl;
        disconnect();
        return;
    }
    int len = _device.istr().get();
    if (len == std::istream::traits_type::eof()) {
        LOG(log);
        ERROR(log) << "unable to read mock command length" << std::endl;
        disconnect();
        return;
    }

    /* Handle the command */
    command(cmd, len);
}

bool
physical_communicator::transition(state_t state)
{
    LOG(log);

    /* Determine if a transition is allowed. */
    bool result = false;
    switch (_state) {
        case state_t::CLOSED:
            result = state == state_t::LISTEN || state == state_t::CONNECT;
            break;
        case state_t::LISTEN:
            result = state == state_t::ESTABLISHED || state == state_t::CLOSED; 
            break;
        case state_t::CONNECT:
            result = state == state_t::ESTABLISHED || state == state_t::CLOSED; 
            break;
        case state_t::ESTABLISHED:
            result = state == state_t::CLOSED;
            break;
    }

    /* Log the result and perform any transition */
    if (result) {
        TRACE(log) << "transition mock communication state from "
                   << state_str(_state) << " to "
                   << state_str(state) << std::endl;
        _state = state;
    }
    else {
        TRACE(log) << "cannot transition mock communication state from "
                   << state_str(_state) << " to "
                   << state_str(state) << std::endl;
    }
    return result;
}

const char*
physical_communicator::state_str(state_t state) const
{
    switch (state) {
        case state_t::CLOSED:
            return "CLOSED";
        case state_t::LISTEN:
            return "LISTEN";
        case state_t::CONNECT:
            return "CONNECT";
        case state_t::ESTABLISHED:
            return "ESTABLISHED";
    }
    return "UNKNOWN";
}

} // namespace mock

