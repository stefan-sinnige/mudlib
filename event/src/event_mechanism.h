#ifndef _MUDLIB_EVENT_MECHANISM_H_
#define _MUDLIB_EVENT_MECHANISM_H_

#include "mud/event/ns.h"
#include "mud/core/handle.h"
#include "mud/core/task.h"
#include "mud/event/event.h"
#include <future>

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief An abstract base class representing an event-loop mechanism.
 */
class event_mechanism
{
public:
    /**
     * Virtual destructor.
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
     * @brief  Deregister an event handler from the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to deregister.
     */
    virtual void deregister_handler(event&& event) = 0;

    /**
     * Initiate the mechanism.
     * @return The future associated to the underlying thread has terminated.
     */
    virtual std::shared_future<void> initiate() = 0;

    /*
     * Request to terminate the mechanism.
     */
    virtual void terminate() = 0;
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_EVENT_MECHANISM_H_ */
