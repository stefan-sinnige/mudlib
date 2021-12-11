#ifndef _MUDLIB_UI_PUSHBUTTON_H_
#define _MUDLIB_UI_PUSHBUTTON_H_

#include <mud/ui/ns.h>
#include <mud/ui/control.h>
#include <mud/ui/event.h>
#include <mud/ui/window.h>

BEGIN_MUDLIB_UI_NS

/**
 * @brief A pushbutton is a region on the window that can be activated by
 * a mouse-click or in-focus keyboard event to perform a particular action.
 */
class MUDLIB_UI_API pushbutton: public control
{
public:
    /**
     * Type definition for event handlers.
     */
    typedef std::function<void(const mud::ui::event::mouse&)> mouse_event_func;

    /**
     * @brief Construct a pushbutton.
     *
     * @param parent [in] The window containing the push-button.
     */
    pushbutton(window& parent);

    /**
     * @brief Remove the pushbutton and its allocated resources.
     */
    virtual ~pushbutton();

    /**
     * Not copyable.
     */
    pushbutton(const pushbutton&) = delete;
    pushbutton& operator=(const pushbutton&) = delete;

    /**
     * Not moveable.
     */
    pushbutton(pushbutton&&) = delete;
    pushbutton& operator=(pushbutton&&) = delete;

    /**
     * Define an event handling routine for events from a mouse device.
     * @param handler [in] The event handler.
     */
    virtual void event(mouse_event_func handler);

protected:
    /**
     * Initialise the suported properties with default values.
     */
    virtual void default_properties() override;

    /**
     * @brief Initialise the pushbutton
     *
     * When initialising the pushbutton, other widgets that are part of the
     * winodw can be created and initialised as well.
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void initialise() override;

    /**
     * @brief Dispatch a UI event.
     * @param event [in] The event details.
     *
     * The UI event that has been received is dispatched to this object for
     * further handling. Depending on the type of event, the control-specific
     * handling routine will be invoked.
     */
    virtual void dispatch(const mud::ui::event& event) override;

private:
    /** The mouse event handler. */
    mouse_event_func _mouse_event_fn;

    /** Platform specific implementation.  */
    class impl;
    struct impl_deleter {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_PUSHBUTTON_H_ */

