#include "mud/event/event_loop.h"
#include "event_mechanism.h"
#include "posix/select_mechanism.h"
#include <errno.h>
#include <algorithm>
#include <system_error>
#include <map>
#include <memory>
#include <vector>

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief The implementation class of an @c mud::event::event_loop, managing
 * different underlying mechanisms, based on the type of handle being used.
 */
class event_loop::impl
{
public:
    /**
     * Default constructor.
     */
    impl();

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
     * Run the loop, waiting for all registered @c handle and invoke the
     * associated @event_handler when needed.
     */
    void loop();

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
    /** The task queue to hold signaled event tasks. */
    std::shared_ptr<mud::core::simple_task_queue> _queue;

    /** The task worker pool to handle signaled event tasks. */
    mud::core::task_worker_pool<mud::core::simple_task> _pool;

    /** Map of supported event loop mechanisms */
    std::map<mud::core::handle::type_t, event_mechanism*> _mechanisms;
};

event_loop::impl::impl()
    : _queue(std::make_shared<mud::core::simple_task_queue>()),
      _pool(_queue)
{
    _mechanisms[mud::core::handle::type_t::SELECT]
        = new select_mechanism(_queue);

}

event_loop::impl::~impl()
{
    /* Make sure the loop is terminated */
    terminate();

    /* Cleanup all mechanism instances */
    for (auto mech: _mechanisms)
    {
        delete mech.second;
    }
}

void
event_loop::impl::register_handler(event&& event)
{
    // Register the event with the appropriate mechanism
    auto find = _mechanisms.find(event.handle()->type());
    if (find != _mechanisms.end())
    {
        find->second->register_handler(std::move(event));
    }
}

void
event_loop::impl::deregister_handler(event&& event)
{
    // Deregister the event from the appropriate mechanism
    auto find = _mechanisms.find(event.handle()->type());
    if (find != _mechanisms.end())
    {
        find->second->deregister_handler(std::move(event));
    }
}

void
event_loop::impl::loop()
{
    // Start the task workers in the pool
    _pool.start();

    // Start the loop on all mechanism
    std::vector<std::shared_future<void>> futures;
    for (auto& mech: _mechanisms)
    {
        futures.push_back(mech.second->initiate());
    }

    // Wait until all mechanisms have stopped
    for (auto future: futures)
    {
        future.wait();
    }

    // Wait for the pool to stop
    _pool.wait();
}

void
event_loop::impl::terminate()
{
    // Stop the task workers in the pool
    _pool.stop();

    // request to terminate  all mechanism.
    for (auto& mech: _mechanisms)
    {
        mech.second->terminate();
    }
}

/** The explicit implementation for POSIX event loops. */

event_loop::event_loop()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

event_loop::~event_loop()
{
}

void
event_loop::register_handler(event&& event)
{
    _impl->register_handler(std::move(event));
}

void
event_loop::deregister_handler(event&& event)
{
    _impl->deregister_handler(std::move(event));
}

void
event_loop::loop()
{
    _impl->loop();
}

void
event_loop::terminate()
{
    this->_impl->terminate();
}

/* static */ event_loop&
event_loop::global()
{
    static event_loop _global;
    return _global;
}

void
event_loop::impl_deleter::operator()(event_loop::impl* ptr) const
{
    delete ptr;
}

event::signal_type
operator|(event::signal_type lhs, event::signal_type rhs)
{
    return static_cast<event::signal_type>(
                    static_cast<std::underlying_type<event::signal_type>::type>(lhs) |
                    static_cast<std::underlying_type<event::signal_type>::type>(rhs));
}

event::signal_type
operator&(event::signal_type lhs, event::signal_type rhs)
{
    return static_cast<event::signal_type>(
                    static_cast<std::underlying_type<event::signal_type>::type>(lhs) &
                    static_cast<std::underlying_type<event::signal_type>::type>(rhs));
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

