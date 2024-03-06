/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_mechanism.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_mechanism.cpp"
#else
  #include "src/x11/x11_mechanism.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
