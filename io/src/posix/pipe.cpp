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

#include "mud/core/hexdump.h"
#include "mud/io/pipe.h"
#include "mud/core/exception.h"
#include "mud/io/streambuf.h"
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <system_error>
#include <unistd.h>

using namespace std::placeholders;

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with pipes. The read and write operations
 * to be used are the POSIX 'read' and 'write'.
 */

namespace _pipe {

    class streambuf : public mud::io::basic_streambuf
    {
    public:
        /* Constructor for UDP specific stream-buffer.
         * @param [in] handle  The handle to use.
         * @param [in] bufsize The initial buffer size.
         * @param [in] putbacksize The size of the putback buffer.
         */
        streambuf(const std::shared_ptr<mud::core::handle>& handle,
                  size_t bufsize = 10, size_t putbacksize = 4);

        /* Pipe specific read and write functions. */
        ssize_t read(void* buffer, size_t count) override;
        ssize_t write(const void* buffer, size_t count) override;
    };

    streambuf::streambuf(const std::shared_ptr<mud::core::handle>& handle,
                         size_t bufsize, size_t putbacksize)
      : mud::io::basic_streambuf(handle, bufsize, putbacksize)
    {}

    ssize_t streambuf::read(void* buf, size_t count)
    {
        LOG(log);
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nread;
        int tries = 10;
        while ((nread = ::read(hndl, buf, count)) < 0
               && tries-- > 0)
        {
            if (errno == EINTR) {
                /* Receive was interrupted, try again */
                continue;
            }
            else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Not ready to read, wait a bit (10*1000 µsec maximum). This
                 * has the side-effect that this read will be blocked at most
                 * by this time interval. */
                fd_set rdfs;
                FD_ZERO(&rdfs);
                FD_SET(hndl, &rdfs);
                struct timeval  tv;
                tv.tv_sec = 0;
                tv.tv_usec = 1000;
                if (::select(hndl+1, &rdfs, nullptr, nullptr, &tv) <= 0) {
                    /* Still nothing available, or low-level errors */
                    TRACE(log) << "No data available on pipe " << hndl
                               << ": " << strerror(errno) << " (retry)"
                               << std::endl;
                    continue;
                }
                if (! FD_ISSET(hndl, &rdfs)) {
                    ERROR(log) << "No data available on pipe " << hndl
                               << std::endl;
                    return -1;
                }
                continue;
            }
            else {
                /* Anything else is an error */
                ERROR(log) << "Error on pipe " << hndl << strerror(errno)
                               << std::endl;
                return -1;
            }
        }

        /* We may have received less than asked for, for example, the peer is
         * still sending more packets. We may need subsequent calls to get more
         * data if needed, dictated by more calls to `underflow` or related. */
        TRACE(log) << "Received " << nread << " bytes on pipe " << hndl
                   << std::endl;
std::cerr << "Received on pipe: " << std::endl
          << mud::core::hexdump((const char*)buf, nread) << std::endl;
        return nread;
    }

    ssize_t streambuf::write(const void* buf, size_t count)
    {
        LOG(log);
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nwritten = 0;
        int tries = 10;
        while ((nwritten = ::write(hndl, buf, count)) < 0
               && tries -- > 0)
        {
            if (errno == EINTR) {
                /* Send was interrupted, try again */
                continue;
            }
            else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Not ready to write, wait a bit (10*1000 µsec maximum). This
                 * has the side-effect that this write will be blocked at most
                 * by this time interval. */
                fd_set wrfs;
                FD_ZERO(&wrfs);
                FD_SET(hndl, &wrfs);
                struct timeval  tv;
                tv.tv_sec = 0;
                tv.tv_usec = 1000;
                if (::select(hndl+1, nullptr, &wrfs, nullptr, &tv) <= 0) {
                    /* Still not ready to write, or low-level errors */
                    ERROR(log) << "Pipe " << hndl << " not ready to write"
                               << std::endl;
                    return -1;
                }
                if (! FD_ISSET(hndl, &wrfs)) {
                    ERROR(log) << "Pipe " << hndl << " not ready to write"
                               << std::endl;
                    return -1;
                }
                continue;
            }
            else if (errno == EMSGSIZE) {
                /* The block is too large, halve it and try again */
                if (count <= 1) {
                    /* Smallest block possible was not small enough */
                    return -1;
                }
                count >>= 1;
                continue;
            }
            else {
                /* Anything else is an error. */
                ERROR(log) << "Error on pipe " << hndl << strerror(errno)
                               << std::endl;
                return -1;
            }
        }
        /* We may have written less than asked for, for example, the output
         * buffer is full. We may need subsequent calls to flush more data if
         * needed dictated by more calls to `underflow` or related. */
        TRACE(log) << "Sent " << nwritten << " bytes on pipe " << hndl
                   << std::endl;
//std::cerr << "Written on pipe: " << std::endl
//          << mud::core::hexdump((const char*)buf, nwritten) << std::endl;
        return nwritten;
    }

} // namespace _pipe

/**
 * @brief Implementation class for a POSIX compliant @c pipe.
 */

class pipe::impl
{
public:
    /**
     * Constructor.
     */
    impl();

    /**
     * Move constructor.
     */
    impl(impl&&);

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The stream for reading.
     */
    std::istream& istr();

    /**
     * The stream for writing.
     */
    std::ostream& ostr();

    /**
     * Read a character.
     */
    char read();

    /**
     * The read handle.
     */
    std::shared_ptr<mud::core::handle> read_handle() const;

    /**
     * The write handle.
     */
    std::shared_ptr<mud::core::handle> write_handle() const;

private:
    /** The stream for reading. */
    std::istream _istr;

    /** The stream for writing. */
    std::ostream _ostr;

    /** The stream buffer for reading */
    std::unique_ptr<_pipe::streambuf> _read_buffer;

    /** The stream buffer for writing */
    std::unique_ptr<_pipe::streambuf> _write_buffer;

    /** The read handle */
    std::shared_ptr<mud::core::handle> _read_handle;

    /** The write handle */
    std::shared_ptr<mud::core::handle> _write_handle;
};

pipe::impl::impl() : _istr(nullptr), _ostr(nullptr)
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
    _read_handle = std::make_shared<mud::core::select_handle>(int(pfd[0]));
    _write_handle = std::make_shared<mud::core::select_handle>(int(pfd[1]));

    /* Logging */
    LOG(log);
    INFO(log) << "Creating read pipe fd: " << pfd[0] << std::endl;
    INFO(log) << "Creating write pipe fd: " << pfd[1] << std::endl;

    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer =
        std::unique_ptr<_pipe::streambuf>(new _pipe::streambuf(_read_handle));
    _write_buffer =
        std::unique_ptr<_pipe::streambuf>(new _pipe::streambuf(_write_handle));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());
}

pipe::impl::~impl()
{
    if (_read_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_read_handle));
    }
    if (_write_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_write_handle));
    }
}

std::istream&
pipe::impl::istr()
{
    return _istr;
}

std::ostream&
pipe::impl::ostr()
{
    return _ostr;
}

std::shared_ptr<mud::core::handle>
pipe::impl::read_handle() const
{
    return _read_handle;
}

std::shared_ptr<mud::core::handle>
pipe::impl::write_handle() const
{
    return _write_handle;
}

void
pipe::impl_deleter::operator()(pipe::impl* ptr) const
{
    delete ptr;
}

/** The explicit specialisation for POSIX pipes. */

pipe::pipe()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

pipe::~pipe() {}

std::istream&
pipe::istr()
{
    return _impl->istr();
}

std::ostream&
pipe::ostr()
{
    return _impl->ostr();
}

std::shared_ptr<mud::core::handle>
pipe::read_handle() const
{
    return _impl->read_handle();
}

std::shared_ptr<mud::core::handle>
pipe::write_handle() const
{
    return _impl->write_handle();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
