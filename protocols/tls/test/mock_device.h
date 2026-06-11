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

#ifndef _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_
#define _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_

#include <utility>
#include <streambuf>
#include <mud/core/event.h>
#include <mud/protocols/communicator.h>
#include <mud/io/pipe.h>

namespace mock {

/**
 * A mock communication channel consisting of two uni-directional pipes. To
 * use it as a bi-directional channel, one end would connect to the read-handle
 * of one pipe and write-handle of the other, while the other end would use the
 * opposite handles.
 */
class channel
{
public:
    /**
     * The two peer entities. Each channel entity should only have one
     * associated device.
     */
    enum class entity {
        entity_a,
        entity_b
    };

    /**
     * @brief Create a channel.
     */
    channel() = default;

    /**
     * @brief Return the first pipe.
     */
    mud::io::pipe& first() { return _channel.first; }

    /**
     * @brief Return the second pipe.
     */
    mud::io::pipe& second() { return _channel.second; }

private:
    /** The physical channel. */
    std::pair<mud::io::pipe, mud::io::pipe> _channel;
};

/**
 * A mock device that defines one end of a communication connection.
 */
class device
{
public:
    /**
     * @brief Unconnected device.
     */
    device() = default;

    /**
     * @brief Create a device using one side of the channel.
     * @param side The entity side of the communication channel.
     * @param comm The communication channel.
     */
    device (channel::entity side, std::shared_ptr<channel> comm);

    /**
     * @brief Copy constructor not allowed.
     */
    device (const device& rhs) = delete;

    /**
     * @brief Move constructor.
     */
    device (device&& rhs) = default;

    /**
     * @brief Copy assignment not allowed.
     */
    device& operator=(const device& rhs) = delete;

    /**
     * @brief Move assignment.
     */
    device& operator=(device&& rhs) = default;

    /**
     * @brief Get the input stream.
     * @return The input stream.
     */
    std::istream& istr();

    /**
     * @brief Get the output stream.
     * @return The output stream.
     */
    std::ostream& ostr();

    /**
     * @brief The handle of the read end of the communication channel.
     */
    std::shared_ptr<mud::core::handle> read_handle();

    /**
     * @brief The handle of the write end of the communication channel.
     */
    std::shared_ptr<mud::core::handle> write_handle();

private:
    /* The channel entity */
    channel::entity _entity;

    /** The communication channel */
    std::shared_ptr<channel> _channel;
};

/**
 * @brief The data provider for the @c channel_streambuf.
 * @details
 * The data provider is the interface that provides the underlying data
 * services to the channel streambuffer and is used to both
 *
 *  - Request for more data (through the @c underflow function)
 *  - Send available data (through the @c overflow function)
 *
 * To simplify the implementation, the @c underflow and @c overflow always
 * operate on maximum 255 characters such that it can align with the maximum
 * data-size of the underlying communication channel without requiring
 * additional buffering.
 */
class data_provider
{
public:
    /**
     * @brief Request for more data
     * @param ptr The buffer pointer to store the data in.
     * @param sz The (maximum) number of bytes of the data to receive.
     * @return The number of bytes written (always smaller or equal to @c sz),
     * or negative if there is an error.
     */
    virtual ssize_t underflow(uint8_t* ptr, size_t sz) = 0;

    /**
     * @brief Send data.
     * @param ptr The buffer pointer to data to send.
     * @param sz The number of bytes of the data to send.
     * @return The number of bytes sent (always smaller or equal to @c sz),
     * or negative if there is an error.
     */
    virtual ssize_t overflow(const uint8_t* ptr, size_t sz) = 0;
};

/**
 * @brief The physical end communicator stream buffer
 * @details
 * The stream buffer associated to the mock physical communicator. Any data
 * that is physically to be sent or received is through the @c channel_provider
 * interface.
 */
class channel_streambuf : public std::streambuf
{
public:
    /** 
     * @brief Create the stream-buffer.
     * @param provider The data provider.
     */
    channel_streambuf(data_provider& provider);

    /**
     * @brief Destruct the stream buffer.
     */
    virtual ~channel_streambuf();

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
    /** The buffer size (excluding put-back area) */
    const size_t _bufsize;

    /** The putback size */
    const size_t _putbacksize;

    /** The buffer */
    char* _buffer;

    /** The data provider. */
    data_provider& _provider;
};

/**
 * @brief Physical end communicator for the mock device.
 * @details
 * End communicator to a particular mocked device. This contains very minimal
 * one-byte messaging.
 */
class physical_communicator:
    public mud::protocols::end_communicator<mock::device>,
    public data_provider
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
     * @brief Request for more data
     * @param ptr The buffer pointer to store the data in.
     * @param sz The (maximum) number of bytes of the data to receive.
     * @return The number of bytes written (always smaller or equal to @c sz),
     * or negative if there is an error.
     */
    virtual ssize_t underflow(uint8_t* ptr, size_t sz) override;

    /**
     * @brief Send data.
     * @param ptr The buffer pointer to data to send.
     * @param sz The number of bytes of the data to send.
     * @return The number of bytes sent (always smaller or equal to @c sz),
     * or negative if there is an error.
     */
    virtual ssize_t overflow(const uint8_t* ptr, size_t sz) override;

    /**
     * @brief Process a non-data command.
     * @param cmd The command as received.
     * @param sz The command length as received.
     * @return Zero if the command is handled sucessfully, or negative if there
     * is an error.
     * @details
     * While processing the command, the communication state can change as a
     * result of the received command, or as a result of handling an error.
     * Furthermore, the processing may also result in sending messages to the
     * connected peer.
     */
    int command(uint8_t cmd, uint8_t sz);

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

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_ */
