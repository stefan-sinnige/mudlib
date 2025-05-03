/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "win32/interface.cpp"
#else
  #include "posix/interface.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
