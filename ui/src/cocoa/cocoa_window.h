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

#ifndef _MUD_UI_COCOA_WINDOW_H_
#define _MUD_UI_COCOA_WINDOW_H_

#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/window.h>
#include <mud/event/event_mechanism.h>
#include "cocoa/cocoa_control.h"
#include <Cocoa/Cocoa.h>

@interface Window : NSWindow
@end

@interface View : NSView
- (BOOL) isFlipped;
@end

BEGIN_MUDLIB_UI_NS

namespace cocoa {

class window
{
public:
    /**
     * @brief Return a reference of the window associated to the native
     * window.
     * @param wnd [in] The native window
     */
    static std::reference_wrapper<mud::ui::window> find(NSWindow* wnd);
};

} // namespace cocoa

class window::impl : public cocoa::control
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

    /**
     * @brief Return the control that occupies the specified position.
     * @param pos [in] The position within the window.
     *
     * If the position is not occupied by a control, retrun the window itself.
     */
    mud::ui::control& control(const position& pos) const;

    /** Return the implementation dependent window details. */
    static const std::unique_ptr<window::impl, window::impl_deleter>& get(
            window&);

    /**
     * Return the NSWindow native object.
     */
    Window*
    NativeWindow() const {
        return _wnd;
    }

    /**
     * Return the NSView native object.
     */
    View*
    NativeView() const {
        return _vw;
    }

private:
    /** Reference to the window */
    window& _window;

    /** Reference to the NSWindow */
    Window* _wnd;

    /** Reference to the NSView */
    View* _vw;

    /** The controls of this window */
    std::vector<std::reference_wrapper<mud::ui::control>> _controls;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_WINDOW_H_ */

