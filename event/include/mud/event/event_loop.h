#ifndef _MUDLIB_EVENT_EVENT_LOOP_H_
#define _MUDLIB_EVENT_EVENT_LOOP_H_

#include <functional>
#include <memory>
#include <mud/event/ns.h>
#include <mud/core/handle.h>
#include <mud/event/event.h>
#include <mud/event/event_mechanism.h>

BEGIN_MUDLIB_EVENT_NS

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
class MUDLIB_EVENT_API event_loop
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
     * @param[in] event  The event to regsietr.
     */
    virtual void register_handler(event&& event);

    /**
     * @brief Deregister an event from the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to deregsietr.
     */
    virtual void deregister_handler(event&& event);

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
     * Request to terminate the run @c loop.
     *
     * This is a thread-safe operation.
     */
    virtual void terminate();

    /**
     * Non-copyable
     */
    event_loop(const event_loop&) = delete;
    event_loop& operator=(const event_loop&) = delete;

    /**
     * Return the global event-loop.
     */
    static event_loop& global();

private:
    /**
     * Platform specific implementation.
     */
    class impl;
    struct impl_deleter {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_EVENT_LOOP_H_ */
