#include "mud/ui/application.h"
#include "mud/event/event_loop.h"
#include "x11/x11_application.h"

BEGIN_MUDLIB_UI_NS

application::application()
{
    mud::event::event_loop::global().add_mechanism(
            mud::core::handle::type_t::X11);
}

application::~application()
{
}

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
{
}

void
application::finalise()
{
}

/* static */
application&
application::instance()
{
    static x11::application _instance;
    return _instance;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

