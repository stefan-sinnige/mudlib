#include "win32/win32_mechanism.h"
#include "mud/ui/event.h"
#include "mud/ui/exception.h"
#include "mud/ui/task.h"
#include "win32/win32_application.h"
#include "win32/win32_control.h"
#include "win32/win32_event.h"
#include <windows.h>

BEGIN_MUDLIB_UI_NS

/* Register the  W32 window message loop */
mud::event::event_mechanism_factory::registrar<
    mud::core::handle::type_t::W32WND, win32::mechanism>
    _registrar;

win32::mechanism::mechanism(
    const std::shared_ptr<mud::core::simple_task_queue>& queue)
  : mud::event::event_mechanism(queue), _running(false)
{}

win32::mechanism::~mechanism()
{
    terminate();
}

void
win32::mechanism::register_handler(mud::event::event&& event)
{}

void
win32::mechanism::deregister_handler(mud::event::event&& event)
{}

std::shared_future<void>
win32::mechanism::initiate()
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
win32::mechanism::terminate()
{
    if (_running.load() == true) {
        _terminate_signal.trigger();
        _running.store(false);
    }
}

bool
win32::mechanism::detachable() const
{
    return false;
}

void
win32::mechanism::setup()
{
    win32::application::instance().initialise();
}

void
win32::mechanism::closedown()
{
    win32::application::instance().finalise();
}

void
win32::mechanism::loop()
{
    // Call the PeekMessage to force the creation of a thread-queue such that
    // windows messages can be posted and handled within this loop.
    MSG msg;
    (void)::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    // Set-up the UI in the same theread that runs the event loop. After set-up
    // has completed, the UI is ready to create UI elements.
    setup();

    // Set-up the handlers to listen to and their handlers to invoke. We only
    // register the terminate and task-queue signal handles as the Winodws
    // UI message handler is already an integral pas of the wait function.
    int count = 0;
    _handlers[count++] = std::make_pair(
        mud::core::internal_handle<HANDLE>(_terminate_signal.handle()),
        std::bind(&win32::mechanism::terminate_signal_handler, this));
    _handlers[count++] = std::make_pair(
        mud::core::internal_handle<HANDLE>(
            task_queue::instance().available().handle()),
        std::bind(&win32::mechanism::task_queue_signal_handler, this));

    // Use the windows loop. This loop will be interrupted when the trigger is
    // raised, when UI tasks are added to the queue or when a Windows event
    // has occurred.
    while (_running) {
        static HANDLE handles[2] = { _handlers[0].first, _handlers[1].first };

        // Wait for and process the handles
        DWORD event = ::MsgWaitForMultipleObjects(
            count, handles, false, INFINITE, QS_ALLEVENTS | QS_ALLPOSTMESSAGE);
        if (event >= WAIT_OBJECT_0 && event < WAIT_OBJECT_0 + count) {
            // Call the event handler.
            _handlers[event - WAIT_OBJECT_0].second();
        } else if (event == WAIT_OBJECT_0 + count) {
            display_signal_handler();
        } else {
            throw std::system_error(::GetLastError(), std::system_category(),
                                    "MsgWaitForMultipleObjects");
        }
    }

    // Signal the end of the mechanism thread
    _promise.set_value();
}

void
win32::mechanism::terminate_signal_handler()
{
    // The _running flag is already set to false. This handler is merely used
    // to break the ::select.
    _terminate_signal.capture();
}

void
win32::mechanism::task_queue_signal_handler()
{
    task_queue::instance().available().capture();

    // If there is task in the queue, execute it
    task tsk;
    if (task_queue::instance().pop(tsk)) {
        tsk();
    }
}

void
win32::mechanism::display_signal_handler()
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        std::unique_ptr<event> event = event_factory(msg);
        if (event != nullptr) {
            event->control().dispatch(*event);
        }
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
