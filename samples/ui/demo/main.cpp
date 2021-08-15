#include <mud/ui/application.h>
#include <mud/ui/window.h>

/* ======================================================================
 * Main
 * ====================================================================== */

int
main(int argc, char** argv)
{
    // Create a top-level window
    mud::ui::window toplevel;
    toplevel.show();

    // Start the application
    mud::ui::application::instance().loop();
}

