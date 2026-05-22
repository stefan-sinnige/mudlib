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

namespace mock {

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

} // namespace mock

