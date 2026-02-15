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

#ifndef _MUD_UI_COCOA_EVENT_H_
#define _MUD_UI_COCOA_EVENT_H_

#include <Cocoa/Cocoa.h>
#include <memory>
#include <mud/ui/event.h>
#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

// Forward declaration of the NSEvent to mud::ui::event factory
std::unique_ptr<mud::ui::event>
event_factory(const NSEvent* cocoa_event);

// Forward declaration of the mud::ui::event to NSEvent factory
NSEvent*
event_factory(const mud::ui::event& event);

/*
 * Specialisation of NSEvent to mud::ui::event adapters
 */

template<typename Type>
class Adapter
{
public:
    /**
     * Convert an @c NSEvent to a @c mud::ui::event.
     */
    std::unique_ptr<mud::ui::event> operator()(const NSEvent* cocoa_event)
    {
        // Default implementation is a null-pointer for unsupported types.
        return std::unique_ptr<mud::ui::event>();
    }
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_EVENT_H_ */
