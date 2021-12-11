#include "cocoa/cocoa_control.h"
#include "cocoa/cocoa_event.h"
#include "cocoa/cocoa_window.h"
#include <Cocoa/Cocoa.h>

#include <iostream>

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<mud::ui::event>
Adapter<mud::ui::event::mouse>::operator()(const NSEvent* cocoa_event)
{
    // Find the associated window and determine which control is the target
    // for the mouse event.
    auto& window = cocoa::window::find([cocoa_event window]).get();
    NSPoint point = [cocoa_event locationInWindow];
    position pos(
            point.x,
            window.property<mud::ui::size>().height() - point.y);
    mud::ui::control& ctrl = window.control(pos);
    auto ev = std::make_unique<mud::ui::event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 * Event Factories
 * ====================================================================== */

std::unique_ptr<mud::ui::event>
event_factory(const NSEvent* cocoa_event)
{
    std::unique_ptr<mud::ui::event> ev;
    try
    {
        switch ((NSInteger)[cocoa_event type])
        {
            case NSEventTypeLeftMouseDown:
            case NSEventTypeLeftMouseUp:
            case NSEventTypeRightMouseDown:
            case NSEventTypeRightMouseUp:
            case NSEventTypeMouseMoved:
            case NSEventTypeLeftMouseDragged:
            case NSEventTypeRightMouseDragged:
            {
                Adapter<mud::ui::event::mouse> adapter;
                ev = adapter(cocoa_event);
                break;
            }
        }
    }
    catch (...)
    {
        // Error mapping event, ignoring it.
    }
    return ev;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

