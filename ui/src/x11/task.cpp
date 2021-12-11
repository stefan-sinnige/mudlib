#include <mud/ui/task.h>

BEGIN_MUDLIB_UI_NS

task::task()
{
}

task::task(mud::core::simple_task::function_type&& fn)
    : mud::core::simple_task(std::move(fn))
{
}

task::task(task&& other)
    : mud::core::simple_task(std::move(other))
{
}

task&
task::operator=(task&& other)
{
    mud::core::simple_task::operator=(std::move(other));
    return *this;
}

task::~task()
{
}

task_queue::task_queue()
{
}

task_queue::~task_queue()
{
}

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
    static mud::core::select_handle::signal _signal;
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

