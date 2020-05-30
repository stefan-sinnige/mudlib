#include "mud/core/exception.h"
#include "mud/io/streambuf.h"
#include <exception>
#include <streambuf>
#include <unistd.h>

BEGIN_MUDLIB_IO_NS

/**
 * Structure to hold the stream buffer data.
 */

class posix_streambuf
{
public:
    /** Constructor */
    posix_streambuf(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            size_t bufsize,
            size_t putbacksize);

    /** Destructor */
    ~posix_streambuf();

    /** Public member variabes */
    const std::unique_ptr<mud::io::kernel_handle>& _handle; /**< Handle */
    const size_t _bufsize;     /**< Size of the buffer */
    const size_t _putbacksize; /**< Size of the putback buffer */
    char* _buffer;             /**< The buffer. */
};

posix_streambuf::posix_streambuf(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        size_t bufsize /* = 10 */,
        size_t putbacksize /* = 4 */)
    : _handle(handle), _bufsize(bufsize), _putbacksize(putbacksize)
{
    _buffer = new char[_bufsize + _putbacksize];
}

posix_streambuf::~posix_streambuf()
{
    delete[] _buffer;
}

/**
 * Implementation of the streambuf using the POSIX generic file descriptor
 * routines @c rad and @write in a buffered manner.
 */

streambuf::streambuf(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        size_t bufsize /* = 10 */,
        size_t putbacksize /* = 4 */)
    : _impl(new posix_streambuf(handle, bufsize, putbacksize))
{
    posix_streambuf* impl = static_cast<posix_streambuf*>(_impl);

    /* Allocate the data structure */
    /* Set the pointers for reading */
    setg(impl->_buffer + impl->_putbacksize,
            impl->_buffer + impl->_putbacksize,
            impl->_buffer + impl->_putbacksize);

    /* Set the pointers for writing */
    setp(impl->_buffer,
            impl->_buffer + (impl->_bufsize - 1));
}

streambuf::~streambuf()
{
    posix_streambuf* impl = static_cast<posix_streambuf*>(_impl);
    delete impl;
}

int
streambuf::underflow()
{
    posix_streambuf* impl = static_cast<posix_streambuf*>(_impl);

    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Bail out of handle is not available */
    if (impl->_handle == nullptr) {
        throw mud::core::not_owner();
    }

    /* Read new characters */
    int nread = ::read(
                    *(impl->_handle),
                    impl->_buffer + impl->_putbacksize,
                    impl->_bufsize - impl->_putbacksize);
    if (nread <= 0) {
        if (nread != 0 && errno != EAGAIN) {
            throw std::system_error(errno, std::system_category(), "read");
        }
        return traits_type::eof();
    }

    /* Reset the buffer pointers with the number of characters read. */
    setg(impl->_buffer + impl->_putbacksize,
            impl->_buffer + impl->_putbacksize,
            impl->_buffer + impl->_putbacksize + nread);

    /* Return the next character. */
    return traits_type::to_int_type(*gptr());
}

int
streambuf::overflow(int c)
{
    posix_streambuf* impl = static_cast<posix_streambuf*>(_impl);

    /* Bail out of handle is not available */
    if (impl->_handle == nullptr) {
        throw mud::core::not_owner();
    }

    /* Insert the character into the buffer */
    if (c != traits_type::eof()) {
        *pptr() = c;
        pbump(1);
    }

    /* Write the buffer contents */
    if (sync() != 0) {
        return traits_type::eof();
    }
    return c;
}

int
streambuf::sync()
{
    posix_streambuf* impl = static_cast<posix_streambuf*>(_impl);

    int num = pptr() - pbase();
    int nwrite = ::write(*(impl->_handle), impl->_buffer, num);
    if (nwrite != num) {
        return 1;
    }
    pbump(-nwrite);
    return 0;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

