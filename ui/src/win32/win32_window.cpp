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
