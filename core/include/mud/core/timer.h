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

#ifndef _MUDLIB_CORE_TIMER_H_
#define _MUDLIB_CORE_TIMER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <mud/core/object.h>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarations.
 */
class timer_dispatcher;

/**
 * @brief Trigger periodic or one-off timed events.
 *
 * @details
 * The @timer object is able to trigger repetitive periodic timed trigger or
 * a one-off trigger. A repetitive trigger is signalled at regular intervals
 * until it is told to stop. A one-shot trigger is only signalled once.
 *
 * Periodic timers can be setup a relative duration from the point the trigger
 * is started. A once-off timer can be setup either through a relative duration
 * or at an absolute clock time. Timers are implemented through the use of an
 * @c event_loop and when a timer is triggered at a specific time-point, other
 * timers that are expired at the point of processing shall be triggered as
 * well.
 *
 * The triggering of all the timers is single-threaded per time-point. It may be
 * possible that multiple triggers are running simultaneously in multiple
 * threads, each invoked for their own time-point. However, timers are not
 * invoked while another invocation is already in process to prevent queueing
 * and potential flooding of outstanding timer events when the processing time
 * exceeds a timer interval.
 *
 * The timer resolution is based on milli-second accuracy, however the @c timer
 * should not be used for precise and accurate timers. Additional delays are
 * likely to occur based on the load of the system and application and the
 * handling of the timers in general. If more accurate timers are required,
 * resort to another means, like OS (kernel) based timers. Future enhancements
 * may add support for accurate timer.
 */
class MUDLIB_CORE_API timer
{
public:
    /**
     * @brief Message when timers have expired.
     * @details
     * This message is a specialisation of @c mud::core::message and is the
     * message type that is passed on the @c expired notification.
     */
    class expired_message: public mud::core::message
    {
    public:
        /**
         * @brief Create a message for an expired timer.
         * @param topic The expired topic.
         * @param time_poiint The current time point.
         */
        expired_message(const mud::core::uuid& topic,
                        const std::chrono::system_clock::time_point& time_point)
            : mud::core::message(topic)
            , _time_point(time_point)
        {}

        /**
         * @brief Return the expiry time point.
         */
        const std::chrono::system_clock::time_point& time_point() const {
            return _time_point;
        }

    private:
        /** The time point */
        std::chrono::system_clock::time_point _time_point;
    };

    /**
     * @brief The type of trigger.
     */
    enum class type_t
    {
        UNKNOWN = 0,  /**< An unknown timer (never been started). */
        PERIODIC = 1, /**< A periodic timer. */
        ONCE_OFF = 2  /**< A one-off timer. */
    };

    /**
     * @brief Creation of a timer.
     */
    timer();

    /**
     * @brief Destructor.
     */
    virtual ~timer();

    /**
     * @brief Return the type of the timer.
     *
     * @details
     * The type is only known when a timer had been started. When a timer is
     * no longer active, it's type shall be @c UNKNOWN.
     */
    type_t type() const;

    /**
     * @brief Not copyable.
     */
    timer(const timer&) = delete;

    /**
     * @brief Not copy-assignable.
     */
    timer& operator=(const timer&) = delete;

    /**
     * @brief Moving a timer will move its underlying resource.
     *
     * @param other The timer to move.
     */
    timer(timer&& other) = default;

    /**
     * @brief Move-assigning a timer will move its underlying resource.
     *
     * @param other The timer to move.
     */
    timer& operator=(timer&& other) = default;

    /**
     * @brief Equality operator.
     *
     * @param other The timer to compare against.
     *
     * @Retuns @c true if the @c other timer is the same.
     */
    bool operator==(const timer& other) const;

    /**
     * @brief In-equality operator.
     *
     * @param other The timer to compare against.
     *
     * @Retuns @c true if the @c other timer is not the same.
     */
    bool operator!=(const timer& other) const;

    /*
     * @brief Start the timer as a periodic timer.
     *
     * @param interval The interval at which a trigger will be signalled.
     * @param epoch The reference time-point to apply the @c interval to.
     */
    void start(const std::chrono::milliseconds& interval,
               const std::chrono::system_clock::time_point& epoch =
                   std::chrono::system_clock::now());

    /**
     * @brief Start the timer as a once-off timer after a wait interval..
     *
     * @param interval The duration interval to wait for.
     */
    void at(const std::chrono::milliseconds& interval);

    /**
     * @brief Start the timer as a once-off timer at a particular time point.
     *
     * @param epoch The time point at which a signal shall be triggered.
     */
    void at(const std::chrono::system_clock::time_point& epoch);

    /**
     * @brief Stop the timer.
     *
     * @details
     * For a periodid timer, it stops timer from being signalled. If there is
     * any timer already queued for signalling, it shall be signalled.
     *
     * If the timer is a one-shot timer that has not been signalled, it shall
     * not be signalled.
     */
    void stop();

    /**
     * @brief The next expiration time.
     *
     * @details
     * The expiration time is always the fixed time-point for a once-off timer,
     * and a calculated time-point of the next periodic interval. The time point
     * may be a time in the past if the timer is a once-off timer or if it has 
     * not yet been triggered.
     *
     * For a timer that is not active (ie. stopped), the expiration is equal to
     * the value of @c std::chrono::time_point::max().
     */
    std::chrono::system_clock::time_point expiration() const;

    /**
     * @brief The notification topic to publish an @c expired_message to.
     */
    const mud::core::uuid& expired() const;

private:
    /**
     * Friend classes.
     */
    friend class timer_dispatcher;

    /**
     * @brief Notification from the dispatcher when a timer has expired.
     *
     * @details
     * The handler that is issued when the timer has expired in the event-loop
     * through the @c timer_dispatcher. This shall in turn invoke the timer
     * expired notifications using the @c expired notification topic. The
     * expiration timer is updated to the next interval time point if the timer
     * is an active periodic timer.
     *
     * @param msg Instance of a @c expired_message that includes the trigger
     * time-point. Any calculation is expected to be based off this reference
     * time-point.
     */
    void on_expired(const mud::core::message& msg);

    /**
     * Platform specific implementation.
     */
    class impl;
    std::shared_ptr<impl> _impl;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_TIMER_H_ */
