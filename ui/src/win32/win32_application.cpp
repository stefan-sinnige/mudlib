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

#include "win32/win32_application.h"
#include "mud/event/event_loop.h"
#include "mud/ui/exception.h"

BEGIN_MUDLIB_UI_NS

/* static */
application&
application::instance()
{
    static win32::application _instance;
    return _instance;
}

/* static */
win32::application&
win32::application::instance()
{
    return static_cast<win32::application&>(mud::ui::application::instance());
}

win32::application::application()
{
    mud::event::event_loop::global().add_mechanism(
        mud::core::handle::type_t::W32WND);
}

win32::application::~application() {}

void
win32::application::initialise()
{}

void
win32::application::finalise()
{}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
