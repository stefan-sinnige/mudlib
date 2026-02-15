/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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

cocoa::control::control(mud::ui::control& ctrl) : _control(ctrl), _ctrl(nullptr)
{}

cocoa::control::~control()
{
    if (_ctrl != nullptr) {
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
    if (found == g_controls.end()) {
        std::stringstream sstr;
        sstr << "no mud::ui::control associattion found for native control "
             << "[" << (void*)ctrl << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

END_MUDLIB_UI_NS
