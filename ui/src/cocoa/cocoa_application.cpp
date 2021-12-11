#include "mud/event/event_loop.h"
#include "mud/ui/exception.h"
#include "cocoa/cocoa_application.h"
#include "cocoa/cocoa_event.h"
#include <Cocoa/Cocoa.h>

BEGIN_MUDLIB_UI_NS

/* static */
application&
application::instance()
{
    static cocoa::application _instance;
    return _instance;
}

/* static */
cocoa::application&
cocoa::application::instance()
{
    return static_cast<cocoa::application&>(mud::ui::application::instance());
}

cocoa::application::application()
{
    mud::event::event_loop::global().add_mechanism(
            mud::core::handle::type_t::COCOA);
}

cocoa::application::~application()
{
}

void
cocoa::application::initialise()
{
}

void
cocoa::application::finalise()
{
}

void
cocoa::application::wakeup()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSEvent* event = [NSEvent otherEventWithType: NSEventTypeApplicationDefined
                            location: NSMakePoint(0,0)
                            modifierFlags: 0
                            timestamp: 0.0
                            windowNumber: 0
                            context: nil
                            subtype: 0
                            data1: 0
                            data2: 0];
    [NSApp postEvent: event atStart: YES];
    [pool release];
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

