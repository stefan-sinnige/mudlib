#include <mud/core/handle.h>
#include <mud/core/exception.h>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Type conversion to the internal type representation
 */

template<>
int
internal_handle<int>(const std::unique_ptr<handle>& handle)
{
    if (!handle) {
        throw mud::core::not_owner();
    }
    if ((handle->type() != handle::type_t::SELECT) &&
        (handle->type() != handle::type_t::__TEST)) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    select_handle* h = static_cast<select_handle*>(handle.get());
    return *h;
}

END_MUDLIB_CORE_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "win32/windows_handle.cpp"
  #include "posix/select_socket.cpp"
#else
  // We can either use a pipe or a socket:
  #include "posix/select_pipe.cpp"
  // #include "posix/select_socket.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
