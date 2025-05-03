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
        _handle.reset(nullptr);
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

const std::unique_ptr<mud::core::handle>&
test_resource::handle() const
{
    return _handle;
}

/*
 * Test mechanism
 */

mud::event::event_mechanism_factory::registrar<
    mud::core::handle::type_t::__TEST, test_mechanism>
    _registrar;

test_mechanism::test_mechanism(
    const std::shared_ptr<mud::core::simple_task_queue>& queue,
    const std::shared_ptr<mud::event::timer_dispatcher>& timers)
  : mud::event::event_mechanism(queue, timers), _running(false)
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
test_mechanism::register_handler(mud::event::event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
        _events.erase(found);
    }
    _events.push_back(event);
}

void
test_mechanism::deregister_handler(mud::event::event&& event)
{
    std::lock_guard<std::mutex> lock(_lock);
    auto found = std::find(_events.begin(), _events.end(), event);
    if (found != _events.end()) {
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
        auto event_it = _events.begin();
        while (event_it != _events.end()) {
            int handle = mud::core::internal_handle<int>(event_it->handle());
            if (os_handle_map[handle] == os_handle_signal::READY_TO_READ) {
                /* Take the event off the list and have it handled as a
                 * task by a task worker.  If the handler instructs to
                 * register the same event again, do so with a copy. */
                auto handler = event_it->handler();
                mud::event::event copy = *event_it;
                mud::core::simple_task task([handler, copy, this]() {
                    if (handler() == mud::event::event::return_type::CONTINUE) {
                        mud::event::event ev = copy;
                        this->register_handler(std::move(ev));
                    }
                });
                queue()->push(std::move(task));
                event_it = _events.erase(event_it);
            } else {
                ++event_it;
            }
        }
        _lock.unlock();
        std::this_thread::yield();
    }

    // Signal the end of the mechanism thread
    _promise.set_value();
}
