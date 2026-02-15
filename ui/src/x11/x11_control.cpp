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

x11::control::control(mud::ui::control& ctrl) : _control(ctrl), _wnd(None) {}

x11::control::~control()
{
    if (_wnd != None) {
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
    if (found == g_controls.end()) {
        std::stringstream sstr;
        sstr << "no mud::ui::control associattion found for native control "
             << "[" << (unsigned long)ctrl << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

END_MUDLIB_UI_NS
