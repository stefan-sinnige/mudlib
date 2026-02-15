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

#include <functional>
#include <sstream>
#include <mud/core/event.h>
#include <mud/core/handle.h>

namespace mock {

class device
{
public:
    /**
     * @brief Create a device placeholder.
     *
     * @details
     * Create a placeholder for an actual device. The placeholder cannot be
     * used as it would not have been associated with an actual device. Only
     * when the device is moved from an actual device can it be utilised.
     */
    device();

    /**
     * @brief Create a mock device, utilising the provided buffer.
     *
     * @details
     * Create a mock device that uses the external string buffers for its
     * input/output streams.
     *
     * @param istr The buffer that the device uses as an input stream.
     * @param ostr The buffer that the device uses as an output stream.
     * @param recv_signal Signal to notify the device that there is something
     * to read.
     */
    device(std::istream& istr, std::ostream& ostr);

    /**
     * @brief Move constructor.
     *
     * @param other The object to move from.
     */
    device(device&& other);

    /**
     * @brief Move assignment.
     *
     * @param other The object to assign from.
     */
    device& operator=(device&& other);

    /** Destructor */
    virtual ~device();

    /**
     * Non-copyable
     */
    device(const device&) = delete;
    device& operator=(const device&) = delete;

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
     * @brief The mock file-descriptor handle.
     */
    std::shared_ptr<mud::core::handle> handle();

    /**
     * @brief The event that is triggered on a signal.
     */
    mud::core::event& signal();

    /**
     * @brief Mock that the device has something to read.
     * @details Invoke the hanlder taht there is something to rea.
     */
    void simulate_signal();

private:
    /** The implementation and its unique reference. */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_ */
