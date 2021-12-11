#ifndef _MUDLIB_EVENT_MECHANISM_H_
#define _MUDLIB_EVENT_MECHANISM_H_

#include <future>
#include <mud/core/factory.h>
#include <mud/core/handle.h>
#include <mud/core/task.h>
#include <mud/event/event.h>
#include <mud/event/ns.h>

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief An abstract base class representing an event-loop mechanism.
 */
class MUDLIB_EVENT_API event_mechanism
{
public:
    /**
     * @brief Constructor.
     */
    event_mechanism(const std::shared_ptr<mud::core::simple_task_queue>& queue)
      : _queue(queue)
    {}

    /**
     * @brief Virtual destructor.
     */
    virtual ~event_mechanism() = default;

    /**
     * @brief Register an event handler with the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to register.
     */
    virtual void register_handler(event&& event) = 0;

    /**
     * @brief Deregister an event handler from the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to deregister.
     */
    virtual void deregister_handler(event&& event) = 0;

    /**
     * Initiate the mechanism on it's own thread.
     *
     * @return The future associated when the mechanism has terminated.
     *
     * Detachable mechanisms will run on their own thread and return the
     * future while the thread continues to run in the background. A
     * non-detachable mechanism will block execution until the mechanism has
     * finished execution.
     */
    virtual std::shared_future<void> initiate() = 0;

    /**
     * @brief Request to terminate the mechanism.
     */
    virtual void terminate() = 0;

    /**
     * @brief Flag to indicate if the mechanism can be run detached in its
     * own thread.
     *
     * Any mechanism that is marked as detachable can be executed in a
     * separate thread when added to the @c event_loop.
     *
     * There can be at most one event-mechanism in an event-loop that
     * is marked as not detachable. The mechanism will be initiated and run on
     * the same thread as the thread that executes the @c event_loop @c loop
     * function.
     */
    virtual bool detachable() const { return true; }

    /**
     * Not copyable
     */
    event_mechanism(const event_mechanism&) = delete;
    event_mechanism& operator=(const event_mechanism&) = delete;

    /**
     * Not movable
     */
    event_mechanism(event_mechanism&&) = delete;
    event_mechanism& operator=(event_mechanism&&) = delete;

protected:
    /**
     * The queue for signaled events.
     */
    const std::shared_ptr<mud::core::simple_task_queue>& queue() const
    {
        return _queue;
    }

private:
    /** The queue for signaled events. */
    std::shared_ptr<mud::core::simple_task_queue> _queue;
};

/**
 * @brief The factory of event mechanisms.
 */
typedef mud::core::factory<mud::core::handle::type_t,
                           mud::event::event_mechanism,
                           const std::shared_ptr<mud::core::simple_task_queue>&>
    event_mechanism_factory;

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_EVENT_MECHANISM_H_ */
