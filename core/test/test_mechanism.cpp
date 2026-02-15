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

#include "test_mechanism.h"

/**
 * Simulate a resource with an ever increasing handle number whenever a new
 * resource is allocated. Keep track if the signal state of each resource
 * after it has been allocated.
 */
static int os_handle_last = 0;
std::map<int, os_handle_signal> os_handle_map;

/*
 * Test resourcee
 */

test_resource::test_resource()
{
    int handle = ++os_handle_last;
    os_handle_map[handle] = os_handle_signal::NONE;
    _handle = std::unique_ptr<mud::core::handle>(
        new mud::core::__test_handle(handle));
    _ch = '\0';
}

test_resource::~test_resource()
{
    if (_handle != nullptr) {
        _handle.reset();
    }
}

void
test_resource::write(char ch)
{
    _ch = ch;
    int handle = mud::core::internal_handle<int>(_handle);
    os_handle_map[handle] = os_handle_signal::READY_TO_READ;
}

char
test_resource::read()
{
    int handle = mud::core::internal_handle<int>(_handle);
    os_handle_map[handle] = os_handle_signal::NONE;
    char c = _ch;
    _ch = '\0';
    return c;
}

const std::shared_ptr<mud::core::handle>&
test_resource::handle() const
{
    return _handle;
}

/*
 * Test mechanism
 */

mud::core::event_mechanism_factory::registrar<
    mud::core::handle::type_t::__TEST, test_mechanism>
    _registrar;

test_mechanism::test_mechanism(
    const std::shared_ptr<mud::core::task_queue<void(void)>>& queue,
    const std::shared_ptr<mud::core::timer_dispatcher>& timers)
  : mud::core::event_mechanism(queue, timers), _running(false)
{}

test_mechanism::~test_mechanism()
{
    terminate();
    if (_future.valid()) {
        _future.wait();
        _thread.join();
    }
}

void
test_mechanism::add(mud::core::event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    LOG(log); 
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        INFO(log) << "Removing existing event " << event.topic() << std::endl;
        _events.erase(found);
    }
    INFO(log) << "Adding event " << event.topic() << std::endl;
    _events.push_back(std::move(event));
}

void
test_mechanism::remove(const mud::core::event& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        LOG(log); 
        INFO(log) << "Removing event " << event.topic() << std::endl;
        _events.erase(found);
    }
}

std::shared_future<void>
test_mechanism::initiate()
{
    bool was_running = _running.exchange(true);
    if (was_running == false) {
        if (_thread.joinable()) {
            _thread.join();
        }
        _promise = std::promise<void>();
        _thread = std::thread(&test_mechanism::loop, this);
        _future = _promise.get_future();
    }
    return _future;
}

void
test_mechanism::terminate()
{
    if (_running.load() == true) {
        _running.store(false);
    }
}

void
test_mechanism::loop()
{
    // Polling all events
    while (_running) {
        _lock.lock();

        /* Move all events that need to be triggered to another list */
        std::list<mud::core::event> triggered;
        auto event_it = _events.begin();
        while (event_it != _events.end()) {
            auto next_it = std::next(event_it);
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (os_handle_map[handle] == os_handle_signal::READY_TO_READ) {
                triggered.splice(triggered.end(), _events, event_it);
            }
            event_it = next_it;
        }

        /* Trigger all events as a task that is handled by a task worker. The
         * handler can instruct to register the event again. */
        event_it = triggered.begin();
        while (event_it != triggered.end()) {
            auto task = std::packaged_task<void()>(
                [this, ev=std::move(*event_it)]() mutable {
                    LOG(log); 
                    INFO(log) << "Executing event " << ev.topic()
                              << std::endl;
                    ev.publish();
                    if (::mud::core::broker::size(ev.topic())) {
                        INFO(log) << "Re-adding event " << ev.topic()
                                  << std::endl;
                        add(std::move(ev));
                    }
                });
            queue()->push(std::move(task));
            ++event_it;
        }
        _lock.unlock();
        std::this_thread::yield();
    }

    // Signal the end of the mechanism thread
    _promise.set_value();
}
