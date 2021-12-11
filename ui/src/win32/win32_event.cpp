#include "win32/win32_event.h"
#include "win32/win32_control.h"

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::mouse>::operator()(const MSG& win32_event)
{
    // Find the associated window and determine which control is the target
    // for the mouse event.
    auto ctrl = win32::control::find(win32_event.hwnd);
    auto ev = std::make_unique<event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 *  Event Factories
 * ====================================================================== */

std::unique_ptr<event>
event_factory(const MSG& msg)
{
    std::unique_ptr<event> ev;
    try {
        switch (msg.message) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                Adapter<event::mouse> adapter;
                ev = adapter(msg);
                break;
            }
        }
    } catch (...) {
        // Error mapping event, ignoring it.
    }
    return ev;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
