#include "mud/ui/exception.h"
#include "win32/win32_application.h"

BEGIN_MUDLIB_UI_NS

win32::application::application()
{
}

win32::application::~application()
{
}

/* static */
win32::application&
win32::application::instance()
{
    return static_cast<win32::application&>(mud::ui::application::instance());
}

void
win32::application::initialise()
{
}

void
win32::application::finalise()
{
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

