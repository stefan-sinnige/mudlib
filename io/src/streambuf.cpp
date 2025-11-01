#if defined(_WIN32)
    // This should not be here - it is socket specific!
    #include <winSock2.h>
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#include "mud/core/exception.h"
#include "mud/io/streambuf.h"
#include <exception>
#include <streambuf>

BEGIN_MUDLIB_IO_NS

basic_streambuf::basic_streambuf(
    std::shared_ptr<mud::core::handle> handle, size_t bufsize /* = 10 */,
    size_t putbacksize /* = 4 */)
  : _handle(handle), _bufsize(bufsize), _putbacksize(putbacksize)
{
    /* Allocate the data structure */
    _buffer = new char[_bufsize + _putbacksize];

    /* Set the pointers for reading */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize);

    /* Set the pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
}

basic_streambuf::~basic_streambuf()
{
    delete[] _buffer;
}

int
basic_streambuf::underflow()
{
    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Bail out of handle is not available */
    if (_handle == nullptr) {
        throw mud::core::not_owner();
    }

    /* Read new characters */
    int nread = read(_buffer + _putbacksize, _bufsize - _putbacksize);
    if (nread <= 0) {
        return traits_type::eof();
    }

    /* Reset the buffer pointers with the number of characters read. */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize + nread);

    /* Return the next character. */
    return traits_type::to_int_type(*gptr());
}

int
basic_streambuf::overflow(int c)
{
    /* Bail out of handle is not available */
    if (_handle == nullptr) {
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
basic_streambuf::sync()
{
    int num = pptr() - pbase();
    int nwrite = write(_buffer, num);
    if (nwrite == -1) {
        return -1;
    }
    pbump(-nwrite);
    return 0;
}

std::shared_ptr<mud::core::handle>
basic_streambuf::handle()
{
    return _handle;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
