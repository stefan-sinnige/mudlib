#include "src/posix/select_mechanism.h"
#include <algorithm>
#include <errno.h>
#include <system_error>

BEGIN_MUDLIB_EVENT_NS

/* Register this mechaism to the factory */
event_mechanism_factory::registrar<mud::core::handle::type_t::SELECT,
                                   select_mechanism>
    _registrar;

select_mechanism::select_mechanism(
    const std::shared_ptr<mud::core::simple_task_queue>& queue)
  : mud::event::event_mechanism(queue), _running(false)
{
    /* Always register the self-event to receive the trigger */
    event ev(_self.handle(), event::signal_type::READING, [&]() {
        _self.capture();
        return mud::event::event::return_type::CONTINUE;
    });
    _events.push_back(ev);
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
select_mechanism::register_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        _events.erase(found);
    }
    _events.push_back(event);
    nop();
}

void
select_mechanism::deregister_handler(event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
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
    /* Loop until we're told to stop. This loop is run on it's own thread. */
    while (_running.load() == true) {
        fd_set readfds, writefds, exceptfds;
        int maxfd;

        multiplex(readfds, writefds, exceptfds, maxfd);
        if (::select(maxfd + 1, &readfds, &writefds, &exceptfds, nullptr) ==
            -1) {
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
select_mechanism::terminate()
{
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
                    auto handler = event_it->handler();
                    event copy = *event_it;
                    mud::core::simple_task task([handler, copy, this]() {
                        if (handler() == event::return_type::CONTINUE) {
                            event ev = copy;
                            this->register_handler(std::move(ev));
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
                    auto handler = event_it->handler();
                    event copy = *event_it;
                    mud::core::simple_task task([handler, copy, this]() {
                        if (handler() == event::return_type::CONTINUE) {
                            event ev = copy;
                            this->register_handler(std::move(ev));
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

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
