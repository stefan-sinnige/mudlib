#include "mud/ui/pushbutton.h"

BEGIN_MUDLIB_UI_NS

void
pushbutton::default_properties()
{
    _properties[std::type_index(typeid(position))] = position(0,0);
    _properties[std::type_index(typeid(size))] = size(60, 25);
    _properties[std::type_index(typeid(text))] = text("");
}

void
pushbutton::event(pushbutton::mouse_event_func fn)
{
    _mouse_event_fn = fn;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

