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

#ifndef _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATOR_H_
#define _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATOR_H_

#include <iostream>
#include <memory>
#include <streambuf>
#include <mud/core/event.h>
#include <mud/protocols/communicator.h>
#include "mock_device.h"

namespace mock {

/**
 * @brief The physical end communicator stream buffer
 * @details
 * The stream buffer associated to the mock physical communicator that is
 * chained to the stream buffer of the underlying mock device. Any data is
 * wrapped in a communication packet.
 */
class channel_streambuf : public std::streambuf
{
public:
    /**
     * Callback function when a command in received.
     */
    typedef std::function<void(uint8_t cmd, uint8_t len)> callback_t;

    /** 
     * @brief Create the stream-buffer.
     * @param chain The chained stream buffer.
     * @param cb The callback when a command message has been received.
     */
    channel_streambuf(std::streambuf* chain, callback_t cb = nullptr);

    /**
     * @brief Destruct the stream buffer.
     */
    virtual ~channel_streambuf();

    /**
     * @brief Send the command.
     * @param cmd The command to send.
     * @param len The length of the data to send.
     * @param data The data to send.
     * @details
     * Reserved for sending direct commands (non-application data) that are
     * only interpreted by the physical communication protoocl. These will be
     * streamed as-is without buffering.
     */
    void send(uint8_t cmd, uint8_t len = 0, const uint8_t* data = nullptr);

protected:
    /**
     * @brief Reading characters from the associated input sequence while
     * decrypting.
     * @return The value pointed to by the @c gptr or @c traits::eof() if no
     * more data is available.
     */
    int underflow() override;

    /**
     * @brief Writing characters to the associated input sequence while
     * encrypting.
     * @param ch The character to store.
     * @return The value @c ch or @c traits::eof() on error.
     */
    int overflow(int c) override;

    /**
     * @brief Synchronise the buffer with the character sequence.
     * @return @c 0 upon success, or @c -1 on failure.
     */
    int sync() override;

private:
    /** The next stream buffer */
    std::streambuf* _chain;

    /** The buffer size (excluding put-back area) */
    const size_t _bufsize;

    /** The putback size */
    const size_t _putbacksize;

    /** The buffer */
    char* _buffer;

    /** The callback function to invoke when a command has been received */
    callback_t _cb;
};

/**
 * @brief Physical end communicator for the mock device.
 * @details
 * End communicator to a particular mocked device. This contains very minimal
 * one-byte messaging.
 */
class physical_communicator:
    public mud::protocols::end_communicator<mock::device>
{
public:
    /**
     * @brief Create the end-communicator to communicate with the physical
     * device directly.
     */
    physical_communicator();

    /**
     * @brief Open the communication channel.
     * @param device The mock device to use.
     */
    void open(mock::device&& device) override;

    /**
     * @brief Close the communication channel.
     */
    void close() override;

    /**
     * @brief The input stream associated to the mock device.
     */
    std::istream& istr() override;

    /**
     * @brief The output stream associated to the mock device.
     */
    std::ostream& ostr() override;

    /**
     * @brief The associated device.
     */
    mock::device& device() override;

    /**
     * @brief The event associated to the device.
     */
    mud::core::event& event() override;

    /**
     * @brief Listen for a connection request.
     */
    void listen();

    /**
     * @brief Send a connection request
     */
    void connect();

    /**
     * @brief Send a disconnection request
     */
    void disconnect();

private:
    /* The communication state */
    enum class state_t {
        CLOSED,
        LISTEN,
        CONNECT,
        ESTABLISHED
    };

    /**
     * @brief Callback when a command has been recived.
     * @param cmd The command
     * @param len The command length
     * @details
     * A physical protoocl command has been received. Note that any data that
     * is associated to the command (when the @ len is not equal to zero) is
     * available in the stream buffer. */
    void on_command(uint8_t cmd, uint8_t len);

    /** Event handler when there is data available. */
    void on_ready_receive(const mud::core::message&);

    /** Transition state, return true if successful transition occurred. */
    bool transition(state_t state);

    /** Return the current starte as a string. */
    const char* state_str(state_t state) const;

    /** The associated mock device event to be registered in the event-loop. */
    mud::core::event _event;

    /** The mock device */
    mock::device _device;

    /** The physical communication stream buffers */
    std::unique_ptr<channel_streambuf> _read_buffer;
    std::unique_ptr<channel_streambuf> _write_buffer;

    /* The stream for reading. */
    std::istream _istr;

    /* The stream for writing. */
    std::ostream _ostr;

    /** The communication state */
    state_t _state;
};

/**
 * @brief Protocol layered communicator.
 * @details
 * A communicator implementing a mock protocol.
 */
class protocol_communicator:
    public mud::protocols::layered_communicator<mock::device>
{
public:
    /**
     * @brief Create the layered-communicator
     */
    protocol_communicator(mud::protocols::communicator<mock::device>& lower);

    /**
     * Destructor.
     */
    virtual ~protocol_communicator() = default;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATOR_H_ */
