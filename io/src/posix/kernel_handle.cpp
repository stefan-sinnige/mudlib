#include "mud/io/kernel_handle.h"
#include <errno.h>
#include <sys/select.h>
#include <mutex>
#include <system_error>

/** The explicit implementation for POSIX kernel handles. */
template class mud::core::basic_handle<int>;

template<>
mud::core::basic_handle<int>::basic_handle(int h)
    : _handle(h)
{
}

template<>
mud::core::basic_handle<int>::basic_handle(basic_handle&& h)
{
    _handle = h._handle;
}

template<>
mud::core::basic_handle<int>::~basic_handle()
{
}

template<>
mud::core::basic_handle<int>::operator int()
{
    return _handle;
}

/* vi: set ai ts=4 expandtab: */

