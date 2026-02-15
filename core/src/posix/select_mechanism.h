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

#ifndef _MUDLIB_POSIX_SELECT_MECHANISM_H_
#define _MUDLIB_POSIX_SELECT_MECHANISM_H_

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mud/core/handle.h>
#include <mud/core/event_mechanism.h>
#include <mud/core/object.h>
#include <mud/core/ns.h>
#include <thread>
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/select.h>
#endif

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The event-loop mechanism using the POSIX @c select call.
 *
 * The I/O event-loop for POSIX systems uses the @c select to perform the
 * multiplexing of I/O @c handle's. A self-event trick (pipe/UDP socket) is
 * used for self-signalling events.
 */
class select_mechanism : public event_mechanism, public mud::core::object
{
public:
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
     * @param queue The queue to hold signaled events.
     * @param timers The timer dispatcher to hold event timers.
     */
    select_mechanism(
        const std::shared_ptr<mud::core::task_queue<void(void)>>& queue,
        const std::shared_ptr<mud::core::timer_dispatcher>& timers);

    /**
     * Destructor.
     */
    virtual ~select_mechanism();

    /**
     * Add an event handler with the loop.
     *
     * @param[in] event  The event to add.
     */
    virtual void add(event&& event) override;

    /**
     * Remove an event handler from the loop.
     *
     * @param[in] event  The event to remove.
     */
    virtual void remove(const event& event) override;

    /**
     * Initiate the mechanism.
     * @returns The @c future that signals the mechanism has terminated.
     */
    virtual std::shared_future<void> initiate() override;

    /*
     * Terminate the mechanism.
     */
    virtual void terminate() override;

    /*
     * @brief The notification to publish when the event-loop has timed out.
     */
    const mud::core::uuid& timed_out() const {
        return _timed_out;
    }

private:
    /**
     * Run the loop, waiting for all registered @c event obkects and invoke the
     * associated @event::handler when needed.
     */
    void loop();

    /*
     * Request to a no-operation. This will effectively stop the @c select
     * and re-multiplex.
     */
    void nop();

    /**
     * Multiplex the registered events into an @fd_set
     */
    void multiplex(fd_set& readfds,   /**< [out] The set of file descriptors to
                                         wait   for a read. */
                   fd_set& writefds,  /**< [out] The set of file descriptors to
                                         wait  for a write. */
                   fd_set& exceptfds, /**< [out] The set of file descriptors to
                                         wait for an exceptional condition. */
                   int& maxfd /**< [out] The maximum file descriptor handle
                                         number */
    );

    /**
     * Demultiplex the @fd_set and dispatch the events handlers on those that
     * fired.
     */
    void demultiplex(
        const fd_set& readfds,  /**< [out] The set of file descriptors to
                                     wait for a read. */
        const fd_set& writefds, /**< [out] The set of file descriptors to
                                     wait for a write. */
        const fd_set& exceptfds /**< [out] The set of file descriptors to
                                     wait for an exceptional condition. */
    );

    /**
     * Remove any event with a bad file descriptor.
     */
    void remove_badf();

    /**
     * Handle commands send to the event-loop from the self-event.
     */
    void command_handler();

    /** Self-signalling resource */
    mud::core::select_handle::signal _self;

    /* Event for self-signalling resource */
    mud::core::event _self_event;

    /** Registered events. */
    std::list<event> _events;

    /** Guard for exclusive access to _events list. */
    std::mutex _lock;

    /** Flag to indicate if the loop is running. */
    std::atomic<bool> _running;

    /** The thread running the loop */
    std::thread _thread;

    /** The promise tied to the thread */
    std::promise<void> _promise;

    /** The future thread object. */
    std::shared_future<void> _future;

    /** The timed out notification topic */
    mud::core::uuid _timed_out;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_POSIX_SELECT_MECHANISM_H_ */
