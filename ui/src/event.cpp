#include "mud/ui/event.h"

BEGIN_MUDLIB_UI_NS

/**
 * Event
 */

event::event(type_t type, mud::ui::control& ctrl) : _type(type), _ctrl(ctrl) {}

event::~event() {}

/**
 * Expose event
 */

event::expose::expose(mud::ui::control& ctrl)
  : event(event::type_t::EXPOSE, ctrl)
{}

event::expose::~expose() {}

/**
 * Mouse event
 */

event::mouse::mouse(mud::ui::control& ctrl) : event(event::type_t::MOUSE, ctrl)
{}

event::mouse::~mouse() {}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
