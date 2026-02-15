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

#include "cocoa/cocoa_event.h"
#include "cocoa/cocoa_control.h"
#include "cocoa/cocoa_window.h"
#include <Cocoa/Cocoa.h>

#include <iostream>

BEGIN_MUDLIB_UI_NS

/* ======================================================================
 * Mouse Events
 * ====================================================================== */

template<>
std::unique_ptr<mud::ui::event>
Adapter<mud::ui::event::mouse>::operator()(const NSEvent* cocoa_event)
{
    // Find the associated window and determine which control is the target
    // for the mouse event.
    auto& window = cocoa::window::find([cocoa_event window]).get();
    NSPoint point = [cocoa_event locationInWindow];
    position pos(point.x, window.property<mud::ui::size>().height() - point.y);
    mud::ui::control& ctrl = window.control(pos);
    auto ev = std::make_unique<mud::ui::event::mouse>(ctrl);
    return ev;
}

/* ======================================================================
 * Event Factories
 * ====================================================================== */

std::unique_ptr<mud::ui::event>
event_factory(const NSEvent* cocoa_event)
{
    std::unique_ptr<mud::ui::event> ev;
    try {
        switch ((NSInteger)[cocoa_event type]) {
            case NSEventTypeLeftMouseDown:
            case NSEventTypeLeftMouseUp:
            case NSEventTypeRightMouseDown:
            case NSEventTypeRightMouseUp:
            case NSEventTypeMouseMoved:
            case NSEventTypeLeftMouseDragged:
            case NSEventTypeRightMouseDragged: {
                Adapter<mud::ui::event::mouse> adapter;
                ev = adapter(cocoa_event);
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
