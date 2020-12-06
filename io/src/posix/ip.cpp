#include "mud/io/ip.h"
#include "mud/io/exception.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <system_error>

BEGIN_MUDLIB_IO_NS

/* ==========================================================================
 * mud::ip::address
 * ========================================================================== */

ip::address::address()
    : m_address(INADDR_ANY)
{
}

ip::address::address(in_addr_t nr)
    : m_address(nr)
{
}

ip::address::address(const std::string& str)
    : m_address(INADDR_NONE)
{
    m_address = ::inet_addr(str.c_str());
    if (m_address == INADDR_NONE) {
        throw exception("converting IP address to IPv4");
    }
}

ip::address::address(const address& rhs)
    : m_address(rhs.m_address)
{
}

ip::address&
ip::address::operator=(const address& rhs)
{
    if (this != &rhs)
    {
        m_address = rhs.m_address;
    }
    return *this;
}

ip::address::~address()
{
}

ip::address::operator in_addr_t() const
{
    return m_address;
}

std::string
ip::address::str() const
{
    struct in_addr addr;
    addr.s_addr = m_address;
    std::string str = ::inet_ntoa(addr);
    return str;
}

/* ==========================================================================
 * mud::ip::socket
 * ========================================================================== */

ip::socket::socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol)
    : basic_socket(domain, type, protocol)
{
}

ip::socket::socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol,
        std::unique_ptr<kernel_handle> handle)
    : basic_socket(domain, type, protocol, std::move(handle))
{
}

ip::socket::socket(socket&& rhs)
    : basic_socket(std::move(rhs))
{
}

ip::socket::~socket()
{
}

/* ==========================================================================
 * mud::ip::reuse_address
 * ========================================================================== */

void
ip::reuse_address::operator()(ip::socket& socket, bool value)
{
    int enable = value;
    if (::setsockopt(*(socket.handle()), SOL_SOCKET, SO_REUSEADDR, &enable,
                    sizeof(int)) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "setting socket option");
    }
}

bool
ip::reuse_address::operator()(ip::socket& socket)
{
    int enable = false;
    socklen_t len = sizeof(int);
    if (::getsockopt(*(socket.handle()), SOL_SOCKET, SO_REUSEADDR, &enable,
                    &len) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "retrieving socket option");
    }
    return (enable != 0);
}

/* ==========================================================================
 * mud::ip::nonblocking
 * ========================================================================== */

void
ip::nonblocking::operator()(ip::socket& socket, bool value)
{
    int flags;
    if ((flags = ::fcntl(*(socket.handle()), F_GETFL, 0)) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "setting socket option");
    }
    if (value)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }
    if (::fcntl(*(socket.handle()), flags) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "setting socket option");
    }
}

bool
ip::nonblocking::operator()(ip::socket& socket)
{
    int flags;
    if ((flags = ::fcntl(*(socket.handle()), F_GETFL, 0)) < 0)
    {
        throw std::system_error(errno, std::system_category(),
                "retrieving socket option");
    }
    return (flags & O_NONBLOCK) == O_NONBLOCK;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

