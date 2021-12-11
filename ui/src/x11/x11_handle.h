#ifndef _MUD_UI_X11_HANDLE_H_
#define _MUD_UI_X11_HANDLE_H_

#include <X11/Xlib.h>
#include <mud/core/handle.h>

typedef mud::core::basic_handle<
mud::core::handle::type_t::X11,
    XID,
    None>
    x11_handle;

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUD_UI_X11_HANDLE_H_ */

