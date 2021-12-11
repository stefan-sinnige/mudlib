#include "mud/event/event_loop.h"
#include "mud/ui/exception.h"
#include "x11/x11_application.h"
#include <X11/Xlib.h>

BEGIN_MUDLIB_UI_NS

/* static */
application&
application::instance()
{
    static x11::application _instance;
    return _instance;
}
/* static */
x11::application&
x11::application::instance()
{
    return static_cast<x11::application&>(mud::ui::application::instance());
}

x11::application::application()
{
    mud::event::event_loop::global().add_mechanism(
            mud::core::handle::type_t::X11);
}

x11::application::~application()
{
}

void
x11::application::initialise()
{
    _display = std::shared_ptr<Display>(::XOpenDisplay(nullptr),
    [](Display* d) {
        ::XCloseDisplay(d);
    });
    if (_display == nullptr)
    {
        throw exception("cannot open display");
    }
}

void
x11::application::finalise()
{
    _display = nullptr;
}

const std::shared_ptr<Display>&
x11::application::display() const
{
    return _display;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

