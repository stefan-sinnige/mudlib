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

#include "timer_dispatcher.h"
#include "posix/select_mechanism.h"
#include <algorithm>

BEGIN_MUDLIB_CORE_NS

timer_dispatcher::timer_dispatcher()
    : _changed(mud::core::uuid(), _handle.handle(),
               mud::core::event::signal_type::READING)
{
    /* Attach the handler when the timer list has changed (ie a timer has been
     * added or removed. The handler will only reset the trigger handle as
     * that is all that is required for the @c select to re-examine the timer
     * list to calculate the updated timeout. */
    ::mud::core::broker::attach(_changed.topic(),
        [&](const mud::core::message&) {
            _handle.capture();
        });
}

void
timer_dispatcher::insert(const std::shared_ptr<mud::core::timer::impl>& timer)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _timers.push_back(timer);
    sort();
    _handle.trigger();
}

void
timer_dispatcher::remove(const std::shared_ptr<mud::core::timer::impl>& timer)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto pos = std::find(_timers.begin(), _timers.end(), timer);
    if (pos != _timers.end()) {
        _timers.erase(pos);
    }
    sort();
    _handle.trigger();
}

std::chrono::milliseconds
timer_dispatcher::wait_duration(
        const std::chrono::system_clock::time_point& epoch)
{
    // If there are no timers, wait until the maximum possible.
    if (_timers.size() == 0) {
        return std::chrono::milliseconds::max();
    }

    // Calculate the wait time against the first timer entry, assuming that
    // all entries are sorted by expiration time point.
    std::lock_guard<std::mutex> lock(_mutex);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        (*_timers.begin())->expiration()  - epoch);
    if (duration.count() < 0) {
        return std::chrono::milliseconds(0);
    }
    return duration;
}

void
timer_dispatcher::dispatch(const std::chrono::system_clock::time_point& epoch)
{
    // Trigger all the timers that have expired and stop as soon as we hit a
    // timer that has not yet expired.
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& timer: _timers) {
        if (timer->expiration() <= epoch) {
            timer->on_expired(epoch);
        }
        else {
            break;
        }
    }
    sort();
}

mud::core::event&
timer_dispatcher::changed()
{
    return _changed;
}

void
timer_dispatcher::sort()
{
    // Remove any timers that are inactive (erase-remove idiom).
    for (auto iter = _timers.begin(); iter != _timers.end(); /* inside loop */)
    {
        if ((*iter)->type() == mud::core::timer::type_t::UNKNOWN) {
            iter = _timers.erase(iter);
        }
        else {
            ++iter;
        }
    }

    // Sort all timers by earliest expiration
    std::sort(_timers.begin(), _timers.end(),
        [](const auto& t1, const auto& t2) {
            return t1->expiration() < t2->expiration();
    });
}

void
timer_dispatcher::on_expired(const mud::core::message&)
{
    auto now = std::chrono::system_clock::now();
    dispatch(now);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
