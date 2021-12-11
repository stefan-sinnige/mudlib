#include "mud/io/socket.h"
#include "mud/io/exception.h"
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

BEGIN_MUDLIB_IO_NS

/* Lookup tables for socket enumerations. The order matches the socket.h
 * enumeration. A value of -1 means 'not supported'. */

int g_domains[] = { AF_UNIX,      AF_LOCAL,     AF_INET,   AF_INET6,
#ifdef LINUX
                    AF_AX25,
#else
    -1,  /**< AX25 */
#endif
                    AF_IPX,       AF_APPLETALK,
#ifdef LINUX
                    AF_X25,
#else
    -1,  /**< X25 */
#endif
                    AF_DECnet,
#ifdef LINUX
                    AF_KEY,       AF_NETLINK,   AF_PACKET, AF_RDS,   AF_PPPOX,
                    AF_LLC,       AF_IB,        AF_MPLS,   AF_CAN,   AF_TIPC,
                    AF_BLUETOOTH, AF_ALG,       AF_VSOCK,  AF_KCM,   AF_XDP
#else
    -1,  /** KEY */
    -1,  /** NETLINK */
    -1,  /** PACKET */
    -1,  /** RDS */
    -1,  /** PPPOX */
    -1,  /** LLC */
    -1,  /** IB */
    -1,  /** MPLS */
    -1,  /** CAN */
    -1,  /** TIPC */
    -1,  /** BLUETOOTH */
    -1,  /** ALG */
    -1,  /** VSOCK */
    -1,  /** KCM */
    -1   /** XDP*/
#endif
};

int g_types[] = { SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, SOCK_RDM };

int g_protocols[]{
    IPPROTO_IP, /** INTRINSIC */
    IPPROTO_IP,       IPPROTO_HOPOPTS, IPPROTO_ICMP,     IPPROTO_IGMP,
    IPPROTO_GGP,      IPPROTO_IPV4,    IPPROTO_IPIP,     IPPROTO_TCP,
    IPPROTO_ST,       IPPROTO_EGP,     IPPROTO_PIGP,     IPPROTO_RCCMON,
    IPPROTO_NVPII,    IPPROTO_PUP,     IPPROTO_ARGUS,    IPPROTO_EMCON,
    IPPROTO_XNET,     IPPROTO_CHAOS,   IPPROTO_UDP,      IPPROTO_MUX,
    IPPROTO_MEAS,     IPPROTO_HMP,     IPPROTO_PRM,      IPPROTO_IDP,
    IPPROTO_TRUNK1,   IPPROTO_TRUNK2,  IPPROTO_LEAF1,    IPPROTO_LEAF2,
    IPPROTO_RDP,      IPPROTO_IRTP,    IPPROTO_TP,       IPPROTO_BLT,
    IPPROTO_NSP,      IPPROTO_INP,     IPPROTO_SEP,      IPPROTO_3PC,
    IPPROTO_IDPR,     IPPROTO_XTP,     IPPROTO_DDP,      IPPROTO_CMTP,
    IPPROTO_TPXX,     IPPROTO_IL,      IPPROTO_IPV6,     IPPROTO_ROUTING,
    IPPROTO_FRAGMENT, IPPROTO_SDRP,    IPPROTO_IDRP,     IPPROTO_RSVP,
    IPPROTO_GRE,      IPPROTO_MHRP,    IPPROTO_BHA,      IPPROTO_ESP,
    IPPROTO_AH,       IPPROTO_INLSP,   IPPROTO_SWIPE,    IPPROTO_NHRP,
    IPPROTO_ICMPV6,   IPPROTO_NONE,    IPPROTO_DSTOPTS,  IPPROTO_AHIP,
    IPPROTO_CFTP,     IPPROTO_HELLO,   IPPROTO_SATEXPAK, IPPROTO_KRYPTOLAN,
    IPPROTO_RVD,      IPPROTO_IPPC,    IPPROTO_ADFS,     IPPROTO_SATMON,
    IPPROTO_VISA,     IPPROTO_IPCV,    IPPROTO_CPNX,     IPPROTO_CPHB,
    IPPROTO_WSN,      IPPROTO_PVP,     IPPROTO_BRSATMON, IPPROTO_ND,
    IPPROTO_WBMON,    IPPROTO_WBEXPAK, IPPROTO_EON,      IPPROTO_VMTP,
    IPPROTO_SVMTP,    IPPROTO_VINES,   IPPROTO_TTP,      IPPROTO_IGP,
    IPPROTO_DGP,      IPPROTO_TCF,     IPPROTO_IGRP,     IPPROTO_OSPFIGP,
    IPPROTO_SRPC,     IPPROTO_LARP,    IPPROTO_MTP,      IPPROTO_AX25,
    IPPROTO_IPEIP,    IPPROTO_MICP,    IPPROTO_SCCSP,    IPPROTO_ETHERIP,
    IPPROTO_ENCAP,    IPPROTO_APES,    IPPROTO_GMTP,     IPPROTO_PIM,
    IPPROTO_IPCOMP,   IPPROTO_PGM,     IPPROTO_SCTP,     IPPROTO_DIVERT,
    IPPROTO_RAW
};

basic_socket::basic_socket(basic_socket::domain_t domain,
                           basic_socket::type_t type,
                           basic_socket::protocol_t protocol)
  : _domain(domain), _type(type), _protocol(protocol)
{
    int sock_domain = g_domains[static_cast<int>(_domain)];
    int sock_type = g_types[static_cast<int>(_type)];
    int sock_protocol = g_protocols[static_cast<int>(_protocol)];
    if (sock_domain == -1 || sock_type == -1 || sock_protocol == -1) {
        throw std::invalid_argument("unsuported socket parameters");
    }
    int fd = ::socket(sock_domain, sock_type, sock_protocol);
    if (fd == -1) {
        throw std::system_error(errno, std::system_category(),
                                "creating socket");
    }
    _handle =
        std::unique_ptr<mud::core::handle>(new mud::core::select_handle(fd));
}

basic_socket::basic_socket(basic_socket::domain_t domain,
                           basic_socket::type_t type,
                           basic_socket::protocol_t protocol,
                           std::unique_ptr<mud::core::handle> handle)
  : _domain(domain), _type(type), _protocol(protocol),
    _handle(std::move(handle))
{}

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
    if (this != &rhs) {
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
        ::close(mud::core::internal_handle<int>(_handle));
        _handle.reset(nullptr);
    }
}

const std::unique_ptr<mud::core::handle>&
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

int
basic_socket::error() const
{
    return errno;
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
