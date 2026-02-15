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

#include "x11/x11_event.h"
#include "x11/x11_control.h"

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Expose Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::expose>::operator()(const XEvent& x11_event)
{
    auto& expose_event = (const XExposeEvent&)(x11_event);
    if (expose_event.count == 0) {
        auto ctrl = x11::control::find(x11_event.xany.window);
        auto ev = std::make_unique<event::expose>(ctrl);
        return ev;
    }
    return std::unique_ptr<event>();
}

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<event>
Adapter<event::mouse>::operator()(const XEvent& x11_event)
{
    auto& button_event = (const XButtonEvent&)(x11_event);
    auto ctrl = x11::control::find(x11_event.xany.window);
    auto ev = std::make_unique<event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 * Event Factories
 * ====================================================================== */

std::unique_ptr<event>
event_factory(const XEvent& x11_event)
{
    std::unique_ptr<event> ev;
    switch (x11_event.type) {
        case Expose: {
            Adapter<event::expose> adapter;
            ev = adapter(x11_event);
            break;
        }
        case ButtonPress:
        case ButtonRelease: {
            Adapter<event::mouse> adapter;
            ev = adapter(x11_event);
            break;
        }
        default:
            break;
    }
    return ev;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
