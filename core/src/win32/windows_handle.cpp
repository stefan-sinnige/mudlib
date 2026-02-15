/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/handle.h"
#include <windows.h>

BEGIN_MUDLIB_CORE_NS

template<>
HANDLE
internal_handle<HANDLE>(std::shared_ptr<handle> handle)
{
    if (handle->type() != handle::type_t::W32HANDLE) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    windows_handle* h = static_cast<windows_handle*>(handle.get());
    return *h;
}

template<>
HWND
internal_handle<HWND>(std::shared_ptr<handle> handle)
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
    std::shared_ptr<mud::core::handle> handle() const;

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
    std::shared_ptr<mud::core::handle> _handle;
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

    // Save the handle
    _handle = std::shared_ptr<mud::core::handle>(
        new mud::core::windows_handle(handle));

    // Logging
    LOG(log);
    TRACE(log) << "Select event " << HANDLE << std::endl;
}

windows_handle::signal::impl::~impl()
{
    if (_handle != nullptr) {
        ::CloseHandle(mud::core::internal_handle<HANDLE>(_handle));
        _handle.reset(nullptr);
    }
}

std::shared_ptr<mud::core::handle>
windows_handle::signal::impl::handle() const
{
    return _handle;
}

void
windows_handle::signal::impl::trigger()
{
    LOG(log);
    TRACE(log) << "Select event trigger fd: "
               << mud::core::internal_handle<HANDLE>(_handle) << std::endl;

    ::SetEvent(mud::core::internal_handle<HANDLE>(_handle));
}

bool
windows_handle::signal::impl::capture()
{
    LOG(log);
    TRACE(log) << "Select event capture fd: "
               << mud::core::internal_handle<HANDLE>(_handle) << std::endl;

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
std::shared_ptr<mud::core::handle>
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
