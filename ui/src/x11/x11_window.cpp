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

#include "x11/x11_window.h"
#include "mud/ui/application.h"
#include "mud/ui/task.h"
#include "mud/ui/window.h"
#include "x11/x11_application.h"
#include "x11/x11_theme.h"
#include <X11/Xlib.h>

BEGIN_MUDLIB_UI_NS

window::impl::impl(window& wnd) : x11::control(wnd), _window(wnd) {}

window::impl::~impl() {}

void
window::impl::initialise()
{
    // Get the display
    x11::application& application = x11::application::instance();
    Display* dpy = application.display().get();

    // Get the gaphics context
    GC gc = x11::theme::instance().gc(x11::theme::item_t::WINDOW,
                                      x11::theme::state_t::PASSIVE);
    XGCValues gc_values;
    XGetGCValues(dpy, gc, GCForeground | GCBackground, &gc_values);

    // Create an x11 window
    int scr = DefaultScreen(dpy);
    Window wnd = ::XCreateSimpleWindow(
        dpy, RootWindow(dpy, scr), _window.property<position>().x(),
        _window.property<position>().y(), _window.property<size>().width(),
        _window.property<size>().height(), 1, gc_values.foreground,
        gc_values.background);
    XSelectInput(dpy, wnd, ExposureMask | KeyPressMask);
    XMapWindow(dpy, wnd);
    XFlush(dpy);
    NativeControl(wnd);

    // Initialise all controls
    for (auto& control : _controls) {
        control.get().initialise();
    }
}

void
window::impl::add_control(mud::ui::control& ctrl)
{
    _controls.push_back(ctrl);
}

void
window::impl_deleter::operator()(window::impl* ptr) const
{
    delete ptr;
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

const std::unique_ptr<window::impl, window::impl_deleter>&
window::impl::get(window& wnd)
{
    return wnd._impl;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
