#ifndef _MUDLIB_UI_WINDOW_H_
#define _MUDLIB_UI_WINDOW_H_

#include <mud/ui/ns.h>
#include <mud/ui/control.h>
#include <future>

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
class MUDLIB_UI_API window: public control
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

    /**
     * @brief Initialise the window
     *
     * When initialising the window, other widgets that are part of the
     * windaw can be created and initialised as well.
     *
     * This function will be invoked on the implementation dependent UI
     * thread.
     */
    virtual void initialise() override;

    /**
     * @brief Return the control that occupies the specified position.
     * @param pos [in] The position within the window.
     *
     * If the position is not occupied by a control, return the window itself.
     */
    mud::ui::control& control(const position& pos) const;

    /**
     * Not copyable.
     */
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    /**
     * Not moveable.
     */
    window(window&&) = delete;
    window& operator=(window&&) = delete;

protected:
    /**
     * Initialise the suported properties with default values.
     */
    virtual void default_properties() override;

    /**
     * @brief Dispatch a UI event.
     * @param event [in] The event details.
     *
     * The UI event that has been received is dispatched to this object for
     * further handling. Depending on the type of event, the control-specific
     * handling routine will be invoked.
     */
    virtual void dispatch(const event& event) override;

    /** Platform specific implementation.  */
    class impl;
    struct impl_deleter {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;

    /**
     * Controls that can access the native implementation of the window.
     */
    friend class pushbutton;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_WINDOW_H_ */

