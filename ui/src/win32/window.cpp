#include "mud/ui/application.h"
#include "mud/ui/task.h"
#include "mud/ui/window.h"
#include "win32/win32_application.h"
#include <windows.h>

BEGIN_MUDLIB_UI_NS

window::window()
{
}

window::~window()
{
}

std::future<void>
window::show()
{
    task tsk(std::bind(&window::initialise, this));
    std::future<void> future = tsk.get_future();
    application::instance().push(std::move(tsk));
    return future;
}

LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    }
    return ::DefWindowProc(wnd, msg, wParam, lParam);
}

void
window::initialise()
{
    const char CLASS_NAME[] = "Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = ::GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    ::RegisterClass(&wc);

    HWND wnd = ::CreateWindowEx(
                    0,
                    CLASS_NAME,
                    "Window Title",
                    WS_OVERLAPPEDWINDOW,
                    10, 10, 100, 100,
                    nullptr,
                    nullptr,
                    ::GetModuleHandle(nullptr),
                    nullptr);
    if (wnd == nullptr)
    {
        throw std::system_error(::GetLastError(), std::system_category(),
                "CrateWindowEx");
    }
    ::ShowWindow(wnd, SW_NORMAL);
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

