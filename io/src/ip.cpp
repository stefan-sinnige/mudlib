#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define SETSOCKOPT_CAST (const char*)
    #define GETSOCKOPT_CAST (char*)
#else
    #include <arpa/inet.h>
    #include <netdb.h>
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
#include <string.h>
#include <sys/types.h>
#include <system_error>

BEGIN_MUDLIB_IO_NS

/**
 * Converstion structures defined in the socket.cpp
 */
extern int g_domains[];
extern int g_types[];
extern int g_protocols[];

/* ==========================================================================
 * mud::ip::address
 * ========================================================================== */

ip::address::address() : m_address(INADDR_ANY) {}

ip::address::address(uint32_t nr) : m_address(nr) {}

ip::address::address(const std::string& node, const hints& criteria)
    : m_address(INADDR_NONE)
{
    // Verify if the node is in a dotted decimal format
    bool dotted = true;
    for (const char& ch : node)
    {
        if ((ch < '0' || ch > '9') && ch != '.') {
            dotted = false;
            break;
        }
    }

    // If dotted decimal format, convert it as-is, otherwise do a look-up.
    if (dotted) {
        m_address = ::inet_addr(node.c_str());
    }
    else {
        struct addrinfo* infos;
        struct addrinfo hint;

        // Define any hints
        memset(&hint, 0, sizeof(struct addrinfo));
        if (criteria.domain != basic_socket::domain_t::UNSPEC)
            hint.ai_family = g_domains[static_cast<int>(criteria.domain)];
        if (criteria.type != basic_socket::type_t::UNSPEC)
            hint.ai_socktype = g_types[static_cast<int>(criteria.type)];
        if (criteria.protocol != basic_socket::protocol_t::UNSPEC)
            hint.ai_protocol = g_protocols[static_cast<int>(criteria.protocol)];

        // Perform a lookup (this is likely to be expensive)
        int result = getaddrinfo(node.c_str(), nullptr, &hint, &infos);
        if (result != 0) {
            throw exception("looking up IP address");
        }
        struct addrinfo* info;
        for (info = infos; info != nullptr; info = info->ai_next)
        {
            // Use the first match
            m_address = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr;
            break;
        }
        freeaddrinfo(infos);
    }

    // Raise an exception on an error.
    if (m_address == INADDR_NONE) {
        throw exception("converting IP address to IPv4");
    }
}

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
                   std::shared_ptr<mud::core::handle> handle)
  : basic_socket(domain, type, protocol, handle)
{}

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
