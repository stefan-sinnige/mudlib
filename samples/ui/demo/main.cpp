#include <mud/ui/application.h>
#include <mud/ui/pushbutton.h>
#include <mud/ui/window.h>
#include <iostream>

/* ======================================================================
 * Main Window
 * ====================================================================== */

class main_window: public mud::ui::window
{
public:
    main_window();

    void initialise() override;

private:
    mud::ui::pushbutton _pushbutton;
};

main_window::main_window()
    : _pushbutton(*this)
{
}

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

