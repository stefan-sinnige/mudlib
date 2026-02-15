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

#include "win32/win32_event.h"
#include "win32/win32_control.h"

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::mouse>::operator()(const MSG& win32_event)
{
    // Find the associated window and determine which control is the target
    // for the mouse event.
    auto ctrl = win32::control::find(win32_event.hwnd);
    auto ev = std::make_unique<event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 *  Event Factories
 * ====================================================================== */

std::unique_ptr<event>
event_factory(const MSG& msg)
{
    std::unique_ptr<event> ev;
    try {
        switch (msg.message) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                Adapter<event::mouse> adapter;
                ev = adapter(msg);
                break;
            }
        }
    } catch (...) {
        // Error mapping event, ignoring it.
    }
    return ev;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
