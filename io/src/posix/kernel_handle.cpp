#include "mud/io/kernel_handle.h"
#include <errno.h>
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
mud::core::basic_handle<int>::basic_handle(basic_handle&& rhs)
{
    _handle = rhs._handle;
    rhs._handle = -1;
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

template<>
bool
mud::core::basic_handle<int>::valid()
{
    return (_handle >= 0);
}

/* vi: set ai ts=4 expandtab: */

