#include "mud/io/ip.h"
#include "mud/io/exception.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
        throw new mud::io::exception("converting IP address to IPv4");
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
    addr.s_addr = htonl(m_address);
    std::string str = ::inet_ntoa(addr);
    return str;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

