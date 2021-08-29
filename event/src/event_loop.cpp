#include "mud/event/event_loop.h"
#include "posix/select_mechanism.h"
#include <errno.h>
#include <algorithm>
#include <system_error>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
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
     * Add a mechanism
     */
    void add_mechanism(mud::core::handle::type_t type);

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
    std::shared_future<void> terminate();

    /**
     * Non-copyable
     */
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

private:
    /** Assert that the event-loop is not running */
    void assert_not_running();

    /** The task queue to hold signaled event tasks. */
    std::shared_ptr<mud::core::simple_task_queue> _queue;

    /** The task worker pool to handle signaled event tasks. */
    mud::core::task_worker_pool<mud::core::simple_task> _pool;

    /** Map of supported event loop mechanisms */
    std::map<mud::core::handle::type_t,
        std::unique_ptr<event_mechanism>> _mechanisms;

    /** Lock to protect the list of mechanisms. */
    std::mutex _lock;

    /** Flag to indicate that the loop is running */
    std::atomic_bool _running;

    /** The promise and sahred-future to set the result of the loop */
    std::promise<void> _promise;
    std::shared_future<void> _future;
};

event_loop::impl::impl()
    : _queue(std::make_shared<mud::core::simple_task_queue>()),
      _pool(_queue, 2), _running(false)
{
    /* Get the future from the promise such that it can easily be shared
     * with multiple threads that terminate the loop, whitout raising a
     * future_already_retrieved exception. */
    _future = _promise.get_future();

    /* Always load the select mechanism which is used by the self-signalling
     * resource. This is either a UDP socket connection or an unnamed pipe -
     * both of which are handled through the SELECT mechanism. */
    add_mechanism(mud::core::handle::type_t::SELECT);
}

event_loop::impl::~impl()
{
}

void
event_loop::impl::add_mechanism(mud::core::handle::type_t type)
{
    assert_not_running();
    std::lock_guard<std::mutex> lock(_lock);
    if (_mechanisms.find(type) == _mechanisms.end())
    {
        auto mechanism = event_mechanism_factory::instance().create(
                        type, _queue);
        _mechanisms[type] = std::move(mechanism);
    }
}

void
event_loop::impl::register_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);

    // Register the event with the appropriate mechanism
    auto find = _mechanisms.find(event.handle()->type());
    if (find != _mechanisms.end())
    {
        find->second->register_handler(std::move(event));
    }
    else
    {
        throw std::invalid_argument("event for unregistered mechanism");
    }
}

void
event_loop::impl::deregister_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);

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
    // Only run the loop if it is not yet running.
    if (_running.exchange(true) == true)
    {
        assert_not_running();
    }

    // Start the task workers in the pool
    _pool.initiate();

    // Start the loop on all detachable  mechanism
    mud::core::handle::type_t non_detachable = mud::core::handle::type_t::NONE;
    std::vector<std::shared_future<void>> futures;
    {
        std::lock_guard<std::mutex> lock(_lock);
        for (auto& mech: _mechanisms)
        {
            if (mech.second->detachable())
            {
                futures.push_back(mech.second->initiate());
            }
            else
            {
                non_detachable = mech.first;
            }
        }
    }

    // Run the mechanism on this thread that is not detachable
    if (non_detachable != mud::core::handle::type_t::NONE)
    {
        _mechanisms[non_detachable]->initiate();
    }

    // Wait until all mechanisms have stopped
    for (auto future: futures)
    {
        //if (future.valid())
        {
            future.wait();
        }
    }

    // Wait for the pool to stop
    _pool.wait();

    // Set the promise to indiate we've stopped
    _promise.set_value();
}

std::shared_future<void>
event_loop::impl::terminate()
{
    std::lock_guard<std::mutex> lock(_lock);

    // Stop the task workers in the pool
    _pool.terminate();

    // request to terminate  all mechanism.
    for (auto& mech: _mechanisms)
    {
        mech.second->terminate();
    }
    _running = false;

    return _future;
}

void
event_loop::impl::assert_not_running()
{
    if (_running == true)
    {
        throw std::invalid_argument("mud::event::event_loop already running");
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
event_loop::add_mechanism (mud::core::handle::type_t type)
{
    _impl->add_mechanism(type);
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

std::shared_future<void>
event_loop::terminate()
{
    return _impl->terminate();
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

