#ifndef _MUDLIB_IO_SOCKET_H_
#define _MUDLIB_IO_SOCKET_H_

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <mud/io/ns.h>
#include <mud/io/kernel_handle.h>

BEGIN_MUDLIB_IO_NS

/**
 * @brief A class for all socket definitions.
 *
 * The @c basic_socket serves as the basic definition of all the sockets. It
 * cannot be constructed directly, but only through the derived socket
 * definition.
 */
class basic_socket
{
public:
    /**
     * The communication domain. These may not all be supported by all
     * operating systems.
     */
    enum class domain_t {
        UNIX = AF_UNIX,          /**< Local communication */
        LOCAL = AF_LOCAL,        /**< Synonym for UNIX */
        INET = AF_INET,          /**< IPv4 */
        INET6 = AF_INET6,        /**< IPv6 */
#ifdef LINUX
        AX25 = AF_AX25,          /**< Amateur radio AX.25 */
#endif
        IPX = AF_IPX,            /**< IPX */
        APPLETALK = AF_APPLETALK,/**< AppleTalk */
#ifdef LINUX
        X25 = AF_X25,            /**< ITU-T X.25 / ISO-8208 */
#endif
        DECnet = AF_DECnet,      /**< DECnet */
#ifdef LINUX
        KEY = AF_KEY,            /**< Key management / IPsec */
        NETLINK = AF_NETLINK,    /**< Kernel user interface */
        PACKET = AF_PACKET,      /**< Low-level packet interface */
        RDS = AF_RDS,            /**< Reliable Datagram Sockets */
        PPPOX = AF_PPPOX,        /**< PPP transport layer (L2TP, PPoE) */
        LLC = AF_LLC,            /**< Logical link control (IEEE 802.2) */
        IB = AF_IB,              /**< Infiniband */
        MPLS = AF_MPLS,          /**< Multiprotocol label switching */
        CAN = AF_CAN,            /**< Controller Area Network */
        TIPC = AF_TIPC,          /**< Transparent Inter-Process Communication */
        BLUETOOTH = AF_BLUETOOTH,/**< Bluetooth low-level socket */
        ALG = AF_ALG,            /**< Kernel crypto API interface */
        VSOCK = AF_VSOCK,        /**< VMWare sockets */
        KCM = AF_KCM,            /**< Kernel connection multiplexor interface */
        XDP = AF_XDP             /**< Express data path */
#endif
    };

    /**
     * The socket communication type.
     */
    enum class type_t {
        STREAM = SOCK_STREAM,       /**< Sequenced, reliable, two-way
                                         connection based byte streams */
        DGRAM = SOCK_DGRAM,         /**< Connectionless, unreliable datagrams
                                         of fixed maximum length */
        SEQPACKET = SOCK_SEQPACKET, /**< Sequenced, reliable, two-way
                                         connection based datagrams of fixed
                                         maximum length */
        RAW = SOCK_RAW,             /**< Raw network protocol access */
        RDM = SOCK_RDM              /**< Reliable datagram without ordering
                                         guarantee. */
    };

    /**
     * The socket communication protocol (as defined by IANA).
     */
    enum class protocol_t {
        INTRINSIC = 0,              /**< Native to the socket type. */
        IP = IPPROTO_IP,            /**< dummy for IP */
        HOPOPTS = IPPROTO_HOPOPTS,  /**< IP6 hop-by-hop options */
        ICMP = IPPROTO_ICMP,        /**< control message protocol */
        IGMP = IPPROTO_IGMP,        /**< group mgmt protocol */
        GGP = IPPROTO_GGP,          /**< gateway^2 (deprecated) */
        IPV4 = IPPROTO_IPV4,        /**< IPv4 encapsulation */
        IPIP = IPPROTO_IPIP,        /**< for compatibility */
        TCP = IPPROTO_TCP,          /**< tcp */
        ST = IPPROTO_ST,            /**< Stream protocol II */
        EGP = IPPROTO_EGP,          /**< exterior gateway protocol */
        PIGP = IPPROTO_PIGP,        /**< private interior gateway */
        RCCMON = IPPROTO_RCCMON,    /**< BBN RCC Monitoring */
        NVPII = IPPROTO_NVPII,      /**< network voice protocol*/
        PUP = IPPROTO_PUP,          /**< pup */
        ARGUS = IPPROTO_ARGUS,      /**< Argus */
        EMCON = IPPROTO_EMCON,      /**< EMCON */
        XNET = IPPROTO_XNET,        /**< Cross Net Debugger */
        CHAOS = IPPROTO_CHAOS,      /**< Chaos*/
        UDP = IPPROTO_UDP,          /**< user datagram protocol */
        MUX = IPPROTO_MUX,          /**< Multiplexing */
        MEAS = IPPROTO_MEAS,        /**< DCN Measurement Subsystems */
        HMP = IPPROTO_HMP,          /**< Host Monitoring */
        PRM = IPPROTO_PRM,          /**< Packet Radio Measurement */
        IDP = IPPROTO_IDP,          /**< xns idp */
        TRUNK1 = IPPROTO_TRUNK1,    /**< Trunk-1 */
        TRUNK2 = IPPROTO_TRUNK2,    /**< Trunk-2 */
        LEAF1 = IPPROTO_LEAF1,      /**< Leaf-1 */
        LEAF2 = IPPROTO_LEAF2,      /**< Leaf-2 */
        RDP = IPPROTO_RDP,          /**< Reliable Data */
        IRTP = IPPROTO_IRTP,        /**< Reliable Transaction */
        TP = IPPROTO_TP,            /**< tp-4 w/ class negotiation */
        BLT = IPPROTO_BLT,          /**< Bulk Data Transfer */
        NSP = IPPROTO_NSP,          /**< Network Services */
        INP = IPPROTO_INP,          /**< Merit Internodal */
        SEP = IPPROTO_SEP,          /**< Sequential Exchange */
        THREEPC = IPPROTO_3PC,      /**< Third Party Connect */
        IDPR = IPPROTO_IDPR,        /**< InterDomain Policy Routing */
        XTP = IPPROTO_XTP,          /**< XTP */
        DDP = IPPROTO_DDP,          /**< Datagram Delivery */
        CMTP = IPPROTO_CMTP,        /**< Control Message Transport */
        TPXX = IPPROTO_TPXX,        /**< TP++ Transport */
        IL = IPPROTO_IL,            /**< IL transport protocol */
        IPV6 = IPPROTO_IPV6,        /**< IP6 header */
        SDRP = IPPROTO_SDRP,        /**< Source Demand Routing */
        ROUTING = IPPROTO_ROUTING,  /**< IP6 routing header */
        FRAGMENT = IPPROTO_FRAGMENT,/**< IP6 fragmentation header */
        IDRP = IPPROTO_IDRP,        /**< InterDomain Routing*/
        RSVP = IPPROTO_RSVP,        /**< resource reservation */
        GRE = IPPROTO_GRE,          /**< General Routing Encap. */
        MHRP = IPPROTO_MHRP,        /**< Mobile Host Routing */
        BHA = IPPROTO_BHA,          /**< BHA */
        ESP = IPPROTO_ESP,          /**< IP6 Encap Sec. Payload */
        AH = IPPROTO_AH,            /**< IP6 Auth Header */
        INLSP = IPPROTO_INLSP,      /**< Integ. Net Layer Security */
        SWIPE = IPPROTO_SWIPE,      /**< IP with encryption */
        NHRP = IPPROTO_NHRP,        /**< Next Hop Resolution */
        ICMPV6 = IPPROTO_ICMPV6,    /**< ICMP6 */
        NONE = IPPROTO_NONE,        /**< IP6 no next header */
        DSTOPTS = IPPROTO_DSTOPTS,  /**< IP6 destination option */
        AHIP = IPPROTO_AHIP,        /**< any host internal protocol */
        CFTP = IPPROTO_CFTP,        /**< CFTP */
        HELLO = IPPROTO_HELLO,      /**< "hello" routing protocol */
        SATEXPAK = IPPROTO_SATEXPAK,/**< SATNET/Backroom EXPAK */
        KRYPTOLAN = IPPROTO_KRYPTOLAN,/**< Kryptolan */
        RVD = IPPROTO_RVD,          /**< Remote Virtual Disk */
        IPPC = IPPROTO_IPPC,        /**< Pluribus Packet Core */
        ADFS = IPPROTO_ADFS,        /**< Any distributed FS */
        SATMON = IPPROTO_SATMON,    /**< Satnet Monitoring */
        VISA = IPPROTO_VISA,        /**< VISA Protocol */
        IPCV = IPPROTO_IPCV,        /**< Packet Core Utility */
        CPNX = IPPROTO_CPNX,        /**< Comp. Prot. Net. Executive */
        CPHB = IPPROTO_CPHB,        /**< Comp. Prot. HeartBeat */
        WSN = IPPROTO_WSN,          /**< Wang Span Network */
        PVP = IPPROTO_PVP,          /**< Packet Video Protocol */
        BRSATMON = IPPROTO_BRSATMON,/**< BackRoom SATNET Monitoring */
        ND = IPPROTO_ND,            /**< Sun net disk proto (temp.) */
        WBMON = IPPROTO_WBMON,      /**< WIDEBAND Monitoring */
        WBEXPAK = IPPROTO_WBEXPAK,  /**< WIDEBAND EXPAK */
        EON = IPPROTO_EON,          /**< ISO cnlp */
        VMTP = IPPROTO_VMTP,        /**< VMTP */
        SVMTP = IPPROTO_SVMTP,      /**< Secure VMTP */
        VINES = IPPROTO_VINES,      /**< Banyon VINES */
        TTP = IPPROTO_TTP,          /**< TTP */
        IGP = IPPROTO_IGP,          /**< NSFNET-IGP */
        DGP = IPPROTO_DGP,          /**< dissimilar gateway prot. */
        TCF = IPPROTO_TCF,          /**< TCF */
        IGRP = IPPROTO_IGRP,        /**< Cisco/GXS IGRP */
        OSPFIGP = IPPROTO_OSPFIGP,  /**< OSPFIGP */
        SRPC = IPPROTO_SRPC,        /**< Strite RPC protocol */
        LARP = IPPROTO_LARP,        /**< Locus Address Resoloution */
        MTP = IPPROTO_MTP,          /**< Multicast Transport */
        AX25 = IPPROTO_AX25,        /**< AX.25 Frames */
        IPEIP = IPPROTO_IPEIP,      /**< IP encapsulated in IP */
        MICP = IPPROTO_MICP,        /**< Mobile Int.ing control */
        SCCSP = IPPROTO_SCCSP,      /**< Semaphore Comm. security */
        ETHERIP = IPPROTO_ETHERIP,  /**< Ethernet IP encapsulation */
        ENCAP = IPPROTO_ENCAP,      /**< encapsulation header */
        APES = IPPROTO_APES,        /**< any private encr. scheme */
        GMTP = IPPROTO_GMTP,        /**< GMTP*/
        PIM = IPPROTO_PIM,          /**< Protocol Independent Mcast */
        IPCOMP = IPPROTO_IPCOMP,    /**< payload compression (IPComp) */
        PGM = IPPROTO_PGM,          /**< PGM */
        SCTP = IPPROTO_SCTP,        /**< SCTP */
        DIVERT = IPPROTO_DIVERT,    /**< divert pseudo-protocol */
        RAW = IPPROTO_RAW           /**< raw IP packet */
    };

    /**
     * @brief Construct an socket.
     * @param domain [in] The communication domain.
     * @param type [in] The socket type.
     * @param protocol [in] The protocol type.
     */
    basic_socket(domain_t domain, type_t type, protocol_t protocol);

    /**
     * Move constructor, moving socket ownership.
     */
    basic_socket(basic_socket&& rhs);

    /**
     * @brief Move assignment, moving socket ownership.
     */
    basic_socket& operator=(basic_socket&& rhs);

    /**
     * @brief Destructor
     */
    virtual ~basic_socket();

    /**
     * The socket file-descriptor handle.
     */
    const std::unique_ptr<kernel_handle>& handle() const;

    /**
     * The domain when the socket was created.
     */
    domain_t domain() const;

    /**
     * @brief The type when the socket was created.
     */
    type_t type() const;

    /**
     * @brief The protocol when the socket was created.
     */
    protocol_t protocol() const;

    /**
     * Non-copyable.
     */
    basic_socket(const basic_socket&) = delete;
    basic_socket& operator=(const basic_socket&) = delete;

protected:
    /**
     * @brief Construct a socket with an existing handle..
     * @param domain [in] The communication domain.
     * @param type [in] The socket type.
     * @param protocol [in] The protocol type.
     * @param handle [in] The existing socket handle.
     */
    basic_socket(domain_t domain, type_t type, protocol_t protocol,
            std::unique_ptr<kernel_handle> handle);

private:
    /* The socket descriptor. */
    std::unique_ptr<mud::io::kernel_handle> _handle;

    /* The domain */
    domain_t _domain;

    /* The type */
    type_t _type;

    /* The protocol */
    protocol_t _protocol;
};


END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_SOCKET_H_ */

