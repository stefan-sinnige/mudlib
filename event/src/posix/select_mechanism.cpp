#include "posix/select_mechanism.h"
#include "posix/select_self.h"
#include <errno.h>
#include <algorithm>
#include <thread>
#include <system_error>
#include <list>
#if defined(WINDOWS) && defined(NATIVE)
    #include <windows.h>
#else
    #include <sys/select.h>
#endif

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief An implementation class of an @c mud::event::event_loop for POSIX
 * systems using the @c select mechanism.
 *
 * The I/O event-loop for POSIX systems uses the @c select to perform the
 * multiplexing of I/O @c handle's. A self-event trick (pipe/UDP socket) is
 * used for self-signalling events.
 */
class select_mechanism::impl
{
public:
    /**
     * Default constructor.
     */
    impl(const std::shared_ptr<mud::core::simple_task_queue>& queue);

    /**
     * Destructor.
     */
    virtual ~impl();

    /**
     * Register an event handler with the loop.
     */
    void register_handler(event&& event);

    /**
     * Deregister an event handler associated to a handle from the loop.
     */
    void deregister_handler(event&& event);

    /**
     * Initiate the mechanism.
     */
    std::shared_future<void> initiate();

    /*
     * Request to terminate the run @c loop.
     */
    void terminate();

    /**
     * Non-copyable
     */
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

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
    void multiplex(
            fd_set& readfds,   /**< [out] The set of file descriptors to wait
                                          for a read. */
            fd_set& writefds,  /**< [out] The set of file descriptors to wait
                                          for a write. */
            fd_set& exceptfds, /**< [out] The set of file descriptors to wait
                                          for an exceptional condition. */
            int& maxfd         /**< [out] The maximum file descriptor handle
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
     * Handle commands send to the event-loop from the self-event.
     */
    void command_handler();

    /** The queue for signaled events */
    std::shared_ptr<mud::core::simple_task_queue> _queue;

    /** Self-signalling resource */
    select_self* _self;

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

select_mechanism::impl::impl(
        const std::shared_ptr<mud::core::simple_task_queue>& queue)
    : _queue(queue), _running(false)
{
    /* Always register the self-event to receive the command */
    _self = new select_self();
    int handle = mud::core::internal_handle<int>(_self->handle());
    event ev(_self->handle(), event::signal_type::READING, [&]() {
        _self->receive();
        return mud::event::event::return_type::CONTINUE;
    });
    _events.push_back(ev);
}

select_mechanism::impl::~impl()
{
    /* Make sure the loop is terminated */
    terminate();

    /* If the mechanism had run, the future would be valid. Ensure that
     * the thread is terminated (joined). */
    if (_future.valid())
    {
        _future.wait();
        _thread.join();
    }

    /* Remove the self event */
    delete _self;
}

void
select_mechanism::impl::register_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end())
    {
        _events.erase(found);
    }
    _events.push_back(event);
    nop();
}

void
select_mechanism::impl::deregister_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end())
    {
        _events.erase(found);
        nop();
    }
}

std::shared_future<void>
select_mechanism::impl::initiate()
{
    if (_running.exchange(true) == false)
    {
        if (_thread.joinable())
        {
            _thread.join();
        }
        _promise = std::promise<void>();
        _thread = std::thread(&select_mechanism::impl::loop, this);
        _future = _promise.get_future();
    }
    return _future;
}

void
select_mechanism::impl::loop()
{
    /* Loop until we're told to stop. This loop is run on it's own thread. */
    while (_running.load() == true)
    {
        fd_set readfds, writefds, exceptfds;
        int maxfd;

        multiplex(readfds, writefds, exceptfds, maxfd);
        if (::select(maxfd+1, &readfds, &writefds, &exceptfds, nullptr) == -1)
        {
            throw std::system_error(errno, std::system_category(), "select");
        }
        demultiplex(readfds, writefds, exceptfds);
    }

    /* Reset the event's command state such that we can re-use the loop
     * again. */
    nop();


    /* Signal the end of the thread. */
    _promise.set_value();
}

void
select_mechanism::impl::terminate()
{
    if (_running.load() == true)
    {
        _running.store(false);
        _self->send();
    }
}

void
select_mechanism::impl::nop()
{
    _self->send();
}

void
select_mechanism::impl::multiplex(
        fd_set& readfds,
        fd_set& writefds,
        fd_set& exceptfds,
        int& maxfd)
{
    std::lock_guard<std::mutex> lock(_lock);
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    maxfd = -1;

    /* Add all registered event handlers */
    for (auto& event: _events)
    {
        if (event.handle() != nullptr &&
                (event.mask() & event::signal_type::READING)
                == event::signal_type::READING)
        {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &readfds);
            if (handle > maxfd) {
                maxfd = handle;
            }
        }
        if (event.handle() != nullptr &&
                (event.mask() & event::signal_type::WRITING)
                == event::signal_type::WRITING)
        {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &writefds);
            if (handle > maxfd) {
                maxfd = handle;
            }
        }
    }
}

void
select_mechanism::impl::demultiplex(
        const fd_set& readfds,
        const fd_set& writefds,
        const fd_set& exceptfds)
{
    std::lock_guard<std::mutex> lock(_lock);

    /* Check all excepts */

    /* Check all reads */
    auto event_it = _events.begin();
    while (event_it != _events.end())
    {
        if (event_it->handle() != nullptr &&
                (event_it->mask() & event::signal_type::READING)
                == event::signal_type::READING)
        {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &readfds))
            {
                if (event_it->handle() == _self->handle())
                {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    (void) (event_it->handler())();
                    ++event_it;
                }
                else
                {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    auto handler = event_it->handler();
                    event copy = *event_it;
                    mud::core::simple_task task([handler, copy, this]() {
                        if (handler() == event::return_type::CONTINUE)
                        {
                            event ev = copy;
                            this->register_handler(std::move(ev));
                        }
                    });
                    _queue->push(std::move(task));
                    event_it = _events.erase(event_it);
                }
            }
            else
            {
                ++event_it;
            }
        }
        else
        {
            ++event_it;
        }
    }

    /* Check all writes */
    event_it = _events.begin();
    while (event_it != _events.end())
    {
        if (event_it->handle() != nullptr &&
                (event_it->mask() & event::signal_type::WRITING)
                == event::signal_type::WRITING)
        {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &writefds))
            {
                if (event_it->handle() == _self->handle())
                {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    (void) (event_it->handler())();
                    ++event_it;
                }
                else
                {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    auto handler = event_it->handler();
                    event copy = *event_it;
                    mud::core::simple_task task([handler, copy, this]() {
                        if (handler() == event::return_type::CONTINUE)
                        {
                            event ev = copy;
                            this->register_handler(std::move(ev));
                        }
                    });
                    _queue->push(std::move(task));
                    event_it = _events.erase(event_it);
                }
            }
            else
            {
                ++event_it;
            }
        }
        else
        {
            ++event_it;
        }
    }
}

void
select_mechanism::impl_deleter::operator()(select_mechanism::impl* ptr) const
{
    delete ptr;
}

/** The explicit implementation for POSIX event loops. */

select_mechanism::select_mechanism(
        const std::shared_ptr<mud::core::simple_task_queue>& queue)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(queue));
}

select_mechanism::~select_mechanism()
{
}

void
select_mechanism::register_handler(event&& event)
{
    _impl->register_handler(std::move(event));
}

void
select_mechanism::deregister_handler(event&& event)
{
    _impl->deregister_handler(std::move(event));
}

std::shared_future<void>
select_mechanism::initiate()
{
    return _impl->initiate();
}

void
select_mechanism::terminate()
{
    _impl->terminate();
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

