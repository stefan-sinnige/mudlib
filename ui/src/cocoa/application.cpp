#include "mud/ui/application.h"
#include "mud/event/event_loop.h"
#include "cocoa/cocoa_application.h"

BEGIN_MUDLIB_UI_NS

application::application()
{
    mud::event::event_loop::global().add_mechanism(
            mud::core::handle::type_t::COCOA);
}

application::~application()
{
}

void
application::loop()
{
    mud::event::event_loop::global().loop();
}

void
application::terminate()
{
    mud::event::event_loop::global().terminate();
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
    static cocoa::application _instance;
    return _instance;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

