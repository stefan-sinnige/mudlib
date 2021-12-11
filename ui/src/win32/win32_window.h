#ifndef _MUD_UI_X11_WINDOW_H_
#define _MUD_UI_X11_WINDOW_H_

#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/window.h>
#include <mud/event/event_mechanism.h>
#include "win32/win32_control.h"
#include <windows.h>

BEGIN_MUDLIB_UI_NS

class window::impl : public win32::control
{
public:
    /**
     * @brief Constructor.
     */
    impl(window&);

    /**
     * @brief Destructor
     */
    ~impl();

    /**
     * @brief Initialise the window
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    void initialise();

    /**
     * @brief Add a control to the window.
     * @param ctrl [in] The control to add
     *
     * Add a control to the window to be displayed.
     */
    void add_control(mud::ui::control& ctrl);

    /** Return the implementation dependent window details. */
    static const std::unique_ptr<window::impl, window::impl_deleter>& get(
            window&);

private:
    /** Reference to the window */
    window& _window;

    /** The controls of this window */
    std::vector<std::reference_wrapper<mud::ui::control>> _controls;
};


END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_WINDOW_H_ */

