/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  // Not implemented
#else
  #include "posix/pipe.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
