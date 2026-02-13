#include "posix/select_mechanism.h"
#include "timer_dispatcher.h"
#include <algorithm>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <system_error>

BEGIN_MUDLIB_CORE_NS

/* Register this mechaism to the factory */
event_mechanism_factory::registrar<mud::core::handle::type_t::SELECT,
                                   select_mechanism>
    _registrar;

select_mechanism::select_mechanism(
    const std::shared_ptr<mud::core::task_queue<void(void)>>& queue,
    const std::shared_ptr<mud::core::timer_dispatcher>& timers)
  : mud::core::event_mechanism(queue, timers)
  , _running(false)
  , _self_event(mud::core::uuid(), _self.handle(), event::signal_type::READING)
{
    LOG(log);

    /* Attach to the self-event notification. This special event is only used
     * to break a 'select', for example, when events have been added or
     * removed. There is no official notification of this event as it would
     * cause deadlocks. The triggering of this event happens on the same thread
     * while it holds the mutex lock (see 'demultiplex'. */
    INFO(log) << "Attaching to self event for select mechanism fd: "
              << mud::core::internal_handle<int>(_self.handle())
              << " [" << _self_event.topic() << "]" << std::endl;
    _events.push_back(std::move(_self_event));

    /* Add the timer-dispatcher changed event. This event is signalled when
     * the timer list has changed. */
    auto& timers_event = timers->changed();
    INFO(log) << "Attaching to timers-changed event for select mechanism fd:"
              << mud::core::internal_handle<int>(timers_event.handle())
              << " [" << timers_event.topic() << "]" << std::endl;
    _events.push_back(std::move(timers_event));

    /* Attach the event-loop timed-out notification to the timer dispatcher's
     * on_expired handled as the event-loop timeout is fully associated to the
     * sorted timers of the timer-dispatcher. */
    ::mud::core::broker::attach(_timed_out, timers.get(),
                                &mud::core::timer_dispatcher::on_expired);
}

select_mechanism::~select_mechanism()
{
    /* Make sure the loop is terminated */
    terminate();

    /* If the mechanism had run, the future would be valid. Ensure that
     * the thread is terminated (joined). */
    if (_future.valid()) {
        _future.wait();
        _thread.join();
    }
}

void
select_mechanism::add(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    LOG(log);
    INFO(log) << "Adding event for select mechanism fd: "
              << mud::core::internal_handle<int>(event.handle())
              << " [" << event.topic() << "]" << std::endl;
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        _events.erase(found);
    }
    _events.push_back(std::move(event));
    nop();
}

void
select_mechanism::remove(const event& event)
{
    std::lock_guard<std::mutex> lock(_lock);

    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        LOG(log);
        INFO(log) << "Removing event from select mechanism fd: "
                  << mud::core::internal_handle<int>(event.handle())
                  << " [" << event.topic() << "]" << std::endl;
        _events.erase(found);
        nop();
    }
}

std::shared_future<void>
select_mechanism::initiate()
{
    if (_running.exchange(true) == false) {
        if (_thread.joinable()) {
            _thread.join();
        }
        _promise = std::promise<void>();
        _thread = std::thread(&select_mechanism::loop, this);
        _future = _promise.get_future();
    }
    return _future;
}

void
select_mechanism::loop()
{
    LOG(log);
    INFO(log) << "Starting select event mechanism loop" << std::endl;

    /* Loop until we're told to stop. This loop is run on it's own thread. */
    while (_running.load() == true) {
        fd_set readfds, writefds, exceptfds;
        int maxfd;

        // Create the structure of all the file descriptors to wait for
        multiplex(readfds, writefds, exceptfds, maxfd);

        // Create the wait time, based on the event timers
        struct timeval tv, *timeout;
        auto now = std::chrono::system_clock::now();
        std::chrono::milliseconds duration = timers()->wait_duration(now);
        if (duration == std::chrono::milliseconds::max()) {
            TRACE(log) << "Selecting with no timeout" << std::endl;
            timeout = nullptr;
        }
        else {
            tv.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(
                    duration).count();
            tv.tv_usec = (duration.count() % 1000) * 1000;
            timeout = &tv;
            TRACE(log) << "Selecting with " << tv.tv_sec << "."
                       << std::setfill('0') << std::setw(6) << tv.tv_usec
                       << " timeout" << std::endl;
        }

        // Wait for the file-descriptors, or timeout.
        int ret = ::select(maxfd + 1, &readfds, &writefds, &exceptfds, timeout);
        if (ret < 0)
        {
            // If we have an EBADF, remove the associated event.
            if (errno == EBADF) {
                remove_badf();
            }
            else {
                throw std::system_error(errno, std::system_category(), "select");
            }
            continue;
        }
        else
        if (ret == 0) {
            // Publish the timeout notification
            ::mud::core::broker::publish(_timed_out);
        }

        // Handle all the file-descriptions that have been signaled.
        demultiplex(readfds, writefds, exceptfds);
    }

    /* Reset the event's command state such that we can re-use the loop
     * again. */
    nop();

    /* Signal the end of the thread. */
    _promise.set_value();
}

void
select_mechanism::terminate()
{
    LOG(log);
    INFO(log) << "Terminating select event mechanism loop" << std::endl;
    if (_running.load() == true) {
        _running.store(false);
        nop();
    }
}

void
select_mechanism::nop()
{
    _self.trigger();
}

void
select_mechanism::multiplex(fd_set& readfds, fd_set& writefds,
                            fd_set& exceptfds, int& maxfd)
{
    std::lock_guard<std::mutex> lock(_lock);
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    maxfd = -1;

    /* Trace logging of what is triggered */
    if (SEVERITY() <= mud::core::log::severity_t::trace) {
        LOG(log);
        TRACE(log) << "Multiplexing " << _events.size() << " events"
                   << std::endl;
        auto it = _events.begin();
        while (it != _events.end()) {
            if (it->handle() == nullptr) {
                TRACE(log) << "Event " << it->topic()
                           << " has invalid handle" << std::endl;
            }
            else {
                int handle = mud::core::internal_handle<int>(it->handle());
                TRACE(log) << "Handle " << std::setw(3) << std::setfill(' ')
                        << handle << ' '
                        << ((int)(it->mask() & event::signal_type::READING) 
                           ? 'r' : '-')
                        << ((int)(it->mask() & event::signal_type::WRITING)
                           ? 'w' : '-')
                        << "x " << it->topic() <<  std::endl;
            }
            it++;
        }
    }

    /* Add all registered event handlers */
    for (auto& event : _events) {
        if (event.handle() != nullptr &&
            (event.mask() & event::signal_type::READING) ==
                event::signal_type::READING) {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &readfds);
            FD_SET(handle, &exceptfds);
            if (handle > maxfd) {
                maxfd = handle;
            }
        }
        if (event.handle() != nullptr &&
            (event.mask() & event::signal_type::WRITING) ==
                event::signal_type::WRITING) {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &writefds);
            FD_SET(handle, &exceptfds);
            if (handle > maxfd) {
                maxfd = handle;
            }
        }
    }
}

void
select_mechanism::demultiplex(const fd_set& readfds, const fd_set& writefds,
                              const fd_set& exceptfds)
{
    std::lock_guard<std::mutex> lock(_lock);
    std::list<event> triggered;

    /* Trace logging of what is triggered */
    if (SEVERITY() <= mud::core::log::severity_t::trace) {
        LOG(log);
        TRACE(log) << "Demultiplexing " << _events.size() << " events"
                   << std::endl;
        auto it = _events.begin();
        while (it != _events.end()) {
            if (it->handle() == nullptr) {
                TRACE(log) << "Event " << it->topic()
                           << " has invalid handle" << std::endl;
            }
            else {
                int handle = mud::core::internal_handle<int>(it->handle());
                TRACE(log) << "Handle " << std::setw(3) << std::setfill(' ')
                        << handle << ' '
                        << ((int)(it->mask() & event::signal_type::READING) &&
                            FD_ISSET(handle, &readfds) ? 'r' : '-')
                        << ((int)(it->mask() & event::signal_type::WRITING) &&
                            FD_ISSET(handle, &writefds) ? 'w' : '-')
                        << (FD_ISSET(handle, &exceptfds) ? 'x' : '-')
                        << ' ' << it->topic() <<  std::endl;
            }
            it++;
        }
    }

    /* Check all excepts */

    /* Check all reads */
    auto event_it = _events.begin();
    while (event_it != _events.end()) {
        auto next_it = std::next(event_it);
        if (event_it->handle() == nullptr) {
            /* Invalid event, remove it */
            next_it = _events.erase(event_it);
        }
        else
        if (event::signal_type::READING ==
            (event_it->mask() & event::signal_type::READING))
        {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &readfds)) {
                if (event_it->handle() == _self.handle()) {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    _self.capture();
                } else {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    triggered.splice(triggered.end(), _events, event_it);
                }
            }
        }
        event_it = next_it;
    }

    /* Check all writes */
    event_it = _events.begin();
    while (event_it != _events.end()) {
        auto next_it = std::next(event_it);
        if (event_it->handle() == nullptr) {
            /* Invalid event, remove it */
            next_it = _events.erase(event_it);
        }
        else
        if (event::signal_type::WRITING ==
            (event_it->mask() & event::signal_type::WRITING))
        {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &writefds)) {
                if (event_it->handle() == _self.handle()) {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    _self.capture();
                } else {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    triggered.splice(triggered.end(), _events, event_it);
                }
            }
        }
        event_it = next_it;
    }

    /* Create tasks for all events that need to be triggered */
    event_it = triggered.begin();
    while (event_it != triggered.end()) {
        auto task = std::packaged_task<void()>(
            [this, ev=std::move(*event_it)]() mutable {
                LOG(log);
                INFO(log) << "Executing event " << ev.topic() << std::endl;
                ev.publish();
                if (::mud::core::broker::size(ev.topic())) {
                    add(std::move(ev));
                }
            });
        queue()->push(std::move(task));
        ++event_it;
    }
}

void
select_mechanism::remove_badf()
{
    std::lock_guard<std::mutex> lock(_lock);

    // Go through all events and remove those with a bad file descriptor.
    for (auto event_it = _events.begin(); event_it != _events.end(); )
    {
        if (event_it->handle() != nullptr) {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            int flags = fcntl(handle, F_GETFD);
            if (flags == -1 && errno == EBADF) {
                LOG(log);
                DEBUG(log) << "Select event mechanism removing bad fd: "
                           << handle << std::endl;
                event_it = _events.erase(event_it);
            }
            else {
                ++event_it;
            }
        }
        else {
            ++event_it;
        }
    }
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
