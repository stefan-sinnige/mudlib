#include "mud/ui/exception.h"
#include "x11/x11_application.h"
#include <X11/Xlib.h>

BEGIN_MUDLIB_UI_NS

x11::application::application()
{
}

x11::application::~application()
{
}

/* static */
x11::application&
x11::application::instance()
{
    return static_cast<x11::application&>(mud::ui::application::instance());
}

const std::shared_ptr<Display>&
x11::application::display() const
{
    return _display;
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

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

