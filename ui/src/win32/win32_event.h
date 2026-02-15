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

#ifndef _MUD_UI_WIN32_EVENT_H_
#define _MUD_UI_WIN32_EVENT_H_

#include <memory>
#include <mud/ui/event.h>
#include <mud/ui/ns.h>
#include <windows.h>

BEGIN_MUDLIB_UI_NS

// Forward declaration of the Win32 event to mud::ui::event factory
std::unique_ptr<event>
event_factory(const MSG&);

// Forward declaration of the mud::ui::event to a MSG factory
std::unique_ptr<MSG>
event_factory(const mud::ui::event& event);

/*
 * Specialisation of a Win32 event to mud::ui::event adapters
 */

template<typename Type>
class Adapter
{
public:
    std::unique_ptr<event> operator()(const MSG& win32_event)
    {
        // Default implementation is a null-pointer.
        return std::unique_ptr<event>();
    }
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_WIN32_EVENT_H_ */
