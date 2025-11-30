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
    const std::shared_ptr<mud::core::simple_task_queue>& queue,
    const std::shared_ptr<mud::core::timer_dispatcher>& timers)
  : mud::core::event_mechanism(queue, timers), _running(false)
{
    LOG(log);

    /* Always register the self-event to receive the trigger */
    _self_event = mud::core::event(
            _self.handle(), event::signal_type::READING,
            [&]() {
                _self.capture();
                return mud::core::event::return_type::CONTINUE;
            });
    _events.push_back(_self_event);
    INFO(log) << "Registering self-event for select mechanism fd: "
              << mud::core::internal_handle<int>(_self.handle())
              << " [" << _self_event.id() << "]" << std::endl;

    /* Register the timer change event */
    mud::core::event timers_ev = timers->event();
    _events.push_back(timers_ev);
    INFO(log) << "Registering timers-event for select mechanism fd: "
              << mud::core::internal_handle<int>(timers_ev.handle())
              << " [" << timers_ev.id() << "]" << std::endl;
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
select_mechanism::register_handler(const event& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    LOG(log);
    INFO(log) << "Registering event for select mechanism fd: "
              << mud::core::internal_handle<int>(event.handle())
              << " [" << event.id() << "]" << std::endl;
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        _events.erase(found);
    }
    _events.push_back(event);
    nop();
}

void
select_mechanism::deregister_handler(const event& event)
{
    std::lock_guard<std::mutex> lock(_lock);

    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        LOG(log);
        INFO(log) << "Deregistering event from select mechanism fd: "
                  << mud::core::internal_handle<int>(event.handle())
                  << " [" << event.id() << "]" << std::endl;
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
            timeout = nullptr;
        }
        else {
            tv.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(
                    duration).count();
            tv.tv_usec = (duration.count() % 1000) * 1000;
            timeout = &tv;
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
            // Timeout, meaning that one of the timers has expired. We may be
            // able to schedule a task to handle timers like so
            // 
            //    mud::core::simple_task task([this]() {
            //        auto now = std::chrono::system_clock::now();
            //        this->timers()->dispatch(now);
            //    });
            //    queue()->push(std::move(task));
            //
            // But that would keep on triggering the select call with zero
            // timeouts until the task is processed and all the expired timers
            // have been processed. This would then have updated their expiry
            // to the next time point. This means we would have a number of
            // duplicate tasks in the queue. Until we have a reliable
            // mechanism to deal with this, we block the event-loop until those
            // timers are triggered by executing the dispatch straight away.
            auto now = std::chrono::system_clock::now();
            timers()->dispatch(now);
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

    /* Add all registered event handlers */
    for (auto& event : _events) {
        if (event.handle() != nullptr &&
            (event.mask() & event::signal_type::READING) ==
                event::signal_type::READING) {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &readfds);
            if (handle > maxfd) {
                maxfd = handle;
            }
        }
        if (event.handle() != nullptr &&
            (event.mask() & event::signal_type::WRITING) ==
                event::signal_type::WRITING) {
            int handle = mud::core::internal_handle<int>(event.handle());
            FD_SET(handle, &writefds);
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
    LOG(log);
    std::lock_guard<std::mutex> lock(_lock);

    /* Check all excepts */

    /* Check all reads */
    auto event_it = _events.begin();
    while (event_it != _events.end()) {
        if (event_it->handle() != nullptr &&
            (event_it->mask() & event::signal_type::READING) ==
                event::signal_type::READING) {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &readfds)) {
                if (event_it->handle() == _self.handle()) {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    (void)(event_it->handler())();
                    ++event_it;
                } else {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    INFO(log) << "Triggered read event " << event_it->id()
                              << std::endl;
                    auto handler = event_it->handler();
                    event ev = *event_it;
                    mud::core::simple_task task([handler, ev, this]() {
                        LOG(log);
                        INFO(log) << "Executing event " << ev.id() << std::endl;
                        if (handler() == event::return_type::CONTINUE) {
                            this->register_handler(ev);
                        }
                    });
                    queue()->push(std::move(task));
                    event_it = _events.erase(event_it);
                }
            } else {
                ++event_it;
            }
        } else {
            ++event_it;
        }
    }

    /* Check all writes */
    event_it = _events.begin();
    while (event_it != _events.end()) {
        if (event_it->handle() != nullptr &&
            (event_it->mask() & event::signal_type::WRITING) ==
                event::signal_type::WRITING) {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (FD_ISSET(handle, &writefds)) {
                if (event_it->handle() == _self.handle()) {
                    /* The handle is the 'self' object. Execute it straight
                     * away as it is used only to re-multiplex. */
                    (void)(event_it->handler())();
                    ++event_it;
                } else {
                    /* Any other event is taken off the list and processed as
                     * a task by a task worker. If the handler instructs to
                     * register the same event again, do so. */
                    INFO(log) << "Triggered write event " << event_it->id()
                              << std::endl;
                    auto handler = event_it->handler();
                    event ev = *event_it;
                    mud::core::simple_task task([handler, ev, this]() {
                        LOG(log);
                        INFO(log) << "Executing event " << ev.id() << std::endl;
                        if (handler() == event::return_type::CONTINUE) {
                            this->register_handler(ev);
                        }
                    });
                    queue()->push(std::move(task));
                    event_it = _events.erase(event_it);
                }
            } else {
                ++event_it;
            }
        } else {
            ++event_it;
        }
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
