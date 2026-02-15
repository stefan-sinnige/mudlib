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

#include "cocoa/cocoa_window.h"

@implementation Window
@end

@implementation View
- (BOOL)isFlipped
{
    return YES;
}
@end

BEGIN_MUDLIB_UI_NS

/* The map of all the current native controls and their associated
 * mud::ui::window object.
 * When a mud::ui::control oject is constructed and a native control is
 * assigned, then an entry is added to this map. When the control is
 * deleted, the entry is removed.
 *
 * This map is thread-safe as the assignment to the native control should
 * occur on the UI thread, as is the lookup of the control. */
std::map<NSWindow*, std::reference_wrapper<mud::ui::window>> g_windows;

window::impl::impl(window& wnd)
  : cocoa::control(wnd), _window(wnd), _wnd(nullptr), _vw(nullptr)
{}

window::impl::~impl()
{
    if (_wnd != nullptr) {
        g_windows.erase(_wnd);
    }
}

void
window::impl::initialise()
{
    // Create the Cocoa window
    NSRect rect = NSMakeRect(
        _window.property<position>().x(), _window.property<position>().y(),
        _window.property<size>().width(), _window.property<size>().height());
    _wnd = [[[Window alloc]
        initWithContentRect:rect
                  styleMask:NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable |
                            NSWindowStyleMaskResizable | NSWindowStyleMaskTitled
                    backing:NSBackingStoreBuffered
                      defer:false] autorelease];
    if (!_wnd) {
        throw std::runtime_error("cannot create window");
    }
    g_windows.emplace(_wnd, std::ref(_window));

    // Create the Cocoa view
    _vw = [[[View alloc] initWithFrame:NSZeroRect] autorelease];
    [_vw setWantsLayer:YES];
    [_wnd makeFirstResponder:_vw];
    [_wnd setContentView:_vw];

    // Initialise all controls
    for (auto& control : _controls) {
        control.get().initialise();
    }

    // Push it to the foreground
    [_wnd makeKeyAndOrderFront:_wnd];
}

void
window::impl::add_control(mud::ui::control& ctrl)
{
    _controls.push_back(ctrl);
}

mud::ui::control&
window::impl::control(const mud::ui::position& pos) const
{
    for (auto& control : _controls) {
        auto& ctrl_pos = control.get().property<position>();
        auto& ctrl_sz = control.get().property<size>();
        if (pos.x() >= ctrl_pos.x() &&
            pos.x() <= ctrl_pos.x() + ctrl_sz.width() &&
            pos.y() >= ctrl_pos.y() &&
            pos.y() <= ctrl_pos.y() + ctrl_sz.height()) {
            return control;
        }
    }
    return _window;
}

void
window::impl_deleter::operator()(window::impl* ptr) const
{
    delete ptr;
}

/* static */ std::reference_wrapper<mud::ui::window>
cocoa::window::find(NSWindow* wnd)
{
    // TODO: verify the current thread-id is the UI thread-id
    auto found = g_windows.find(wnd);
    if (found == g_windows.end()) {
        std::stringstream sstr;
        sstr << "no mud::ui::window associattion found for native control "
             << "[" << (void*)wnd << "]";
        throw std::runtime_error(sstr.str());
    }
    return found->second;
}

/** The explicit specialisation of a UI window */

window::window()
{
    default_properties();
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this));
}

window::~window() {}

void
window::initialise()
{
    _impl->initialise();
}

void
window::dispatch(const event& event)
{}

mud::ui::control&
window::control(const mud::ui::position& pos) const
{
    return _impl->control(pos);
}

const std::unique_ptr<window::impl, window::impl_deleter>&
window::impl::get(window& wnd)
{
    return wnd._impl;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
