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

int g_domains[] = {
#ifdef AF_UNSPEC
    AF_UNSPEC,
#else
    0, /** UNSPEC */
#endif
#ifdef AF_UNIX
    AF_UNIX,
#else
    -1, /** UNIX */
#endif
#ifdef AF_LOCAL
    AF_LOCAL,
#else
    -1, /** LOCAL */
#endif
#ifdef AF_INET
    AF_INET,
#else
    -1, /** INET */
#endif
#ifdef AF_INET6
    AF_INET6,
#else
    -1, /** INET6 */
#endif
#ifdef AF_AX25
    AF_AX25,
#else
    -1, /** AX25 */
#endif
#ifdef AF_IPX
    AF_IPX,
#else
    -1, /** IPX */
#endif
#ifdef AF_APPLETALK
    AF_APPLETALK,
#else
    -1, /** APPLETALK */
#endif
#ifdef AF_X25
    AF_X25,
#else
    -1, /** X25 */
#endif
    AF_DECnet,
#ifdef AF_KEY
    AF_KEY,
#else
    -1, /** KEY */
#endif
#ifdef AF_NETLINK
    AF_NETLINK,
#else
    -1, /** NETLINK */
#endif
#ifdef AF_PACKET
    AF_PACKET,
#else
    -1, /** PACKET */
#endif
#ifdef AF_RDS
    AF_RDS,
#else
    -1, /** RDS */
#endif
#ifdef AF_PPPOX
    AF_PPPOX,
#else
    -1, /** PPPOX */
#endif
#ifdef AF_LLC
    AF_LLC,
#else
    -1, /** LLC */
#endif
#ifdef AF_IB
    AF_IB,
#else
    -1, /** IB */
#endif
#ifdef AF_MPLS
    AF_MPLS,
#else
    -1, /** MPLS */
#endif
#ifdef AF_CAN
    AF_CAN,
#else
    -1, /** CAN */
#endif
#ifdef AF_TIPC
    AF_TIPC,
#else
    -1, /** TIPC */
#endif
#ifdef AF_BLUETOOTH
    AF_BLUETOOTH,
#else
    -1, /** BLUETOOTH */
#endif
#ifdef AF_ALG
    AF_ALG,
#else
    -1, /** ALG */
#endif
#ifdef AF_VSOCK
    AF_VSOCK,
#else
    -1, /** VSOCK */
#endif
#ifdef AF_KCM
    AF_KCM,
#else
    -1, /** KCM */
#endif
#ifdef AF_XDP
    AF_XDP
#else
    -1 /** XDP */
#endif
};

int g_types[] = {
    0,
    SOCK_STREAM,
    SOCK_DGRAM,
    SOCK_SEQPACKET,
    SOCK_RAW,
    SOCK_RDM
};

int g_protocols[]{
#ifdef IPPROTO_IP
    IPPROTO_IP, /* UNSPEC */
#else
    0, /* UNSPEC */
#endif
#ifdef IPPROTO_IP
    IPPROTO_IP, /* INTRINSIC */
#else
    -1, /* INTRINSIC */
#endif
#ifdef IPPROTO_IP
    IPPROTO_IP,
#else
    -1, /** IP */
#endif
#ifdef IPPROTO_HOPOPTS
    IPPROTO_HOPOPTS,
#else
    -1, /** HOPOPTS */
#endif
#ifdef IPPROTO_ICMP
    IPPROTO_ICMP,
#else
    -1, /** ICMP */
#endif
#ifdef IPPROTO_IGMP
    IPPROTO_IGMP,
#else
    -1, /** IGMP */
#endif
#ifdef IPPROTO_GGP
    IPPROTO_GGP,
#else
    -1, /** GGP */
#endif
#ifdef IPPROTO_IPV4
    IPPROTO_IPV4,
#else
    -1, /** IPV4 */
#endif
#ifdef IPPROTO_IPIP
    IPPROTO_IPIP,
#else
    -1, /** IPIP */
#endif
#ifdef IPPROTO_TCP
    IPPROTO_TCP,
#else
    -1, /** TCP */
#endif
#ifdef IPPROTO_ST
    IPPROTO_ST,
#else
    -1, /** ST */
#endif
#ifdef IPPROTO_EGP
    IPPROTO_EGP,
#else
    -1, /** EGP */
#endif
#ifdef IPPROTO_PIGP
    IPPROTO_PIGP,
#else
    -1, /** PIGP */
#endif
#ifdef IPPROTO_RCCMON
    IPPROTO_RCCMON,
#else
    -1, /** RCCMON */
#endif
#ifdef IPPROTO_NVPII
    IPPROTO_NVPII,
#else
    -1, /** NVPII */
#endif
#ifdef IPPROTO_PUP
    IPPROTO_PUP,
#else
    -1, /** PUP */
#endif
#ifdef IPPROTO_ARGUS
    IPPROTO_ARGUS,
#else
    -1, /** ARGUS */
#endif
#ifdef IPPROTO_EMCON
    IPPROTO_EMCON,
#else
    -1, /** EMCON */
#endif
#ifdef IPPROTO_XNET
    IPPROTO_XNET,
#else
    -1, /** XNET */
#endif
#ifdef IPPROTO_CHAOS
    IPPROTO_CHAOS,
#else
    -1, /** CHAOS */
#endif
#ifdef IPPROTO_UDP
    IPPROTO_UDP,
#else
    -1, /** UDP */
#endif
#ifdef IPPROTO_MUX
    IPPROTO_MUX,
#else
    -1, /** MUX */
#endif
#ifdef IPPROTO_MEAS
    IPPROTO_MEAS,
#else
    -1, /** MEAS */
#endif
#ifdef IPPROTO_HMP
    IPPROTO_HMP,
#else
    -1, /** HMP */
#endif
#ifdef IPPROTO_PRM
    IPPROTO_PRM,
#else
    -1, /** PRM */
#endif
#ifdef IPPROTO_IDP
    IPPROTO_IDP,
#else
    -1, /** IDP */
#endif
#ifdef IPPROTO_TRUNK1
    IPPROTO_TRUNK1,
#else
    -1, /** TRUNK1 */
#endif
#ifdef IPPROTO_TRUNK2
    IPPROTO_TRUNK2,
#else
    -1, /** TRUNK2 */
#endif
#ifdef IPPROTO_LEAF1
    IPPROTO_LEAF1,
#else
    -1, /** LEAF1 */
#endif
#ifdef IPPROTO_LEAF2
    IPPROTO_LEAF2,
#else
    -1, /** LEAF2 */
#endif
#ifdef IPPROTO_RDP
    IPPROTO_RDP,
#else
    -1, /** RDP */
#endif
#ifdef IPPROTO_IRTP
    IPPROTO_IRTP,
#else
    -1, /** IRTP */
#endif
#ifdef IPPROTO_TP
    IPPROTO_TP,
#else
    -1, /** TP */
#endif
#ifdef IPPROTO_BLT
    IPPROTO_BLT,
#else
    -1, /** BLT */
#endif
#ifdef IPPROTO_NSP
    IPPROTO_NSP,
#else
    -1, /** NSP */
#endif
#ifdef IPPROTO_INP
    IPPROTO_INP,
#else
    -1, /** INP */
#endif
#ifdef IPPROTO_SEP
    IPPROTO_SEP,
#else
    -1, /** SEP */
#endif
#ifdef IPPROTO_3PC
    IPPROTO_3PC,
#else
    -1, /** 3PC */
#endif
#ifdef IPPROTO_IDPR
    IPPROTO_IDPR,
#else
    -1, /** IDPR */
#endif
#ifdef IPPROTO_XTP
    IPPROTO_XTP,
#else
    -1, /** XTP */
#endif
#ifdef IPPROTO_DDP
    IPPROTO_DDP,
#else
    -1, /** DDP */
#endif
#ifdef IPPROTO_CMTP
    IPPROTO_CMTP,
#else
    -1, /** CMTP */
#endif
#ifdef IPPROTO_TPXX
    IPPROTO_TPXX,
#else
    -1, /** TPXX */
#endif
#ifdef IPPROTO_IL
    IPPROTO_IL,
#else
    -1, /** IL */
#endif
#ifdef IPPROTO_IPV6
    IPPROTO_IPV6,
#else
    -1, /** IPV6 */
#endif
#ifdef IPPROTO_ROUTING
    IPPROTO_ROUTING,
#else
    -1, /** ROUTING */
#endif
#ifdef IPPROTO_FRAGMENT
    IPPROTO_FRAGMENT,
#else
    -1, /** FRAGMENT */
#endif
#ifdef IPPROTO_SDRP
    IPPROTO_SDRP,
#else
    -1, /** SDRP */
#endif
#ifdef IPPROTO_IDRP
    IPPROTO_IDRP,
#else
    -1, /** IDRP */
#endif
#ifdef IPPROTO_RSVP
    IPPROTO_RSVP,
#else
    -1, /** RSVP */
#endif
#ifdef IPPROTO_GRE
    IPPROTO_GRE,
#else
    -1, /** GRE */
#endif
#ifdef IPPROTO_MHRP
    IPPROTO_MHRP,
#else
    -1, /** MHRP */
#endif
#ifdef IPPROTO_BHA
    IPPROTO_BHA,
#else
    -1, /** BHA */
#endif
#ifdef IPPROTO_ESP
    IPPROTO_ESP,
#else
    -1, /** ESP */
#endif
#ifdef IPPROTO_AH
    IPPROTO_AH,
#else
    -1, /** AH */
#endif
#ifdef IPPROTO_INLSP
    IPPROTO_INLSP,
#else
    -1, /** INLSP */
#endif
#ifdef IPPROTO_SWIPE
    IPPROTO_SWIPE,
#else
    -1, /** SWIPE */
#endif
#ifdef IPPROTO_NHRP
    IPPROTO_NHRP,
#else
    -1, /** NHRP */
#endif
#ifdef IPPROTO_ICMPV6
    IPPROTO_ICMPV6,
#else
    -1, /** ICMPV6 */
#endif
#ifdef IPPROTO_NONE
    IPPROTO_NONE,
#else
    -1, /** NONE */
#endif
#ifdef IPPROTO_DSTOPTS
    IPPROTO_DSTOPTS,
#else
    -1, /** DSTOPTS */
#endif
#ifdef IPPROTO_AHIP
    IPPROTO_AHIP,
#else
    -1, /** AHIP */
#endif
#ifdef IPPROTO_CFTP
    IPPROTO_CFTP,
#else
    -1, /** CFTP */
#endif
#ifdef IPPROTO_HELLO
    IPPROTO_HELLO,
#else
    -1, /** HELLO */
#endif
#ifdef IPPROTO_SATEXPAK
    IPPROTO_SATEXPAK,
#else
    -1, /** SATEXPAK */
#endif
#ifdef IPPROTO_KRYPTOLAN
    IPPROTO_KRYPTOLAN,
#else
    -1, /** KRYPTOLAN */
#endif
#ifdef IPPROTO_RVD
    IPPROTO_RVD,
#else
    -1, /** RVD */
#endif
#ifdef IPPROTO_IPPC
    IPPROTO_IPPC,
#else
    -1, /** IPPC */
#endif
#ifdef IPPROTO_ADFS
    IPPROTO_ADFS,
#else
    -1, /** ADFS */
#endif
#ifdef IPPROTO_SATMON
    IPPROTO_SATMON,
#else
    -1, /** SATMON */
#endif
#ifdef IPPROTO_VISA
    IPPROTO_VISA,
#else
    -1, /** VISA */
#endif
#ifdef IPPROTO_IPCV
    IPPROTO_IPCV,
#else
    -1, /** IPCV */
#endif
#ifdef IPPROTO_CPNX
    IPPROTO_CPNX,
#else
    -1, /** CPNX */
#endif
#ifdef IPPROTO_CPHB
    IPPROTO_CPHB,
#else
    -1, /** CPHB */
#endif
#ifdef IPPROTO_WSN
    IPPROTO_WSN,
#else
    -1, /** WSN */
#endif
#ifdef IPPROTO_PVP
    IPPROTO_PVP,
#else
    -1, /** PVP */
#endif
#ifdef IPPROTO_BRSATMON
    IPPROTO_BRSATMON,
#else
    -1, /** BRSATMON */
#endif
#ifdef IPPROTO_ND
    IPPROTO_ND,
#else
    -1, /** ND */
#endif
#ifdef IPPROTO_WBMON
    IPPROTO_WBMON,
#else
    -1, /** WBMON */
#endif
#ifdef IPPROTO_WBEXPAK
    IPPROTO_WBEXPAK,
#else
    -1, /** WBEXPAK */
#endif
#ifdef IPPROTO_EON
    IPPROTO_EON,
#else
    -1, /** EON */
#endif
#ifdef IPPROTO_VMTP
    IPPROTO_VMTP,
#else
    -1, /** VMTP */
#endif
#ifdef IPPROTO_SVMTP
    IPPROTO_SVMTP,
#else
    -1, /** SVMTP */
#endif
#ifdef IPPROTO_VINES
    IPPROTO_VINES,
#else
    -1, /** VINES */
#endif
#ifdef IPPROTO_TTP
    IPPROTO_TTP,
#else
    -1, /** TTP */
#endif
#ifdef IPPROTO_IGP
    IPPROTO_IGP,
#else
    -1, /** IGP */
#endif
#ifdef IPPROTO_DGP
    IPPROTO_DGP,
#else
    -1, /** DGP */
#endif
#ifdef IPPROTO_TCF
    IPPROTO_TCF,
#else
    -1, /** TCF */
#endif
#ifdef IPPROTO_IGRP
    IPPROTO_IGRP,
#else
    -1, /** IGRP */
#endif
#ifdef IPPROTO_OSPFIGP
    IPPROTO_OSPFIGP,
#else
    -1, /** OSPFIGP */
#endif
#ifdef IPPROTO_SRPC
    IPPROTO_SRPC,
#else
    -1, /** SRPC */
#endif
#ifdef IPPROTO_LARP
    IPPROTO_LARP,
#else
    -1, /** LARP */
#endif
#ifdef IPPROTO_MTP
    IPPROTO_MTP,
#else
    -1, /** MTP */
#endif
#ifdef IPPROTO_AX25
    IPPROTO_AX25,
#else
    -1, /** AX25 */
#endif
#ifdef IPPROTO_IPEIP
    IPPROTO_IPEIP,
#else
    -1, /** IPEIP */
#endif
#ifdef IPPROTO_MICP
    IPPROTO_MICP,
#else
    -1, /** MICP */
#endif
#ifdef IPPROTO_SCCSP
    IPPROTO_SCCSP,
#else
    -1, /** SCCSP */
#endif
#ifdef IPPROTO_ETHERIP
    IPPROTO_ETHERIP,
#else
    -1, /** ETHERIP */
#endif
#ifdef IPPROTO_ENCAP
    IPPROTO_ENCAP,
#else
    -1, /** ENCAP */
#endif
#ifdef IPPROTO_APES
    IPPROTO_APES,
#else
    -1, /** APES */
#endif
#ifdef IPPROTO_GMTP
    IPPROTO_GMTP,
#else
    -1, /** GMTP */
#endif
#ifdef IPPROTO_PIM
    IPPROTO_PIM,
#else
    -1, /** PIM */
#endif
#ifdef IPPROTO_IPCOMP
    IPPROTO_IPCOMP,
#else
    -1, /** IPCOMP */
#endif
#ifdef IPPROTO_PGM
    IPPROTO_PGM,
#else
    -1, /** PGM */
#endif
#ifdef IPPROTO_SCTP
    IPPROTO_SCTP,
#else
    -1, /** SCTP */
#endif
#ifdef IPPROTO_DIVERT
    IPPROTO_DIVERT,
#else
    -1, /** DIVERT */
#endif
#ifdef IPPROTO_RAW
    IPPROTO_RAW
#else
    -1, /** RAW */
#endif
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
    LOG(log);
    INFO(log) << "Creating socket fd: " << fd << std::endl;
    _handle = std::make_shared<mud::core::select_handle>(fd);
}

basic_socket::basic_socket(basic_socket::domain_t domain,
                           basic_socket::type_t type,
                           basic_socket::protocol_t protocol,
                           std::shared_ptr<mud::core::handle> handle)
  : _domain(domain), _type(type), _protocol(protocol),
    _handle(handle)
{}

basic_socket::~basic_socket()
{
    close();
}

void
basic_socket::close()
{
    if (_handle != nullptr) {
        LOG(log);
        INFO(log) << "Closing socket fd: "
                  << mud::core::internal_handle<int>(_handle) << std::endl;
    }
}

std::shared_ptr<mud::core::handle>
basic_socket::handle()
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
