#include "mud/ui/application.h"
#include "mud/ui/task.h"
#include "mud/ui/window.h"
#include "x11/x11_application.h"
#include <X11/Xlib.h>

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
    x11::application& application = x11::application::instance();

    Window w;
    int s = DefaultScreen(application.display().get());
    w = ::XCreateSimpleWindow(
                    application.display().get(),
                    RootWindow(application.display().get(), s),
                    10, 10, 100, 100, 1,
                    BlackPixel(application.display().get(), s),
                    WhitePixel(application.display().get(), s));
    XSelectInput(application.display().get(), w, ExposureMask | KeyPressMask);
    XMapWindow(application.display().get(), w);
    XFlush(application.display().get());
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

