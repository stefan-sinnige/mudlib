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

#include "win32/win32_window.h"
#include "mud/ui/application.h"
#include "mud/ui/event.h"
#include "mud/ui/task.h"
#include "mud/ui/window.h"
#include "win32/win32_application.h"
#include <windows.h>

BEGIN_MUDLIB_UI_NS

window::impl::impl(window& wnd) : win32::control(wnd), _window(wnd) {}

window::impl::~impl() {}

LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {}
    return ::DefWindowProc(wnd, msg, wParam, lParam);
}

void
window::impl::initialise()
{
    // Create the window
    const char CLASS_NAME[] = "Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = ::GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    ::RegisterClass(&wc);

    HWND wnd = ::CreateWindowEx(
        0, CLASS_NAME, "Window Title", WS_OVERLAPPEDWINDOW,
        _window.property<position>().x(), _window.property<position>().y(),
        _window.property<size>().width(), _window.property<size>().height(),
        nullptr, nullptr, ::GetModuleHandle(nullptr), nullptr);
    if (wnd == nullptr) {
        throw std::system_error(::GetLastError(), std::system_category(),
                                "CrateWindowEx");
    }
    NativeControl(wnd);

    // Create the controls (should this be part of WM_CREATE ?).
    for (auto& control : _controls) {
        control.get().initialise();
    }

    ::ShowWindow(wnd, SW_NORMAL);
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
window::dispatch(const mud::ui::event& event)
{}

const std::unique_ptr<window::impl, window::impl_deleter>&
window::impl::get(window& wnd)
{
    return wnd._impl;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
