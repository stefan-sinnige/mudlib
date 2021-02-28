#include "mud/io/socket.h"
#include "mud/io/exception.h"
#include <iostream>
#include <system_error>
#include <winsock2.h>

BEGIN_MUDLIB_IO_NS

/* Winsock initialiser. */
struct WSAInitialiser
{
    WSAInitialiser() {
        WORD version = MAKEWORD(2, 2);
        WSADATA wsa;
        if (WSAStartup(version, &wsa) != 0) {
            std::cerr << "Error initialising WSA ["
                    << WSAGetLastError() << "]" << std::endl;
            exit(1);
        }
    }
    ~WSAInitialiser() {
        WSACleanup();
    }
} g_wsa_initialiser;

/* Lookup tables for socket enumerations. The order matches the socket.h
 * enumeration. A value of -1 means 'not supported'. */

int g_domains[] = {
    AF_UNIX,
    AF_UNIX,  /** LOCAL */
    AF_INET,
    AF_INET6,
    -1,  /** AX25 */
    AF_IPX,
    AF_APPLETALK,
    -1,  /** X25 */
    AF_DECnet,
    -1, /** KEY */
    -1, /** NETLINK */
    -1, /** PACKET */
    -1, /** RDS */
    -1, /** PPPOX */
    -1, /** LLC */
    -1, /** IB */
    -1, /** MPLS */
    -1, /** CAN */
    -1, /** TIPC */
    -1, /** BLUETOOTH */
    -1, /** ALG */
    -1, /** VSOCK */
    -1, /** KCM */
    -1  /** _XDP */
};

int g_types[] = {
    SOCK_STREAM,
    SOCK_DGRAM,
    SOCK_SEQPACKET,
    SOCK_RAW,
    SOCK_RDM
};

int g_protocols[] = {
    IPPROTO_IP,  /** INTRINSIC */
    IPPROTO_IP,
    IPPROTO_HOPOPTS,
    IPPROTO_ICMP,
    IPPROTO_IGMP,
    IPPROTO_GGP,
    IPPROTO_IPV4,
    -1, /** IPIP */
    IPPROTO_TCP,
    IPPROTO_ST,
    IPPROTO_EGP,
    -1, /** PIGP */
    -1, /** RCCMON */
    -1, /** NVPII */
    IPPROTO_PUP,
    -1, /** ARGUS */
    -1, /** EMCON */
    -1, /** XNET */
    -1, /** CHAOS */
    IPPROTO_UDP,
    -1, /** MUX */
    -1, /** MEAS */
    -1, /** HMP */
    -1, /** PRM */
    IPPROTO_IDP,
    -1, /** TRUNK1 */
    -1, /** TRUNK2 */
    -1, /** LEAF1 */
    -1, /** LEAF2 */
    IPPROTO_RDP,
    -1, /** IRTP */
    -1, /** TP */
    -1, /** BLT */
    -1, /** NSP */
    -1, /** INP */
    -1, /** SEP */
    -1, /** 3PC */
    -1, /** IDPR */
    -1, /** XTP */
    -1, /** DDP */
    -1, /** CMTP */
    -1, /** TPXX */
    -1, /** IL */
    IPPROTO_IPV6,
    IPPROTO_ROUTING,
    IPPROTO_FRAGMENT,
    -1, /** SDRP */
    -1, /** IDRP */
    -1, /** RSVP */
    -1, /** GRE */
    -1, /** MHRP */
    -1, /** BHA */
    IPPROTO_ESP,
    IPPROTO_AH,
    -1, /** INLSP */
    -1, /** SWIPE */
    -1, /** NHRP */
    IPPROTO_ICMPV6,
    IPPROTO_NONE,
    IPPROTO_DSTOPTS,
    -1, /** AHIP */
    -1, /** CFTP */
    -1, /** HELLO */
    -1, /** SATEXPAK */
    -1, /** KRYPTOLAN */
    -1, /** RVD */
    -1, /** IPPC */
    -1, /** ADFS */
    -1, /** SATMON */
    -1, /** VISA */
    -1, /** IPCV */
    -1, /** CPNX */
    -1, /** CPHB */
    -1, /** WSN */
    -1, /** PVP */
    -1, /** BRSATMON */
    IPPROTO_ND,
    -1, /** WBMON */
    -1, /** WBEXPAK */
    -1, /** EON */
    -1, /** VMTP */
    -1, /** SVMTP */
    -1, /** VINES */
    -1, /** TTP */
    IPPROTO_IGP,
    -1, /** DGP */
    -1, /** TCF */
    -1, /** IGRP */
    -1, /** OSPFIGP */
    -1, /** SRPC */
    -1, /** LARP */
    -1, /** MTP */
    -1, /** AX25 */
    -1, /** IPEIP */
    -1, /** MICP */
    -1, /** SCCSP */
    -1, /** ETHERIP */
    -1, /** ENCAP */
    -1, /** APES */
    -1, /** GMTP */
    IPPROTO_PIM,
    -1, /** IPCOMP */
    IPPROTO_PGM,
    IPPROTO_SCTP,
    -1, /** DIVERT */
    IPPROTO_RAW
};

basic_socket::basic_socket(
        basic_socket::domain_t domain,
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
        throw std::system_error(error(), std::system_category(),
                "creating socket");
    }
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
        ::closesocket(*_handle);
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

int
basic_socket::error() const
{
    return ::GetLastError();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

