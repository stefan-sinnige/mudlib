#ifndef _MUDLIB_UI_APPLICATION_H_
#define _MUDLIB_UI_APPLICATION_H_

#include <string>
#include <mud/ui/ns.h>
#include <mud/ui/task.h>

BEGIN_MUDLIB_UI_NS

// Fowrard declarations to implementation specific appications.
namespace x11 {
class application;
}
namespace win32 {
class application;
}

/**
 * @brief An application with a user interface.
 *
 * The application is created as soon as the application's @c instance is
 * used. Any window and widget created before the @c loop will only be created
 * when the @c loop is run.
 */
class MUDLIB_UI_API application
{
public:
    /**
     * @brief Destructor.
     */
    virtual ~application();

    /**
     * @brief Start the application event loop.
     *
     * This is essentially running the global event-loop and waits until the
     * application is finished. This will effectively start the application
     * and create any window and widget that is created at that point.
     */
    void loop();

    /**
     * @brief Request to terminate the application.
     */
    void terminate();

    /**
     * @brief Push a UI task to be executed.
     *
     * Push a UI task that is to be executed on the UI thread.
     */
    void push(task&& tsk);

    /**
     * Not copyable and not moveable
     */
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    application(application&&) = delete;
    application& operator=(application&&) = delete;

    /**
     * Return the global instance of the application.
     */
    static application& instance();

protected:
    /**
     * @brief Initialise the application
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void initialise();

    /**
     * @brief Finalise the application
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void finalise();

private:
    /**
     * @brief Default constructor.
     */
    application();

    friend class x11::application;
    friend class win32::application;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_APPLICATION_H_ */
