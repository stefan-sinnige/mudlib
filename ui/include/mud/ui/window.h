#ifndef _MUDLIB_UI_WINDOW_H_
#define _MUDLIB_UI_WINDOW_H_

#include <mud/ui/ns.h>
#include <mud/ui/object.h>

BEGIN_MUDLIB_UI_NS

/**
 * @brief A window is the top-level user interface element container.
 *
 * A window is the base class for all the user interface elements. This
 * includes widgets like buttons, text-boxes, canvases. It also includes
 * elements that are containers for other widgets, including panels and
 * top level windows.
 *
 */
class MUDLIB_UI_API window: public object
{
public:
    /**
     * @brief Construct a window.
     *
     * The window is constructed but will not be shown until @c show is
     * invoked.
     */
    window();

    /**
     * @brief Remove the window and its allocated resources.
     */
    virtual ~window();

    /**
     * @brief Show the window.
     *
     * The window will be created and initialised. This involves incoking
     * the implementation dependent routines on the UI thread and will
     * involve calling @c initialise.
     *
     * @return The future to set when the window is initialised and shown.
     */
    std::future<void> show();

protected:
    /**
     * @brief Initialise the window
     *
     * When initialising the window, other widgets that are part of the
     * winodw can be created and initialised as well.
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void initialise();
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_WINDOW_H_ */

