/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  // Nothing
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_handle.cpp"
#else
  #include "src/x11/x11_handle.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
