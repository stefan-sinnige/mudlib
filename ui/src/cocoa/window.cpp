#include "mud/ui/application.h"
#include "mud/ui/task.h"
#include "mud/ui/window.h"
#include "cocoa/cocoa_application.h"
#include <Cocoa/Cocoa.h>

@interface Window : NSWindow
@end
@implementation Window
@end

@interface View : NSView
@end
@implementation View
@end

BEGIN_MUDLIB_UI_NS

window::window()
{
}

window::~window()
{
}

std::future<void>
window::show()
{
    task tsk(std::bind(&window::initialise, this));
    std::future<void> future = tsk.get_future();
    application::instance().push(std::move(tsk));
    return future;
}

void
window::initialise()
{
    NSRect rect = NSMakeRect(10, 10, 100, 100);
    Window* wnd = [[[Window alloc]
                            initWithContentRect: rect
                            styleMask: NSWindowStyleMaskTitled
                            backing: NSBackingStoreBuffered
                            defer: false] autorelease];
    if (!wnd) {
        throw std::runtime_error("cannot create window");
    }
    [wnd makeKeyAndOrderFront: wnd];
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

