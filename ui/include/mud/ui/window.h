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

#ifndef _MUDLIB_UI_WINDOW_H_
#define _MUDLIB_UI_WINDOW_H_

#include <future>
#include <mud/ui/control.h>
#include <mud/ui/ns.h>

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
class MUDLIB_UI_API window : public control
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
    struct impl_deleter
    {
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
