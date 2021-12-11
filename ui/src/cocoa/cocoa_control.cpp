#include "cocoa/cocoa_control.h"
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
std::map<NSControl*, std::reference_wrapper<mud::ui::control>> g_controls;

cocoa::control::control(mud::ui::control& ctrl)
    : _control(ctrl),  _ctrl(nullptr)
{
}

cocoa::control::~control()
{
    if (_ctrl != nullptr)
    {
        g_controls.erase(_ctrl);
    }
}

void
cocoa::control::NativeControl(NSControl* ctrl)
{
    // TODO: verify the current thread-id is the UI thread-id
    _ctrl = ctrl;
    g_controls.emplace(_ctrl, std::ref(_control));
}

NSControl*
cocoa::control::NativeControl() const
{
    return _ctrl;
}

/* static */ std::reference_wrapper<mud::ui::control>
cocoa::control::find(NSControl* ctrl)
{
    // TODO: verify the current thread-id is the UI thread-id
    auto found = g_controls.find(ctrl);
    if (found == g_controls.end())
    {
        std::stringstream sstr;
        sstr << "no mud::ui::control associattion found for native control "
                << "[" << (void*)ctrl << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

END_MUDLIB_UI_NS

