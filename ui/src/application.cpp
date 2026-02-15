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

#include "mud/ui/application.h"
#include "mud/event/event_loop.h"
#include "mud/ui/control.h"
#include "mud/ui/event.h"

BEGIN_MUDLIB_UI_NS

application::application() {}

application::~application() {}

void
application::loop()
{
    mud::event::event_loop::global().loop();
}

std::shared_future<void>
application::terminate()
{
    return mud::event::event_loop::global().terminate();
}

void
application::push(task&& tsk)
{
    task_queue::instance().push(std::move(tsk));
}

void
application::initialise()
{}

void
application::finalise()
{}

std::future<void>
application::inject(const mud::ui::event& event)
{
    mud::ui::task tsk([&event]() { event.control().dispatch(event); });

    std::future<void> future = tsk.get_future();
    push(std::move(tsk));
    return future;
}

END_MUDLIB_UI_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_application.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_application.cpp"
#else
  #include "src/x11/x11_application.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
