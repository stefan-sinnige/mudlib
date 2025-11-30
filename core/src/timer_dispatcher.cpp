#include "timer_dispatcher.h"
#include "posix/select_mechanism.h"
#include <algorithm>

BEGIN_MUDLIB_CORE_NS

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

void
timer_dispatcher::dispatch(const std::chrono::system_clock::time_point& epoch)
{
    // Trigger all the timers that have expired and stop as soon as we hit a
    // timer that has not yet expired.
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& timer: _timers) {
        if (timer->expiration() <= epoch) {
            timer->on_expire(epoch);
        }
        else {
            break;
        }
    }
    sort();
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

mud::core::event
timer_dispatcher::event()
{
    return mud::core::event(
        _handle.handle(), mud::core::event::signal_type::READING,
        [&]() {
            // The event was raised as part of a change to the list of timers,
            // in order to force the event-loop to re-evaluate itself and to
            // take into account any possible timer change.
            _handle.capture();
            return mud::core::event::return_type::CONTINUE;
        });
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
