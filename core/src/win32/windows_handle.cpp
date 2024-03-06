#include "mud/core/handle.h"
#include <windows.h>

BEGIN_MUDLIB_CORE_NS

template<>
HANDLE
internal_handle<HANDLE>(const std::unique_ptr<handle>& handle)
{
    if (handle->type() != handle::type_t::W32HANDLE) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    windows_handle* h = static_cast<windows_handle*>(handle.get());
    return *h;
}

template<>
HWND
internal_handle<HWND>(const std::unique_ptr<handle>& handle)
{
    if (handle->type() != handle::type_t::W32WND) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    win32_handle* h = static_cast<win32_handle*>(handle.get());
    return *h;
}

/**
 * Implementation of a signalling resource.
 */
template<>
class windows_handle::signal::impl
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The handle.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * Send a signal to the resource.
     */
    void trigger();

    /**
     * Receive a signal.
     */
    bool capture();

private:
    /** The event handle */
    std::unique_ptr<mud::core::handle> _handle;
};

template<>
void
windows_handle::signal::impl_deleter::operator()(signal::impl* ptr) const
{
    delete ptr;
}

windows_handle::signal::impl::impl()
{
    // Create a manual reset event.
    HANDLE handle = ::CreateEvent(nullptr, true, false, nullptr);

    // Dave the handle
    _handle = std::unique_ptr<mud::core::handle>(
        new mud::core::windows_handle(handle));
}

windows_handle::signal::impl::~impl()
{
    if (_handle != nullptr) {
        ::CloseHandle(mud::core::internal_handle<HANDLE>(_handle));
        _handle.reset(nullptr);
    }
}

const std::unique_ptr<mud::core::handle>&
windows_handle::signal::impl::handle() const
{
    return _handle;
}

void
windows_handle::signal::impl::trigger()
{
    ::SetEvent(mud::core::internal_handle<HANDLE>(_handle));
}

bool
windows_handle::signal::impl::capture()
{
    ::ResetEvent(mud::core::internal_handle<HANDLE>(_handle));
    // Cannot determine the current state
    return true;
}

/** The explicit implementation for self signalling resources. */

template<>
windows_handle::signal::signal()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

template<>
windows_handle::signal::~signal()
{}

template<>
const std::unique_ptr<mud::core::handle>&
windows_handle::signal::handle() const
{
    return _impl->handle();
}

template<>
void
windows_handle::signal::trigger()
{
    _impl->trigger();
}

template<>
bool
windows_handle::signal::capture()
{
    return _impl->capture();
}

END_MUDLIB_CORE_NS
