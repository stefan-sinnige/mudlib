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
#include "mock_communicator.h"
#include <streambuf>

namespace mock {

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
 *                                 CLOSED
 *                                  
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

channel_streambuf::channel_streambuf(std::streambuf* chain, callback_t cb)
    : _chain(chain)
    , _bufsize(255)
    , _putbacksize(4)
    , _cb(cb)
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

void
channel_streambuf::send(uint8_t cmd, uint8_t len, const uint8_t* data)
{
    _chain->sputc(cmd);
    if (len > 0 && data != nullptr) {
        _chain->sputc(len);
        _chain->sputn((const char*)data, len);
    }
    else {
        _chain->sputc(0);
    }
    _chain->pubsync();
}

int
channel_streambuf::underflow()
{
    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Read the command and length bytes, bail out if none can be read. */
    int cmd = _chain->sbumpc();
    if (cmd == traits_type::eof()) {
        return traits_type::eof();
    }
    int len = _chain->sbumpc();
    if (len == traits_type::eof()) {
        return traits_type::eof();
    }

    /* Process commands. */
    if (cmd == CMD_CONNECT || cmd == CMD_DISCONNECT) {
        _cb(cmd, len);
        return traits_type::eof();
    }
    else
    if (cmd == CMD_DATA) {
        /* Read the data block in the buffer */
        int nread = 0;
        while (nread < len) {
            int nr = _chain->sgetn(_buffer + _putbacksize + nread, len - nread);
            if (nr <= 0) {
                break;
            }
            nread += nr;
        }

        /* Check if block was fully read */
        if (nread != len) {
            _cb(CMD_DISCONNECT, 0);
            return traits_type::eof();
        }

        /* Reset the get buffer pointers with the number of characters read. */
        setg(_buffer + _putbacksize, _buffer + _putbacksize,
             _buffer + _putbacksize + len);
        
        /* Notify new data */
        _cb(cmd, len);

        return traits_type::to_int_type(*gptr());
    }
    
    /* Unknown command */
    _cb(CMD_DISCONNECT, 0);
    return traits_type::eof();
}

int
channel_streambuf::overflow(int c)
{
    /* Insert the character into the buffer */
    if (c != traits_type::eof()) {
        *pptr() = c;
        pbump(1);
    }

    /* Encrypt the block */
    if (sync() != 0) {
        return traits_type::eof();
    }
    return c;
}

int
channel_streambuf::sync()
{
    /* Issue the CMD_DATA header */
    int len = pptr() - pbase();
    _chain->sputc(CMD_DATA);
    _chain->sputc(len);

    /* Write the block */
    int nwritten = 0;
    while (nwritten < len) {
        int nr = _chain->sputn((const char*)pbase() + nwritten, len - nwritten);
        if (nr <= 0) {
            break;
        }
        nwritten += nr;
    }
    if (nwritten <= 0) {
        return -1;
    }
    _chain->pubsync();

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
    _read_buffer = std::make_unique<channel_streambuf>(
            _device.istr().rdbuf(),
            [this](uint8_t cmd, uint8_t len) {
                this->on_command(cmd, len);
            });
    _write_buffer = std::make_unique<channel_streambuf>(
            _device.ostr().rdbuf(),
            [this](uint8_t cmd, uint8_t len) {
                this->on_command(cmd, len);
            });
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
        _write_buffer->send(CMD_CONNECT);
    }
}

void
physical_communicator::disconnect()
{
    if (transition(state_t::CLOSED)) {
        _write_buffer->send(CMD_DISCONNECT);
    }
}

void
physical_communicator::on_command(uint8_t cmd, uint8_t len)
{
    LOG(log);
    if (cmd == CMD_CONNECT) {
        auto previous_state = _state;
        if (transition(state_t::ESTABLISHED)) {
            if (previous_state == state_t::LISTEN) {
                _write_buffer->send(CMD_CONNECT);
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
            mud::core::broker::publish(received());
        }
        else {
            disconnect();
        }
    }
}

void
physical_communicator::on_ready_receive(const mud::core::message& msg)
{
    /* Trigger a read on the device through the stream buffer without
     * consuming anything. */
    (void) _read_buffer->sgetc();
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
        TRACE(log) << "[mock::physical] Transition from "
                   << state_str(_state) << " to "
                   << state_str(state) << std::endl;
        _state = state;
    }
    else {
        TRACE(log) << "[mock::physical] Cannot transition from "
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

/* ======================================================================
 * Protocol communicator
 *
 * The mock-communicator is implementing some mocked protocol. Depending
 * on the layering, the data processed here might have been from an lower
 * evel security layer that may add encryption and would ultimately connect
 * to a physical end-communicator.
 * ====================================================================== */

protocol_communicator::protocol_communicator(communicator<mock::device>& lower)
    : mud::protocols::layered_communicator<mock::device>(lower)
{
}

} // namespace mock

