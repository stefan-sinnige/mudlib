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

#ifndef _MUD_UI_X11_APPLICATION_H_
#define _MUD_UI_X11_APPLICATION_H_

#include <X11/Xlib.h>
#include <mud/core/handle.h>
#include <mud/event/event_mechanism.h>
#include <mud/ui/application.h>
#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

namespace x11 {

    class application : public mud::ui::application
    {
    public:
        /**
         * @brief Constructor.
         */
        application();

        /**
         * @brief Destructor
         */
        ~application();

        /**
         * Return the global instance of the X11 application.
         */
        static application& instance();

        /**
         * Return the X11 display
         */
        const std::shared_ptr<Display>& display() const;

        /**
         * @brief Initialise the application
         *
         * This function will be invoked on the implementation dependent UI
         * thread.
         */
        virtual void initialise() override;

        /**
         * @brief Finalise the application
         *
         * This function will be invoked on the implementation dependent UI
         * thread.
         */
        virtual void finalise() override;

    private:
        /* X11 variables */
        std::shared_ptr<Display> _display;
    };

} // namespace x11

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_APPLICATION_H_ */
