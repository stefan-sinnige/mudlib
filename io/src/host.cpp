/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "win32/host.cpp"
#else
  #include "posix/host.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
