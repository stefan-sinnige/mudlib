#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define SETSOCKOPT_CAST (const char*)
    #define GETSOCKOPT_CAST (char*)
#else
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #define SETSOCKOPT_CAST (const void*)
    #define GETSOCKOPT_CAST (void*)
#endif
#include "mud/io/exception.h"
#include "mud/io/ip.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <system_error>

BEGIN_MUDLIB_IO_NS

/* ==========================================================================
 * mud::ip::address
 * ========================================================================== */

ip::address::address() : m_address(INADDR_ANY) {}

ip::address::address(uint32_t nr) : m_address(nr) {}

ip::address::address(const std::string& str) : m_address(INADDR_NONE)
{
    m_address = ::inet_addr(str.c_str());
    if (m_address == INADDR_NONE) {
        throw exception("converting IP address to IPv4");
    }
}

ip::address::address(const address& rhs) : m_address(rhs.m_address) {}

ip::address&
ip::address::operator=(const address& rhs)
{
    if (this != &rhs) {
        m_address = rhs.m_address;
    }
    return *this;
}

ip::address::~address() {}

ip::address::operator uint32_t() const
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

ip::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                   basic_socket::protocol_t protocol)
  : basic_socket(domain, type, protocol)
{}

ip::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                   basic_socket::protocol_t protocol,
                   std::unique_ptr<mud::core::handle> handle)
  : basic_socket(domain, type, protocol, std::move(handle))
{}

ip::socket::socket(socket&& rhs) : basic_socket(std::move(rhs))
{
    std::swap(_source_address, rhs._source_address);
    std::swap(_destination_address, rhs._destination_address);
}

ip::socket&
ip::socket::operator=(socket&& rhs)
{
    if (this != &rhs) {
        basic_socket::operator=(std::move(rhs));
        std::swap(_source_address, rhs._source_address);
        std::swap(_destination_address, rhs._destination_address);
    }
    return *this;
}

ip::socket::~socket() {}

const ip::address&
ip::socket::source_address() const
{
    return _source_address;
}

const ip::address&
ip::socket::destination_address() const
{
    return _destination_address;
}

void
ip::socket::source_address(const address& peer)
{
    _source_address = peer;
}

void
ip::socket::destination_address(const address& peer)
{
    _destination_address = peer;
}

/* ==========================================================================
 * mud::ip::reuse_address
 * ========================================================================== */

void
ip::reuse_address::operator()(ip::socket& socket, bool value)
{
    int enable = value;
    int handle = mud::core::internal_handle<int>(socket.handle());
    if (::setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, SETSOCKOPT_CAST & enable,
                     sizeof(int)) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "setting socket option");
    }
}

bool
ip::reuse_address::operator()(ip::socket& socket)
{
    int enable = false;
    socklen_t len = sizeof(int);
    int handle = mud::core::internal_handle<int>(socket.handle());
    if (::getsockopt(handle, SOL_SOCKET, SO_REUSEADDR, GETSOCKOPT_CAST & enable,
                     &len) < 0) {
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
    int handle = mud::core::internal_handle<int>(socket.handle());
#if defined(_WIN32)
    u_long mode = value;
    if (::ioctlsocket(handle, FIONBIO, &mode) != 0) {
        throw std::system_error(::WSAGetLastError(), std::system_category(),
                                "setting socket option (non-blocking)");
    }
#else
    int flags;
    if ((flags = ::fcntl(handle, F_GETFL, 0)) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "setting socket option (non-blocking)");
    }
    if (value) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    if (::fcntl(handle, F_SETFL, flags) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "setting socket option (non-blocking)");
    }
#endif
}

bool
ip::nonblocking::operator()(ip::socket& socket)
{
#if defined(_WIN32)
    throw std::system_error(
        -1, std::system_category(),
        "retrieving socket option (noon-blocking) not supported");
#else
    int flags;
    int handle = mud::core::internal_handle<int>(socket.handle());
    if ((flags = ::fcntl(handle, F_GETFL, 0)) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "retrieving socket option (non-blocking)");
    }
    return (flags & O_NONBLOCK) == O_NONBLOCK;
#endif
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
