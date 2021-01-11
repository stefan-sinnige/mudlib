#include "mud/io/socket.h"
#include "mud/io/exception.h"
#include <unistd.h>

BEGIN_MUDLIB_IO_NS

basic_socket::basic_socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol)
    : _domain(domain), _type(type), _protocol(protocol)
{
    int fd = ::socket(static_cast<int>(domain),
                    static_cast<int>(type),
                    static_cast<int>(protocol));
    _handle  = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(fd));
}

basic_socket::basic_socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol,
        std::unique_ptr<kernel_handle> handle)
    : _domain(domain), _type(type), _protocol(protocol),
      _handle(std::move(handle))
{
}

basic_socket::~basic_socket()
{
    close();
}

basic_socket::basic_socket(basic_socket&& rhs)
{
    _handle = std::move(rhs._handle);
    _domain = rhs._domain;
    _type = rhs._type;
    _protocol = rhs._protocol;
}

basic_socket&
basic_socket::operator=(basic_socket&& rhs)
{
    if (this != &rhs)
    {
        _handle = std::move(rhs._handle);
        _domain = rhs._domain;
        _type = rhs._type;
        _protocol = rhs._protocol;
    }
    return *this;
}

void
basic_socket::close()
{
    if (_handle != nullptr) {
        ::close(*_handle);
        _handle.reset(nullptr);
    }
}

const std::unique_ptr<kernel_handle>&
basic_socket::handle() const
{
    return _handle;
}

basic_socket::domain_t
basic_socket::domain() const
{
    return _domain;
}

basic_socket::type_t
basic_socket::type() const
{
    return _type;
}

basic_socket::protocol_t
basic_socket::protocol() const
{
    return _protocol;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

