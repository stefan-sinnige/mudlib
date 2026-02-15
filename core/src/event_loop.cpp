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

#include "mud/core/event_loop.h"
#include "posix/select_mechanism.h"
#include "timer_dispatcher.h"
#include <algorithm>
#include <errno.h>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The implementation class of an @c mud::core::event_loop, managing
 * different underlying mechanisms, based on the type of handle being used.
 */
class event_loop::impl
{
public:
    /**
     * The type of a task that executes an event.
     */
    typedef std::packaged_task<void(void)> task_type;

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
     * Add an event handler with the loop.
     */
    void add(event&& event);

    /**
     * Remove an event handler associated to a handle from the loop.
     */
    void remove(const event& event);

    /**
     * Run the loop, waiting for all registered @c handle and invoke the
     * associated @event_handler when needed.
     */
    void loop();

    /*
     * Return the readiness future.
     */
    std::shared_future<void> ready() const;

    /*
     * Request to terminate the run @c loop.
     */
    std::shared_future<void> terminate();

    /*
     * Return the rimer dispatcher.
     */
    const std::shared_ptr<mud::core::timer_dispatcher>& timers();

    /**
     * Non-copyable
     */
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

private:
    /** Assert that the event-loop is not running */
    void assert_not_running();

    /** The task queue to hold signaled event tasks. */
    std::shared_ptr<mud::core::task_queue<void(void)>> _queue;

    /** The timer dispatcher to hold the event timers. */
    std::shared_ptr<mud::core::timer_dispatcher> _timers;

    /** The task worker pool to handle signaled event tasks. */
    mud::core::task_worker_pool<void(void)> _pool;

    /** Map of supported event loop mechanisms */
    std::map<mud::core::handle::type_t, std::unique_ptr<event_mechanism>>
        _mechanisms;

    /** Lock to protect the list of mechanisms. */
    std::mutex _lock;

    /** Flag to indicate that the loop is running */
    std::atomic_bool _running;

    /** The promise and shared-future to set the readiness of the loop */
    std::promise<void> _ready_promise;
    std::shared_future<void> _ready_future;

    /** The promise and shared-future to set the result of the loop */
    std::promise<void> _terminate_promise;
    std::shared_future<void> _terminate_future;
};

event_loop::impl::impl()
  : _queue(std::make_shared<mud::core::task_queue<void(void)>>())
  , _timers(std::make_shared<mud::core::timer_dispatcher>())
  , _pool(_queue, 2)
  , _running(false)
  , _ready_future(_ready_promise.get_future())
{
    /* Always load the select mechanism which is used by the self-signalling
     * resource. This is either a UDP socket connection or an unnamed pipe -
     * both of which are handled through the SELECT mechanism. */
    add_mechanism(mud::core::handle::type_t::SELECT);
}

event_loop::impl::~impl() {}

void
event_loop::impl::add_mechanism(mud::core::handle::type_t type)
{
    assert_not_running();
    std::lock_guard<std::mutex> lock(_lock);
    if (_mechanisms.find(type) == _mechanisms.end()) {
        auto mechanism =
            event_mechanism_factory::instance().create(type, _queue, _timers);
        _mechanisms[type] = std::move(mechanism);
    }
}

void
event_loop::impl::add(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);

    // Nothing to do for a null event
    if (event.topic().null()) {
        return;
    }

    // Add the event with the appropriate mechanism
    auto find = _mechanisms.find(event.handle()->type());
    if (find != _mechanisms.end()) {
        find->second->add(std::move(event));
    } else {
        throw std::invalid_argument("event for unregistered mechanism");
    }
}

void
event_loop::impl::remove(const event& event)
{
    std::lock_guard<std::mutex> lock(_lock);

    // Nothing to do for a null event
    if (event.topic().null()) {
        return;
    }

    // Remove the event from the appropriate mechanism
    auto find = _mechanisms.find(event.handle()->type());
    if (find != _mechanisms.end()) {
        find->second->remove(event);
    }
}

void
event_loop::impl::loop()
{
    LOG(log);
    INFO(log) << "Starting event loop" << std::endl;

    // Only run the loop if it is not yet running.
    if (_running.exchange(true) == true) {
        assert_not_running();
    }

    // Create new terminate promise.
    _terminate_promise = std::promise<void>();
    _terminate_future = _terminate_promise.get_future();

    // Start the task workers in the pool. When this returns all the task
    // workers in the pool are running.
    _pool.initiate();

    // Start the loop on all detachable  mechanism
    mud::core::handle::type_t non_detachable = mud::core::handle::type_t::NONE;
    std::vector<std::shared_future<void>> futures;
    {
        std::lock_guard<std::mutex> lock(_lock);
        for (auto& mech : _mechanisms) {
            if (mech.second->detachable()) {
                futures.push_back(mech.second->initiate());
            } else {
                non_detachable = mech.first;
            }
        }
    }

    // Run the mechanism on this thread that is not detachable
    if (non_detachable != mud::core::handle::type_t::NONE) {
        _mechanisms[non_detachable]->initiate();
    }

    // Set the promise to indicate we are ready
    _ready_promise.set_value();

    // Wait until all mechanisms have stopped
    for (auto future : futures) {
        // if (future.valid())
        {
            future.wait();
        }
    }

    // Wait for the pool to stop
    _pool.wait();

    // Set the promise to indiate we've stopped
    _terminate_promise.set_value();
}

std::shared_future<void>
event_loop::impl::ready() const
{
    return _ready_future;
}

std::shared_future<void>
event_loop::impl::terminate()
{
    LOG(log);
    INFO(log) << "Terminating event loop" << std::endl;

    std::lock_guard<std::mutex> lock(_lock);

    // Stop the task workers in the pool
    _pool.terminate();

    // request to terminate  all mechanism.
    for (auto& mech : _mechanisms) {
        mech.second->terminate();
    }
    _running = false;

    // Create new ready promise when we wish to restart the loop
    _ready_promise = std::promise<void>();
    _ready_future = _ready_promise.get_future();

    return _terminate_future;
}

const std::shared_ptr<mud::core::timer_dispatcher>&
event_loop::impl::timers()
{
    return _timers;
}

void
event_loop::impl::assert_not_running()
{
    if (_running == true) {
        throw std::invalid_argument("mud::core::event_loop already running");
    }
}

/** The explicit implementation for POSIX event loops. */

event_loop::event_loop()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

event_loop::~event_loop() {}

void
event_loop::add_mechanism(mud::core::handle::type_t type)
{
    _impl->add_mechanism(type);
}

void
event_loop::add(event&& event)
{
    _impl->add(std::move(event));
}

void
event_loop::remove(const event& event)
{
    _impl->remove(event);
}

void
event_loop::loop()
{
    _impl->loop();
}

std::shared_future<void>
event_loop::ready() const
{
    return _impl->ready();
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

const std::shared_ptr<mud::core::timer_dispatcher>&
event_loop::timers()
{
    return _impl->timers();
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

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
