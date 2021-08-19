#include <mud/core/handle.h>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Type conversion to the internal type representation
 */

template<>
int
internal_handle<int>
(const std::unique_ptr<handle>& handle)
{
    if ((handle->type() != handle::type_t::SELECT) &&
            (handle->type() != handle::type_t::__TEST)) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    select_handle* h = static_cast<select_handle*>(handle.get());
    return *h;
}

#if defined(WINDOWS) && defined(NATIVE)
template<>
HANDLE
internal_handle<HANDLE>
(const std::unique_ptr<handle>& handle)
{
    if (handle->type() != handle::type_t::W32HANDLE) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    windows_handle* h = static_cast<windows_handle*>(handle.get());
    return *h;
}

template<>
HWND
internal_handle<HWND>
(const std::unique_ptr<handle>& handle)
{
    if (handle->type() != handle::type_t::W32WND) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    win32_handle* h = static_cast<win32_handle*>(handle.get());
    return *h;
}
#endif

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

