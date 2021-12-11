#include "win32/win32_control.h"
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
std::map<HWND, std::reference_wrapper<mud::ui::control>> g_controls;

win32::control::control(mud::ui::control& ctrl) : _control(ctrl), _wnd(nullptr)
{}

win32::control::~control()
{
    if (_wnd != nullptr) {
        g_controls.erase(_wnd);
    }
}

void
win32::control::NativeControl(HWND wnd)
{
    // TODO: verify the current thread-id is the UI thread-id
    _wnd = wnd;
    g_controls.emplace(_wnd, std::ref(_control));
}

HWND
win32::control::NativeControl() const
{
    return _wnd;
}

/* static */ std::reference_wrapper<mud::ui::control>
win32::control::find(HWND ctrl)
{
    // TODO: verify the current thread-id is the UI thread-id
    auto found = g_controls.find(ctrl);
    if (found == g_controls.end()) {
        std::stringstream sstr;
        sstr << "no mud::ui::control associattion found for native control "
             << "[" << (unsigned long)ctrl << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

END_MUDLIB_UI_NS
