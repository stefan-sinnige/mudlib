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

#ifndef _MUDLIB_CORE_MECHANISM_H_
#define _MUDLIB_CORE_MECHANISM_H_

#include <future>
#include <mud/core/factory.h>
#include <mud/core/handle.h>
#include <mud/core/task.h>
#include <mud/core/event.h>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarations.
 */
class timer_dispatcher;

/**
 * @brief An abstract base class representing an event-loop mechanism.
 */
class MUDLIB_CORE_API event_mechanism
{
public:
    /**
     * The type of a task that executes an event.
     */
    typedef std::packaged_task<void(void)> task_type;

    /**
     * @brief Constructor.
     *
     * @details
     * Construct an event loop mechanism that implements the event-loop using
     * a specific technology. The task queue and the timer dispatcher are
     * always an integral part of any mechanism.
     *
     * @parm queue The task queue to use.
     * @parm timers The timer dispatcher to use.
     */
    event_mechanism(
            const std::shared_ptr<mud::core::task_queue<void(void)>>& queue,
            const std::shared_ptr<mud::core::timer_dispatcher>& timers);

    /**
     * @brief Virtual destructor.
     */
    virtual ~event_mechanism() = default;

    /**
     * @brief Add an event handler with the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to add.
     */
    virtual void add(event&& event) = 0;

    /**
     * @brief Remove an event handler from the loop.
     *
     * This is a thread-safe operation.
     *
     * @param[in] event  The event to remove.
     */
    virtual void remove(const event& event) = 0;

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
    const std::shared_ptr<mud::core::task_queue<void(void)>>& queue() const
    {
        return _queue;
    }

    /**
     * The timer dispatcher for timer events.
     */
    const std::shared_ptr<mud::core::timer_dispatcher>& timers() const
    {
        return _timers;
    }

private:
    /** The queue for signaled events. */
    std::shared_ptr<mud::core::task_queue<void(void)>> _queue;

    /** The timer dispatcher for timer. */
    std::shared_ptr<mud::core::timer_dispatcher> _timers;
};

/**
 * @brief The factory of event mechanisms.
 */
typedef mud::core::factory<mud::core::handle::type_t,
                           mud::core::event_mechanism,
                           const std::shared_ptr<mud::core::task_queue<void(void)>>&,
                           const std::shared_ptr<mud::core::timer_dispatcher>&>
    event_mechanism_factory;

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_CORE_MECHANISM_H_ */
