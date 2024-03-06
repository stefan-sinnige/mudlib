/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_event.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_event.cpp"
#else
  #include "src/x11/x11_event.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
