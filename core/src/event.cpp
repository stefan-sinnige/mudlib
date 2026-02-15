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

#include <mud/core/event.h>
#include <mud/core/message.h>

BEGIN_MUDLIB_CORE_NS

event::event(const mud::core::uuid& topic,
             std::shared_ptr<mud::core::handle> handle,
             signal_type mask)
  : _topic(topic), _handle(handle), _mask(mask)
{
}

event::event(event&& rhs)
  : _topic(rhs._topic)
  , _handle(rhs._handle)
  , _mask(rhs._mask)
{
}

event&
event::operator=(event&& rhs) {
    if (this != &rhs) {
        _topic = rhs._topic;
        _handle = rhs._handle;
        _mask = rhs._mask;
    }
    return *this;
}

bool
event::operator==(const event& rhs) const
{
    return _topic == rhs._topic;
}

bool
event::operator!=(const event& rhs) const
{
    return !operator==(rhs);
}

const mud::core::uuid&
event::topic() const
{
    return _topic;
}

std::shared_ptr<mud::core::handle>
event::handle() const
{
    return _handle;
};

event::signal_type
event::mask() const
{
    return _mask;
}

void
event::publish() const
{
    ::mud::core::message msg(_topic);
    ::mud::core::broker::publish(msg);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
