#include "mud/ui/exception.h"
#include "cocoa/cocoa_application.h"
#include <Cocoa/Cocoa.h>

BEGIN_MUDLIB_UI_NS

cocoa::application::application()
{
}

cocoa::application::~application()
{
}

/* static */
cocoa::application&
cocoa::application::instance()
{
    return static_cast<cocoa::application&>(mud::ui::application::instance());
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

