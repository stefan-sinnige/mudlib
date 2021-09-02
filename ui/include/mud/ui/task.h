#ifndef _MUDLIB_UI_TASK_H_
#define _MUDLIB_UI_TASK_H_

#include <mud/ui/ns.h>
#include <mud/core/handle.h>
#include <mud/core/task.h>

BEGIN_MUDLIB_UI_NS

/**
 * @brief A UI task that is to be executed on the UI application thread.
 *
 * A UI task is any task that needs to be run on the UI application thread. This
 * includes the creation of windows and wigets.
 *
 */
class MUDLIB_UI_API task: public mud::core::simple_task
{
public:
    /**
     * @brief An invalid task.
     */
    task();

    /**
     * @brief Constructor of a task function to be executed.
     */
    task(mud::core::simple_task::function_type&& fn);

    /**
     * @brief Move constructor.
     */
    task(task&& other);

    /**
     * @brief Move assignment.
     */
    task& operator=(task&& rhs);

    /**
     * Destructor.
     */
    virtual ~task();
};

/**
 * @brief A queue of UI tasks.
 *
 * A queue to hold UI tasks. There can only be a single instance of a UI
 * task_queue.
 */
class MUDLIB_UI_API task_queue: public mud::core::simple_task_queue
{
public:
    /**
     * @brief Destructing a queue.
     */
    virtual ~task_queue();

    /**
     * @brief Push a new UI task to the queue.
     */
    void push(task&& tsk);

    /**
     * @brief Try to pop a UI task from the queue.
     *
     * If a task has been popped, it is moved to the @tsk object and @c true
     * is returned. If there is no task available, @c false is returned.
     */
    bool pop(task& tsk);

    /**
     * @brief Reference to the signalling resource.
     *
     * Return a handle signal that is used to signal the awaiting UI thread
     * of available tasks in the queue.
     */
    mud::core::handle::signal& available();

    /**
     * @brief Return the global singleton implementation.
     */
    static task_queue& instance();

private:
    /**
     * @brief Creating an empty queue.
     */
    task_queue();
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_TASK_H_ */

