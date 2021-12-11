#ifndef _MUD_UI_COCOA_EVENT_H_
#define _MUD_UI_COCOA_EVENT_H_

#include <memory>
#include <mud/ui/ns.h>
#include <mud/ui/event.h>
#include <Cocoa/Cocoa.h>

BEGIN_MUDLIB_UI_NS

// Forward declaration of the NSEvent to mud::ui::event factory
std::unique_ptr<mud::ui::event> event_factory(const NSEvent* cocoa_event);

// Forward declaration of the mud::ui::event to NSEvent factory
NSEvent* event_factory(const mud::ui::event& event);

/*
 * Specialisation of NSEvent to mud::ui::event adapters
 */

template<typename Type>
class Adapter
{
public:
    /**
     * Convert an @c NSEvent to a @c mud::ui::event.
     */
    std::unique_ptr<mud::ui::event>
    operator()(const NSEvent* cocoa_event) {
        // Default implementation is a null-pointer for unsupported types.
        return std::unique_ptr<mud::ui::event>();
    }
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_EVENT_H_ */

