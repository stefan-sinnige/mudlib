/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/task.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/task.cpp"
#else
  #include "src/x11/task.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
