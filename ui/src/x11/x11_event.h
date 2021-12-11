#ifndef _MUD_UI_COCOA_EVENT_H_
#define _MUD_UI_COCOA_EVENT_H_

#include <X11/Xlib.h>
#include <memory>
#include <mud/ui/event.h>
#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

// Forward declaration of the XEvent to mud::ui::event factory
std::unique_ptr<event>
event_factory(const XEvent& x11_event);

// Forward declaration of the mud::ui::event to XEvent factory
std::unique_ptr<XEvent>
event_factory(const mud::ui::event& event);

/*
 * Specialisation of XEvent to mud::ui::event adapters
 */

template<typename Type>
class Adapter
{
public:
    std::unique_ptr<event> operator()(const XEvent& x11_event)
    {
        // Default implementation is a null-pointer.
        return std::unique_ptr<event>();
    }
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_EVENT_H_ */
