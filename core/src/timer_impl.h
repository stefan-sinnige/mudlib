#ifndef _MUDLIB_CORE_TIMER_IMPL_H_
#define _MUDLIB_CORE_TIMER_IMPL_H_

#include "mud/core/timer.h"
#include "mud/core/event_loop.h"
#include <atomic>
#include <thread>

BEGIN_MUDLIB_CORE_NS

/**
 * Spin lock to safely protect the access and update of the timer expiration
 * time point. This is a generic implementation of a sping lock that may be
 * made publicly available in future if C++ is not supplying with one.
 *
 * Spin locks should be used with care to avoid dead locks and long busy waits.
 * The application of the spin-lock in this case is to hold the lock for the
 * most limited amount of time possible.
 */

class spin_lock
{
public: 
    /**
     * @brief Construct the spin-lock object.
     */
    spin_lock() = default;

    /**
     * @brief Acquire the exclusive lock.
     *
     * @details
     * Aqcuire the lock which may use busy spinning until this state has bee
     * achieved.
     */
    void lock() {
        while (_state.test_and_set(std::memory_order_acquire)) {
            // In future, C++20 has the ability to `wait` instead.
            std::this_thread::yield();
        }
    }

    /**
     * @brief Release the exclusive lock.
     */
    void unlock() {
        _state.clear(std::memory_order_release);
        // In future, C++20 can `notify_one`.
    }
private:
    /** The atomic flag that signals the lock state. */
    std::atomic_flag _state = ATOMIC_FLAG_INIT;
};

/**
 * @brief The implementation class of a @c mud::core::timer.
 */
class timer::impl : public mud::core::object
{
public:
    /**
     * Default constructor.
     */
    impl();

    /**
     * Destructor.
     */
    virtual ~impl() = default;

    /** The type of the timer. */
    timer::type_t type() const {
        return _type;
    }

    /** Start the periodic timer. */
    void start(const std::chrono::milliseconds& interval,
               const std::chrono::system_clock::time_point& epoch);

    /** Start the once-off timer. */
    void start(const std::chrono::system_clock::time_point& epoch);

    /** Start the once-off timer. */
    void start(const std::chrono::milliseconds& interval);

    /** Stop the timer. */
    void stop();

    /**
     * Return the next expiration trigger.
     */
    std::chrono::system_clock::time_point expiration() const;

    /**
     * @brief The impulse issued when a timer has expired.
     */
    timer::expire_impulse_type expire_impulse() {
        return _expire_impulse;
    }

    /**
     * Handler when the timer is triggered.
     */
    void on_expire(const std::chrono::system_clock::time_point& time_point);

private:
    /** The type of the timer */
    timer::type_t _type;

    /** The periodic interval */
    std::chrono::milliseconds _interval; 

    /** The epoch of the timer's first trigger. It shall never be updated. */
    std::chrono::system_clock::time_point _epoch;

    /** The time point for the next expiration trigger. */
    std::chrono::system_clock::time_point _expiration;

    /** The expire impulse.  */
    timer::expire_impulse_type _expire_impulse;

    /** The expiration timer locking mechanism. */
    spin_lock _lock;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_CORE_TIMER_IMPL_H_ */

