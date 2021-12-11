#ifndef _MUD_UI_WIN32_EVENT_H_
#define _MUD_UI_WIN32_EVENT_H_

#include <memory>
#include <mud/ui/event.h>
#include <mud/ui/ns.h>
#include <windows.h>

BEGIN_MUDLIB_UI_NS

// Forward declaration of the Win32 event to mud::ui::event factory
std::unique_ptr<event>
event_factory(const MSG&);

// Forward declaration of the mud::ui::event to a MSG factory
std::unique_ptr<MSG>
event_factory(const mud::ui::event& event);

/*
 * Specialisation of a Win32 event to mud::ui::event adapters
 */

template<typename Type>
class Adapter
{
public:
    std::unique_ptr<event> operator()(const MSG& win32_event)
    {
        // Default implementation is a null-pointer.
        return std::unique_ptr<event>();
    }
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_WIN32_EVENT_H_ */
