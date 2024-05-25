#ifndef _MUDLIB_POSIX_SELECT_MECHANISM_H_
#define _MUDLIB_POSIX_SELECT_MECHANISM_H_

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mud/core/handle.h>
#include <mud/event/event_mechanism.h>
#include <mud/event/ns.h>
#include <thread>
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/select.h>
#endif

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief The event-loop mechanism using the POSIX @c select call.
 *
 * The I/O event-loop for POSIX systems uses the @c select to perform the
 * multiplexing of I/O @c handle's. A self-event trick (pipe/UDP socket) is
 * used for self-signalling events.
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
     * Destructor.
     */
    virtual ~select_mechanism();

    /**
     * Register an event handler with the loop.
     *
     * @param[in] event  The event to register.
     */
    virtual void register_handler(event&& event) override;

    /**
     * Deregister an event handler from the loop.
     *
     * @param[in] event  The event to deregister.
     */
    virtual void deregister_handler(event&& event) override;

    /**
     * Initiate the mechanism.
     * @returns The @c future that signals the mechanism has terminated.
     */
    virtual std::shared_future<void> initiate() override;

    /*
     * Terminate the mechanism.
     */
    virtual void terminate() override;

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
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_POSIX_SELECT_MECHANISM_H_ */
