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

#ifndef _MUD_UI_X11_WINDOW_H_
#define _MUD_UI_X11_WINDOW_H_

#include "win32/win32_control.h"
#include <mud/core/handle.h>
#include <mud/event/event_mechanism.h>
#include <mud/ui/ns.h>
#include <mud/ui/window.h>
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
