#include <Cocoa/Cocoa.h>
#include "cocoa/cocoa_mechanism.h"
#include "cocoa/cocoa_application.h"
#include "mud/ui/exception.h"
#include "mud/ui/task.h"

BEGIN_MUDLIB_UI_NS

mud::event::event_mechanism_factory::registrar<
mud::core::handle::type_t::COCOA,
    cocoa::mechanism> _registrar;

cocoa::mechanism::mechanism(
        const std::shared_ptr<mud::core::simple_task_queue>& queue)
    : mud::event::event_mechanism(queue), _running(false)
{
}

cocoa::mechanism::~mechanism()
{
    terminate();
}

void
cocoa::mechanism::register_handler(mud::event::event&& event)
{
}

void
cocoa::mechanism::deregister_handler(mud::event::event&& event)
{
}

std::shared_future<void>
cocoa::mechanism::initiate()
{
    // As this mechanism is not-detachable, run it on the current thread and
    // only return after completion.
    bool was_running = _running.exchange(true);
    if (was_running == false)
    {
        loop();
        _promise = std::promise<void>();
        _future = _promise.get_future();
    }
    return _future;
}

void
cocoa::mechanism::terminate()
{
    if (_running.load() == true)
    {
        _running.store(false);
        _terminate_signal.trigger();
        cocoa::application::instance().wakeup();
    }
}

bool
cocoa::mechanism::detachable() const
{
    return false;
}

void
cocoa::mechanism::setup()
{
    cocoa::application::instance().initialise();
}

void
cocoa::mechanism::closedown()
{
    cocoa::application::instance().finalise();
}

void
cocoa::mechanism::loop()
{
    [NSApplication sharedApplication];
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    while (_running)
    {
        [pool release];
        pool = [[NSAutoreleasePool alloc] init];

        // Check for Task queue events
        if (task_queue::instance().available().capture())
        {
            task_queue_signal_handler();
        }

        // Check for termination
        if (_terminate_signal.capture())
        {
            if (!_running)
            {
            }
            terminate_signal_handler();
        }

        // Process UI Events
        NSEvent* event;
        do
        {
            event = [NSApp  nextEventMatchingMask: NSEventMaskAny
                            untilDate: [NSDate distantPast]
                            inMode: NSDefaultRunLoopMode
                            dequeue: YES];
            [NSApp sendEvent: event];
        }
        while (event != nullptr);

        // Use the UI run-loop to block on any event (including custom ones
        // from the task-queue and the termination signal.
        event = [NSApp  nextEventMatchingMask: NSEventMaskAny
                        untilDate: [NSDate distantFuture]
                        inMode: NSDefaultRunLoopMode
                        dequeue: NO];
    }
    [pool release];

    // Signal the end of the mechanism thread
    _promise.set_value();
}

void
cocoa::mechanism::terminate_signal_handler()
{
    // The _running flag is already set to false. This handler is merely used
    // to break the ::select.
}

void
cocoa::mechanism::task_queue_signal_handler()
{
    task tsk;
    while (task_queue::instance().pop(tsk))
    {
        tsk();
    }
}

void
cocoa::mechanism::display_signal_handler()
{
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

