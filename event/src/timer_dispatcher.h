#ifndef _MUDLIB_EVENT_TIMER_DISPATCHER_H_
#define _MUDLIB_EVENT_TIMER_DISPATCHER_H_

#include "mud/event/event.h"
#include "timer_impl.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

BEGIN_MUDLIB_EVENT_NS

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
class timer_dispatcher
{
public:
    /**
     * @brief Creation of a timer dispatcher.
     */
    timer_dispatcher() = default;

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
    void insert(const std::shared_ptr<mud::event::timer::impl>& timer);

    /**
     * @brief Remove a timer.
     *
     * @details
     * The timer has stopped and is removed from the event-loop. The event-loop
     * is taking into account the removal of this timer's expiration time.
     *
     * @param timer The @c timer to remove.
     */
    void remove(const std::shared_ptr<mud::event::timer::impl>& timer);

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
     * @brief Return the event to signal when the timer list has changed.
     */
    mud::event::event event();

private:
    /**
     * @brief Sort the timers on first activation
     */
    void sort();

    /** The list of active timers.  */
    std::vector<std::shared_ptr<mud::event::timer::impl>> _timers;

    /** Protecting the list against simultaneous modifications.  */
    std::mutex _mutex;

    /** The handle to signal of any change in the timer list. */
    mud::core::select_handle::signal _handle;
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_TIMER_DISPATCHER_H_ */
