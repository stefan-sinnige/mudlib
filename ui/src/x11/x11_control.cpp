#include "x11/x11_control.h"
#include <map>
#include <sstream>

BEGIN_MUDLIB_UI_NS

/* The map of all the current native controls and their associated
 * mud::ui::control object.
 * When a mud::ui::control oject is constructed and a native control is
 * assigned, then an entry is added to this map. When the control is
 * deleted, the entry is removed.
 *
 * This map is thread-safe as the assignment to the native control should
 * occur on the UI thread, as is the lookup of the control. */
std::map<Window, std::reference_wrapper<mud::ui::control>> g_controls;

x11::control::control(mud::ui::control& ctrl)
    : _control(ctrl),  _wnd(None)
{
}

x11::control::~control()
{
    if (_wnd != None)
    {
        g_controls.erase(_wnd);
    }
}

void
x11::control::NativeControl(Window wnd)
{
    // TODO: verify the current thread-id is the UI thread-id
    _wnd = wnd;
    g_controls.emplace(_wnd, std::ref(_control));
}

Window
x11::control::NativeControl() const
{
    return _wnd;
}

/* static */ std::reference_wrapper<mud::ui::control>
x11::control::find(Window ctrl)
{
    // TODO: verify the current thread-id is the UI thread-id
    auto found = g_controls.find(ctrl);
    if (found == g_controls.end())
    {
        std::stringstream sstr;
        sstr << "no mud::ui::control associattion found for native control "
                << "[" << (unsigned long)ctrl << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

END_MUDLIB_UI_NS

