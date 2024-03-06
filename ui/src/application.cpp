#include "mud/ui/application.h"
#include "mud/event/event_loop.h"
#include "mud/ui/control.h"
#include "mud/ui/event.h"

BEGIN_MUDLIB_UI_NS

application::application() {}

application::~application() {}

void
application::loop()
{
    mud::event::event_loop::global().loop();
}

std::shared_future<void>
application::terminate()
{
    return mud::event::event_loop::global().terminate();
}

void
application::push(task&& tsk)
{
    task_queue::instance().push(std::move(tsk));
}

void
application::initialise()
{}

void
application::finalise()
{}

std::future<void>
application::inject(const mud::ui::event& event)
{
    mud::ui::task tsk([&event]() { event.control().dispatch(event); });

    std::future<void> future = tsk.get_future();
    push(std::move(tsk));
    return future;
}

END_MUDLIB_UI_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_application.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_application.cpp"
#else
  #include "src/x11/x11_application.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
