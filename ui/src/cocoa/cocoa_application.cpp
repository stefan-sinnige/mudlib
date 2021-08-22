#include "mud/ui/exception.h"
#include "cocoa/cocoa_application.h"

BEGIN_MUDLIB_UI_NS

cocoa::application::application()
{
}

cocoa::application::~application()
{
}

/* static */
cocoa::application&
cocoa::application::instance()
{
    return static_cast<cocoa::application&>(mud::ui::application::instance());
}

void
cocoa::application::initialise()
{
}

void
cocoa::application::finalise()
{
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

