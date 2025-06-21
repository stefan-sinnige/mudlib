#include "mud/event/timer.h"
#include "mud/event/event_loop.h"
#include "timer_impl.h"
#include "timer_dispatcher.h"
#include <atomic>
#include <stdexcept>
#include <thread>
#include <ctime>
#include <iomanip>
#include <iostream>

std::ostream&
operator<<(std::ostream& ostr, const std::chrono::system_clock::time_point& tp)
{
    std::time_t tp_t = std::chrono::system_clock::to_time_t(tp);
    ostr << std::put_time(::localtime(&tp_t), "%F %T");
    return ostr;
}

BEGIN_MUDLIB_EVENT_NS

timer::impl::impl()
    : _type(timer::type_t::UNKNOWN)
    , _expiration(std::chrono::system_clock::time_point::max())
{
    _expire_impulse = std::make_shared<mud::core::impulse<void>>();
}

void
timer::impl::start(
        const std::chrono::milliseconds& interval,
        const std::chrono::system_clock::time_point& epoch)
{
    if (interval.count() == 0) {
        throw std::out_of_range("interval value cannot be zero");
    }
    _type = timer::type_t::PERIODIC;
    _interval = interval;
    _epoch = epoch;
    _lock.lock();
    _expiration = _epoch + _interval;
    _lock.unlock();
}

void
timer::impl::start(const std::chrono::system_clock::time_point& epoch)
{
    _type = timer::type_t::ONCE_OFF;
    _interval = std::chrono::milliseconds(0);
    _lock.lock();
    _expiration = _epoch = epoch;
    _lock.unlock();
}

void
timer::impl::start(const std::chrono::milliseconds& interval)
{
    _type = timer::type_t::ONCE_OFF;
    _interval = std::chrono::milliseconds(0);
    _lock.lock();
    _expiration = _epoch = std::chrono::system_clock::now() + interval;
    _lock.unlock();
}

void
timer::impl::stop()
{
    _type = timer::type_t::UNKNOWN;
    _expiration = std::chrono::system_clock::time_point::max();
}

void
timer::impl::on_expire(const std::chrono::system_clock::time_point& time_point)
{
    if (_type == timer::type_t::PERIODIC) {
        // Increase the expiration with the interval until the next point past
        // the time_point. This ensures that the timer is always based off the
        // original epoch:
        //    expiration = epoch + n * interval
        // Keep track how many times we need to increment which is an indication
        // that the timer's processing is delayed or the handling is taking
        // longer than expected.
        size_t cnt = 0;
        _lock.lock();
        while (_expiration < time_point) {
            _expiration += _interval;
        }
        _lock.unlock();
    }
    else
    if (_type == timer::type_t::ONCE_OFF) {
        // A once-off timer that has been triggered is no longer active.
        _type = timer::type_t::UNKNOWN;
    }
    _expire_impulse->pulse();
}

std::chrono::system_clock::time_point
timer::impl::expiration() const
{
    std::chrono::system_clock::time_point exp;
    const_cast<timer::impl*>(this)->_lock.lock();
    exp = _expiration;
    const_cast<timer::impl*>(this)->_lock.unlock();
    return exp;
}

/** The explicit implementation for POSIX event loops. */

timer::timer()
{
    _impl = std::make_shared<impl>();
}

timer::~timer()
{
    stop();
}

bool
timer::operator==(const mud::event::timer& other) const
{
    return _impl == other._impl;
}

bool
timer::operator!=(const mud::event::timer& other) const
{
    return _impl != other._impl;
}

timer::type_t
timer::type() const
{
    return _impl->type();
}

void
timer::start(
    const std::chrono::milliseconds& interval,
    const std::chrono::system_clock::time_point& epoch)
{
    LOG(log);
    INFO(log) << "Starting timer at " << epoch << std::endl;
    _impl->start(interval, epoch);
    mud::event::event_loop::global().timers()->insert(_impl);
}

void
timer::stop()
{
    LOG(log);
    INFO(log) << "Stopping timer" << std::endl;
    _impl->stop();
    mud::event::event_loop::global().timers()->remove(_impl);
}

void
timer::at(const std::chrono::milliseconds& interval)
{
    LOG(log);
    INFO(log) << "Starting timer at " << interval.count() << "ms" << std::endl;
    _impl->start(interval);
    mud::event::event_loop::global().timers()->insert(_impl);
}

void
timer::at(const std::chrono::system_clock::time_point& epoch)
{
    LOG(log);
    INFO(log) << "Starting timer at " << epoch << std::endl;
    _impl->start(epoch);
    mud::event::event_loop::global().timers()->insert(_impl);
}

std::chrono::system_clock::time_point
timer::expiration() const
{
    return _impl->expiration();
}

timer::expire_impulse_type
timer::expire_impulse() {
    return _impl->expire_impulse();
}

void
timer::on_expire(const std::chrono::system_clock::time_point& time_point)
{
    LOG(log);
    INFO(log) << "Timer expired, pulsing" << std::endl;
    _impl->on_expire(time_point);
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
