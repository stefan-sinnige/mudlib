#ifndef _MUD_UI_COCOA_PUSHBUTTON_H_
#define _MUD_UI_COCOA_PUSHBUTTON_H_

#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/pushbutton.h>
#include <mud/event/event_mechanism.h>
#include "x11/x11_control.h"
#include <X11/Xlib.h>

BEGIN_MUDLIB_UI_NS

class pushbutton::impl : public x11::control
{
public:
    /**
     * @brief Constructor.
     */
    impl(pushbutton& pb, window& parent);

    /**
     * @brief Destructor
     */
    ~impl();

    /**
     * @brief Initialise the pushbutton
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    void initialise();

    /**
     * @brief Expose the opushbutton
     *
     * The push-button is exposed, ie. must be (re-)drawn.
     */
    void expose();

private:
    /** Create a rectangle. */
    void draw_rectangle(Display* dpy, int scr, Drawable window);

    /** Create the text. */
    void draw_text(Display* dpy, int scr, Drawable window);

    /** Reference to the pushbutton */
    pushbutton& _pushbutton;

    /** Reference to the parent window containing the push-button */
    window& _parent;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_PUSHBUTTON_H_ */

