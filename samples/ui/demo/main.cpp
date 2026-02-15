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

#include <iostream>
#include <mud/ui/application.h>
#include <mud/ui/pushbutton.h>
#include <mud/ui/window.h>

/* ======================================================================
 * Main Window
 * ====================================================================== */

class main_window : public mud::ui::window
{
public:
    main_window();

    void initialise() override;

private:
    mud::ui::pushbutton _pushbutton;
};

main_window::main_window() : _pushbutton(*this) {}

void
main_window::initialise()
{
    // Initialise the window

    // Initialise the push-button
    _pushbutton.property<mud::ui::position>(10, 10)
        .property<mud::ui::size>(80, 25)
        .property<mud::ui::text>("Press Me");
    _pushbutton.event([](const mud::ui::event::mouse& ev) {
        std::cout << "Mouse event" << std::endl;
    });

    // Calling the base method will start to draw the  window.
    mud::ui::window::initialise();
}

/* ======================================================================
 * Main
 * ====================================================================== */

int
main(int argc, char** argv)
{
    // Create the top-level window
    main_window toplevel;
    toplevel.show();

    // Start the application
    mud::ui::application::instance().loop();
}
