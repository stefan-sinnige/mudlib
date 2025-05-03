#ifndef _MUDLIB_EVENT_TIMER_H_
#define _MUDLIB_EVENT_TIMER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <mud/core/object.h>
#include <mud/core/impulse.h>
#include <mud/event/ns.h>

BEGIN_MUDLIB_EVENT_NS

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
class MUDLIB_EVENT_API timer
{
public:
    /**
     * @brief The type of the @c impulse when a timer has expired.
     */
    typedef std::shared_ptr<mud::core::impulse<void>> expire_impulse_type;

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
     * @brief The impulse issued when a timer has expired.
     */
    expire_impulse_type expire_impulse();

private:
    /**
     * Friend classes.
     */
    friend class timer_dispatcher;

    /**
     * @brief Event handler when a timer has expired.
     *
     * @details
     * The handler that is issued when the timer has expired. This shall
     * invoke any registered impulses and update the expiration timer to the
     * next interval time point if the timer is an active periodic timer.
     *
     * @param time_point The time-point that the trigger is invoked. Any
     * calculation is expected to be based off this reference.
     */
    void on_expire(const std::chrono::system_clock::time_point& time_point);

    /**
     * Platform specific implementation.
     */
    class impl;
    std::shared_ptr<impl> _impl;
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_TIMER_H_ */
