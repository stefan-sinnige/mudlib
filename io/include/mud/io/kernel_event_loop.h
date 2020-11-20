#ifndef _MUDLIB_IO_KERNEL_EVENT_LOOP_H_
#define _MUDLIB_IO_KERNEL_EVENT_LOOP_H_

#include <functional>
#include <memory>
#include <mud/io/ns.h>
#include <mud/io/kernel_handle.h>

BEGIN_MUDLIB_IO_NS

/**
 * @brief A base class to define a blocking loop that reacts on I/O events.
 *
 * The event-loop waits for kernel triggered events indefinitely or until told
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
 */
class kernel_event_loop
{
public:
    /**
     * Definition for an event-handling routine.
     */
    typedef std::function<void(void)> event_handler;

    /**
     * @brief Default constructor.
     */
    kernel_event_loop();

    /**
     * @brief Move constructor.
     */
    kernel_event_loop(kernel_event_loop&&) = default;

    /**
     * Destructor.
     */
    virtual ~kernel_event_loop();

    /**
     * Register an event handler with the loop. The @c event_handler is
     * associated to a @c handle. Any event that occurs on that handle will
     * invoke the event-handler.
     *
     * If an event handler is already associated with that @c handle, then it
     * will be replaced with the new handler.
     *
     * This is a thread-safe operation.
     *
     * @param[in] handle   The handle to wait for events.
     * @param[in] handler  The event handler to register.
     */
    virtual void register_handler(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            event_handler handler);

    /**
     * Deregister an event handler from the loop. Any event that occurs on
     * the @c handle associated to the event-handler will no longer invoke
     * the event handler.
     *
     * This is a thread-safe operation.
     *
     * @param[in] handle   The handle and its associated handler to remove.
     */
    virtual void deregister_handler(
            const std::unique_ptr<mud::io::kernel_handle>& handle);

    /**
     * Run the loop, waiting for all registered @c handle and invoke the
     * associated @event_handler when needed.
     *
     * This method does not usually terminate unless it was explicitely
     * requested to do so through the @c terminate method. It will therefore
     * block the thread it is currently running on.
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
    kernel_event_loop(const kernel_event_loop&) = delete;
    kernel_event_loop& operator=(const kernel_event_loop&) = delete;

private:
    /**
     * Platform specific implementation.
     */
    class impl;
    std::unique_ptr<impl> _impl;
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_KERNEL_EVENT_LOOP_H_ */
