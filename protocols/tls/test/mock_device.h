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

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_ */
