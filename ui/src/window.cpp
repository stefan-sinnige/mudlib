#include "mud/ui/application.h"
#include "mud/ui/window.h"

BEGIN_MUDLIB_UI_NS

void
window::default_properties()
{
    _properties[std::type_index(typeid(position))] = position(0,0);
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

/* vi: set ai ts=4 expandtab: */

