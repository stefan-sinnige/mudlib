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

#include "x11/x11_mechanism.h"
#include "mud/ui/event.h"
#include "mud/ui/exception.h"
#include "mud/ui/task.h"
#include "x11/x11_application.h"
#include "x11/x11_control.h"
#include "x11/x11_event.h"
#include <memory>
#include <sys/select.h>

BEGIN_MUDLIB_UI_NS

mud::event::event_mechanism_factory::registrar<mud::core::handle::type_t::X11,
                                               x11::mechanism>
    _registrar;

x11::mechanism::mechanism(
    const std::shared_ptr<mud::core::simple_task_queue>& queue)
  : mud::event::event_mechanism(queue), _running(false)
{}

x11::mechanism::~mechanism()
{
    terminate();
}

void
x11::mechanism::register_handler(mud::event::event&& event)
{}

void
x11::mechanism::deregister_handler(mud::event::event&& event)
{}

std::shared_future<void>
x11::mechanism::initiate()
{
    // As this mechanism is not-detachable, run it on the current thread and
    // only return after completion.
    bool was_running = _running.exchange(true);
    if (was_running == false) {
        loop();
        _promise = std::promise<void>();
        _future = _promise.get_future();
    }
    return _future;
}

void
x11::mechanism::terminate()
{
    if (_running.load() == true) {
        _terminate_signal.trigger();
        _running.store(false);
    }
}

bool
x11::mechanism::detachable() const
{
    return false;
}

void
x11::mechanism::setup()
{
    x11::application::instance().initialise();
}

void
x11::mechanism::closedown()
{
    x11::application::instance().finalise();
}

void
x11::mechanism::loop()
{
    fd_set readfds;

    // Set-up the UI in the same theread that runs the event loop. After set-up
    // has completed, the UI is ready to create UI elements.
    setup();

    // Set-up the handles to listen to and their handlers to invoke.
    _handlers[mud::core::internal_handle<int>(_terminate_signal.handle())] =
        std::bind(&x11::mechanism::terminate_signal_handler, this);
    _handlers[mud::core::internal_handle<int>(
        task_queue::instance().available().handle())] =
        std::bind(&x11::mechanism::task_queue_signal_handler, this);
    _handlers[::XConnectionNumber(
        x11::application::instance().display().get())] =
        std::bind(&x11::mechanism::display_signal_handler, this);

    // Use ::select to wait for all event.
    while (_running) {
        // Set-up the select and wait until signalled.
        FD_ZERO(&readfds);
        int maxfd = -1;
        for (auto& entry : _handlers) {
            FD_SET(entry.first, &readfds);
            if (maxfd < entry.first) {
                maxfd = entry.first;
            }
        }
        if (::select(maxfd + 1, &readfds, nullptr, nullptr, nullptr) < 0) {
            throw std::system_error(errno, std::system_category(), "select");
        }

        // Invoke any signalled hander.
        for (auto& entry : _handlers) {
            if (FD_ISSET(entry.first, &readfds)) {
                entry.second();
            }
        }

        // Yield this thread
        std::this_thread::yield();
    }

    // Close-down the UI, after which UI elements can no longer be used.
    closedown();

    // Signal the end of the mechanism thread
    _promise.set_value();
}

void
x11::mechanism::terminate_signal_handler()
{
    // The _running flag is already set to false. This handler is merely used
    // to break the ::select.
    _terminate_signal.capture();
}

void
x11::mechanism::task_queue_signal_handler()
{
    task_queue::instance().available().capture();

    // If there is task in the queue, execute it
    task tsk;
    if (task_queue::instance().pop(tsk)) {
        tsk();
    }
}

void
x11::mechanism::display_signal_handler()
{
    // If there are pending XEvents, process them
    while (::XPending(x11::application::instance().display().get()) > 0) {
        XEvent x11_event;
        XNextEvent(x11::application::instance().display().get(), &x11_event);
        std::unique_ptr<event> event = event_factory(x11_event);
        if (event != nullptr) {
            event->control().dispatch(*event);
        } else {
        }
    }
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
