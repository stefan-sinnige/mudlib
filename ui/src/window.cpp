#include "mud/ui/window.h"
#include "mud/ui/application.h"

BEGIN_MUDLIB_UI_NS

void
window::default_properties()
{
    _properties[std::type_index(typeid(position))] = position(0, 0);
    _properties[std::type_index(typeid(size))] = size(200, 150);
}

std::future<void>
window::show()
{
    task tsk(std::bind(&window::initialise, this));
    std::future<void> future = tsk.get_future();
    application::instance().push(std::move(tsk));
    return future;
}

END_MUDLIB_UI_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_window.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_window.cpp"
#else
  #include "src/x11/x11_window.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
