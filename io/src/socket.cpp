/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "win32/socket.cpp"
#else
  #include "posix/socket.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
