#include "x11/x11_control.h"
#include "x11/x11_event.h"

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Expose Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::expose>::operator()(const XEvent& x11_event)
{
    auto& expose_event = (const XExposeEvent&)(x11_event);
    if (expose_event.count == 0)
    {
        auto ctrl = x11::control::find(x11_event.xany.window);
        auto ev = std::make_unique<event::expose>(ctrl);
        return ev;
    }
    return std::unique_ptr<event>();
}

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::mouse>::operator()(const XEvent& x11_event)
{
    auto& button_event = (const XButtonEvent&)(x11_event);
    auto ctrl = x11::control::find(x11_event.xany.window);
    auto ev = std::make_unique<event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 * Event Factories
 * ====================================================================== */

std::unique_ptr<event>
event_factory(const XEvent& x11_event)
{
    std::unique_ptr<event> ev;
    switch (x11_event.type)
    {
        case Expose:
        {
            Adapter<event::expose> adapter;
            ev = adapter(x11_event);
            break;
        }
        case ButtonPress:
        case ButtonRelease:
        {
            Adapter<event::mouse> adapter;
            ev = adapter(x11_event);
            break;
        }
        default:
            break;
    }
    return ev;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

