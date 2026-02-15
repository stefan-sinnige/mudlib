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

#ifndef _MUD_UI_COCOA_PUSHBUTTON_H_
#define _MUD_UI_COCOA_PUSHBUTTON_H_

#include "x11/x11_control.h"
#include <X11/Xlib.h>
#include <mud/core/handle.h>
#include <mud/event/event_mechanism.h>
#include <mud/ui/ns.h>
#include <mud/ui/pushbutton.h>

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
