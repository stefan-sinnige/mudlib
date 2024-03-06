#include "mud/ui/control.h"

BEGIN_MUDLIB_UI_NS

control::control() {}

control::~control() {}

END_MUDLIB_UI_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_control.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_control.cpp"
#else
  #include "src/x11/x11_control.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
