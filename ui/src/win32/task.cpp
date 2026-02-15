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

#include <mud/ui/task.h>

BEGIN_MUDLIB_UI_NS

task::task() {}

task::task(mud::core::simple_task::function_type&& fn)
  : mud::core::simple_task(std::move(fn))
{}

task::task(task&& other) : mud::core::simple_task(std::move(other)) {}

task&
task::operator=(task&& other)
{
    mud::core::simple_task::operator=(std::move(other));
    return *this;
}

task::~task() {}

task_queue::task_queue() {}

task_queue::~task_queue() {}

void
task_queue::push(task&& tsk)
{
    mud::core::simple_task_queue::push(std::move(tsk));
    available().trigger();
}

bool
task_queue::pop(task& tsk)
{
    return mud::core::simple_task_queue::pop(tsk);
}

mud::core::handle::signal&
task_queue::available()
{
    static mud::core::windows_handle::signal _signal;
    return _signal;
}

/* static */
task_queue&
task_queue::instance()
{
    static task_queue _instance;
    return _instance;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
