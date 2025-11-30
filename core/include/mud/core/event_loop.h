#ifndef _MUDLIB_CORE_EVENT_LOOP_H_
#define _MUDLIB_CORE_EVENT_LOOP_H_

#include <functional>
#include <memory>
#include <mud/core/handle.h>
#include <mud/core/event.h>
#include <mud/core/event_mechanism.h>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarations.
 */
class timer;
class timer_dispatcher;

/**
 * @brief A base class to define a blocking loop that reacts on @c event's.
 *
 * The event-loop waits for triggered events indefinitely or until told
 * to terminate. Only a single event handler can be assigned to a handle in
 * the event-loop at any time. If another event-handler is registered, any
 * previously registered event-handler will be deregistered.
 * The underlying implementation uses a kernel supported multiplexing technique
 * to be able to wait for multiple I/O handles at one.
 *
 * It is possible to access the event-loop in a multithreaded environment to
 * register and deregister event handlers. As soon as a change in registered
 * event handlers is detected, the event-loop will be enforced to take into
 * account the updated set of event handles to listen to.
 *
 * There is a single global event loop readily available.
 */
class MUDLIB_CORE_API event_loop
{
public:
    /**
     * @brief Default constructor.
     */
    event_loop();

    /**
     * @brief Move constructor.
     */
    event_loop(event_loop&&) = default;

    /**
     * Destructor.
     */
    virtual ~event_loop();

    /**
     * @brief Add a mechanism that processes events for a handle @c type.
     *
     * A mechanism is an instance of @c event_mechanism that defines the type
     * of handles it processes and the machinery of waiting for and dispatching
     * events.
     *
     * @param type [in]  The handle type associated to the mechanism.
     */
    void add_mechanism(mud::core::handle::type_t type);

    /**
     * @brief Register an event with the loop.
     *
     * If an event is already associated with the same @c handle, then it
     * will be replaced with the new event object.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to register.
     */
    virtual void register_handler(const event& event);

    /**
     * @brief Deregister an event from the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to deregister.
     */
    virtual void deregister_handler(const event& event);

    /**
     * @brief Run the loop, waiting for all registered @c handle and invoke the
     * associated @event_handler when needed.
     *
     * This method does not usually terminate unless it was explicitely
     * requested to do so through the @c terminate method. It will therefore
     * block the thread it is currently running on.
     *
     * Any mechanism that is marked as non-detachable will be run on the same
     * thread as this loop. These are typically UI threads.
     */
    virtual void loop();

    /*
     * @brief Request to terminate the run @c loop.
     * @return The future object returning the result of the @c loop. This
     * object can be used to wait upon the end of the running of the loop.
     *
     # @note This is a thread-safe operation.
     */
    virtual std::shared_future<void> terminate();

    /**
     * Non-copyable
     */
    event_loop(const event_loop&) = delete;
    event_loop& operator=(const event_loop&) = delete;

    /**
     * Return the global event-loop.
     */
    static event_loop& global();

    /**
     * @brief Return the timer dispatcher.
     *
     * @details
     * This is a private function that should not be used by a user. It is only
     * made public to make it accessible by the unit-test.
     */
    const std::shared_ptr<timer_dispatcher>& timers();

private:
    /**
     * Friends that require privileged access.
     */
    friend class mud::core::timer;

    /**
     * Platform specific implementation.
     */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_EVENT_LOOP_H_ */
