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
#include <fcntl.h>
#include <system_error>
#include <unistd.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Implementation of a @signal signalling resource.
 */
template<>
// class handle::signal<handle::type_t::SELECT>::impl
class select_handle::signal::impl
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
    /** The read handle */
    std::shared_ptr<mud::core::handle> _read_handle;

    /** The write handle */
    std::shared_ptr<mud::core::handle> _write_handle;
};

template<>
void
select_handle::signal::impl_deleter::operator()(signal::impl* ptr) const
{
    delete ptr;
}

select_handle::signal::impl::impl()
{
    /* Create the pipe */
    int pfd[2];
    if (::pipe(pfd) == -1) {
        throw std::system_error(errno, std::system_category(), "creating pipe");
    }

    /* Ensure non-blocking I/O */
    if (::fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1) {
        throw std::system_error(errno, std::system_category(),
                                "set pipe non-blocking");
    }

    /* Set the ownership of the pipe handles */
    _read_handle = std::shared_ptr<mud::core::handle>(
        new mud::core::select_handle(int(pfd[0])));
    _write_handle = std::shared_ptr<mud::core::handle>(
        new mud::core::select_handle(int(pfd[1])));

    /* Logging */
    LOG(log);
    DEBUG(log) << "Select read pipe fd: " << pfd[0] << std::endl;
    DEBUG(log) << "Select write pipe fd: " << pfd[1] << std::endl;
}

select_handle::signal::impl::~impl()
{
    if (_read_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_read_handle));
        _read_handle.reset();
    }
    if (_write_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_write_handle));
        _write_handle.reset();
    }
}

std::shared_ptr<mud::core::handle>
select_handle::signal::impl::handle() const
{
    return _read_handle;
}

void
select_handle::signal::impl::trigger()
{
    LOG(log);
    TRACE(log) << "Select pipe trigger fd: "
               << mud::core::internal_handle<int>(_write_handle) << std::endl;

    char ch = 'N';
    (void)::write(mud::core::internal_handle<int>(_write_handle), &ch, 1);
}

bool
select_handle::signal::impl::capture()
{
    LOG(log);
    TRACE(log) << "Select pipe capture fd: "
               << mud::core::internal_handle<int>(_read_handle) << std::endl;

    char ch;
    int n = ::read(mud::core::internal_handle<int>(_read_handle), &ch, 1);
    return (n == 1);
}

/** The explicit implementation for self signalling resources. */

template<>
select_handle::signal::signal()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

template<>
select_handle::signal::~signal()
{}

template<>
std::shared_ptr<mud::core::handle>
select_handle::signal::handle() const
{
    return _impl->handle();
}

template<>
void
select_handle::signal::trigger()
{
    _impl->trigger();
}

template<>
bool
select_handle::signal::capture()
{
    return _impl->capture();
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
