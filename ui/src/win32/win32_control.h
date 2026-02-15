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

#ifndef _MUD_UI_X11_CONTROL_H_
#define _MUD_UI_X11_CONTROL_H_

#include <functional>
#include <mud/ui/control.h>
#include <mud/ui/ns.h>
#include <windows.h>

BEGIN_MUDLIB_UI_NS

namespace win32 {

    class control
    {
    public:
        /**
         * @brief Constructor.
         * @param ctrl [in] The associated UI control
         */
        control(mud::ui::control& ctrl);

        /**
         * @brief Destructor
         */
        ~control();

        /**
         * Set the native control object.
         * @param ctrl [in] The native control.
         */
        void NativeControl(HWND ctrl);

        /**
         * Return the HWND native object.
         */
        HWND NativeControl() const;

        /**
         * @brief Return a reference of the control associated to the native
         * control.
         * @param ctrl [in] The native control
         */
        static std::reference_wrapper<mud::ui::control> find(HWND ctrl);

    private:
        /** Reference to the UI Control */
        mud::ui::control& _control;

        /** Reference to the Win32 Window */
        HWND _wnd;
    };

}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_CONTROL_H_ */
