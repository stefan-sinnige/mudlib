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

#ifndef _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_
#define _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_

#include <mud/protocols/communicator.h>
#include "mock_device.h"

namespace mock {

/**
 * End communicator to a particular mocked device. This does not contain
 * any messaging but is purely a wrapper for a device's input/output streams.
 */
class end_communicator:
    public mud::protocols::end_communicator<mock::device>
{
public:
    end_communicator();
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    mock::device& device() override;
    mud::core::event& event() override;
private:
    void on_signal(const mud::core::message& msg);
    mud::core::event _event;
    mock::device _device;
};

/**
 * Communicator for a A protocol on a mocked device. This protocol implements
 * the following messages:
 *   A_ENVELOPE
 *     An envelope message that contains contents for an upper communicator
 *   A_REQUEST / A_REPLY
 *     A message that is only interpreted at this communicator
 */
class layer_a_communicator:
    public mud::protocols::layered_communicator<mock::device>
{
public:
    layer_a_communicator(mud::protocols::communicator<mock::device>& lower);
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    void on_received(const mud::core::message&) override;
};

/**
 * Communicator for a B protocol on a mocked device. This protocol implements
 * the following messages:
 *   B_ENVELOPE
 *     An envelope message that contains contents for an upper communicator
 *   B_REQUEST / B_REPLY
 *     A message that is only interpreted at this communicator
 */
class layer_b_communicator:
    public mud::protocols::layered_communicator<mock::device>
{
public:
    layer_b_communicator(mud::protocols::communicator<mock::device>& lower);
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    void on_received(const mud::core::message& msg) override;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_ */
