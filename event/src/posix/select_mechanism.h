#ifndef _MUDLIB_SELECT_EVENT_LOOP_H_
#define _MUDLIB_SELECT_EVENT_LOOP_H_

#include <functional>
#include <future>
#include <memory>
#include <mud/event/ns.h>
#include <mud/core/handle.h>
#include "event_mechanism.h"

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief The event-loop mechanism using the POSIX @c select call.
 */
class select_mechanism : public event_mechanism
{
public:
    /**
     * Definition for an event-handling routine.
     */
    typedef std::function<void(void)> event_handler;

    /**
     * The types of readiness to examine.
     */
    enum class readiness_t
    {
        READING,
        WRITING
    };

    /**
     * @brief Default constructor.
     * @param [in] The queue to hold signaled events.
     */
    select_mechanism(
            const std::shared_ptr<mud::core::simple_task_queue>& queue);

    /**
     * @brief Move constructor.
     */
    select_mechanism(select_mechanism&&) = default;

    /**
     * Destructor.
     */
    virtual ~select_mechanism();

    /**
     * Register an event handler with the loop.
     *
     * @param[in] event  The event to register.
     */
    virtual void register_handler(event&& event);

    /**
     * Deregister an event handler from the loop.
     *
     * @param[in] event  The event to deregister.
     */
    virtual void deregister_handler(event&& event);

    /**
     * Initiate the mechanism.
     * @returns The @c future that signals the mechanism has terminated.
     */
    virtual std::shared_future<void> initiate();

    /*
     * Terminate the mechanism.
     */
    virtual void terminate();

    /**
     * Non-copyable
     */
    select_mechanism(const select_mechanism&) = delete;
    select_mechanism& operator=(const select_mechanism&) = delete;

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

#endif /* _MUDLIB_SELECT_EVENT_LOOP_H_ */
