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

#ifndef _MUDLIB_CORE_TIMER_DISPATCHER_H_
#define _MUDLIB_CORE_TIMER_DISPATCHER_H_

#include "mud/core/event.h"
#include "mud/core/object.h"
#include "timer_impl.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The timer dispatcher.
 *
 * @details
 * The @c timer-dispatcher is a private class (ie. not exposed as a public
 * interface) that maintains the list of all active timers. The dispatcher is an
 * integeral member of an @c event_loop and should only be used in that context.
 *
 * Only active timers are part of the dispatcher. That means, only when a @c
 * timer is started, it shall be made part of the dispatcher. Any timer that is
 * stopped shall be removed from the dispatcher.
 */
class timer_dispatcher: public mud::core::object
{
public:
    /**
     * @brief Creation of a timer dispatcher.
     */
    timer_dispatcher();

    /**
     * @brief Destructor.
     */
    virtual ~timer_dispatcher() = default;

    /**
     * @brief Insert a timer.
     *
     * @details
     * The timer has started and is added to the event-loop. The event-loop is
     * taking into account the timer's expiration time.
     *
     * @param timer The @c timer to add.
     */
    void insert(const std::shared_ptr<mud::core::timer::impl>& timer);

    /**
     * @brief Remove a timer.
     *
     * @details
     * The timer has stopped and is removed from the event-loop. The event-loop
     * is taking into account the removal of this timer's expiration time.
     *
     * @param timer The @c timer to remove.
     */
    void remove(const std::shared_ptr<mud::core::timer::impl>& timer);

    /**
     * @brief Return the wait duration.
     *
     * @details
     * The wait duration is the number of milliseconds to wait from the @c epoch
     * until the next timer. This is always a value that is equal to or bigger
     * than zero. If no wait is available, the maximum number is returned.
     *
     * @param epoch The reference time-point to wait from.
     */
    std::chrono::milliseconds wait_duration(
            const std::chrono::system_clock::time_point& epoch);

    /**
     * @brief Dispatch the timer triggers.
     *
     * @details
     * Dispatch the triggers to all the timers that have been expired up to
     * the @c epoch time point.
     *
     * @param epoch The time point to use as a base reference for the trigger.
     */
    void dispatch(const std::chrono::system_clock::time_point& epoch);

    /**
     * @brief Return the event to signal when the timer list has changed.
     */
    mud::core::event& changed();

    /**
     * @brief Handler when one of the timers has expired.
     * @details
     * This handler is usually associated to the event-loop implementation. When
     * invoked, it shall 
     */
    void on_expired(const message&);

private:
    /**
     * @brief Sort the timers on first activation
     */
    void sort();

    /** The list of active timers.  */
    std::vector<std::shared_ptr<mud::core::timer::impl>> _timers;

    /** Protecting the list against simultaneous modifications.  */
    std::mutex _mutex;

    /** The handle to signal of any change in the timer list. */
    mud::core::select_handle::signal _handle;

    /** The event associated to the handle when the timer list has changed. */
    mud::core::event _changed;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_TIMER_DISPATCHER_H_ */
