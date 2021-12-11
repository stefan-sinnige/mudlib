#include "win32/win32_application.h"
#include "mud/event/event_loop.h"
#include "mud/ui/exception.h"

BEGIN_MUDLIB_UI_NS

/* static */
application&
application::instance()
{
    static win32::application _instance;
    return _instance;
}

/* static */
win32::application&
win32::application::instance()
{
    return static_cast<win32::application&>(mud::ui::application::instance());
}

win32::application::application()
{
    mud::event::event_loop::global().add_mechanism(
        mud::core::handle::type_t::W32WND);
}

win32::application::~application() {}

void
win32::application::initialise()
{}

void
win32::application::finalise()
{}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
