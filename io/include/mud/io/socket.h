#ifndef _MUDLIB_IO_SOCKET_H_
#define _MUDLIB_IO_SOCKET_H_

#include <mud/core/handle.h>
#include <mud/io/ns.h>
#include <string>

BEGIN_MUDLIB_IO_NS

/**
 * @brief A class for all socket definitions.
 *
 * The @c basic_socket serves as the basic definition of all the sockets. It
 * cannot be constructed directly, but only through the derived socket
 * definition.
 */
class MUDLIB_IO_API basic_socket
{
public:
    /**
     * The communication domain. These may not all be supported by all
     * operating systems.
     */
    enum class domain_t
    {
        UNSPEC,    /**< Unspecified */
        UNIX,      /**< Local communication */
        LOCAL,     /**< Synonym for UNIX */
        INET,      /**< IPv4 */
        INET6,     /**< IPv6 */
        AX25,      /**< Amateur radio AX.25 */
        IPX,       /**< IPX */
        APPLETALK, /**< AppleTalk */
        X25,       /**< ITU-T X.25 / ISO-8208 */
        DECnet,    /**< DECnet */
        KEY,       /**< Key management / IPsec */
        NETLINK,   /**< Kernel user interface */
        PACKET,    /**< Low-level packet interface */
        RDS,       /**< Reliable Datagram Sockets */
        PPPOX,     /**< PPP transport layer (L2T, PPOe) */
        LLC,       /**< Logical link control (IEEE 802.2) */
        IB,        /**< Infiniband */
        MPLS,      /**< Multiprotocol label switching */
        CAN,       /**< Controller Area Network */
        TIPC,      /**< Transparent Inter-Process Communication */
        BLUETOOTH, /**< Bluetooth low-level socket */
        ALG,       /**< Kernel crypto API interface */
        VSOCK,     /**< VMWare sockets */
        KCM,       /**< Kernel connection multiplexor interface */
        XDP        /**< Express data path */
    };

    /**
     * The socket communication type.
     */
    enum class type_t
    {
        UNSPEC,    /**< Unspecified */
        STREAM,    /**< Sequenced, reliable, two-way connection based byte
                        streams */
        DGRAM,     /**< Connectionless, unreliable datagrams of fixed
                        maximum length */
        SEQPACKET, /**< Sequenced, reliable, two-way connection based
                        datagrams of fixed maximum length */
        RAW,       /**< Raw network protocol access */
        RDM        /**< Reliable datagram without ordering guarantee. */
    };

    /**
     * The socket communication protocol (as defined by IANA).
     */
    enum class protocol_t
    {
        UNSPEC,    /**< Unspecified */
        INTRINSIC, /**< Native to the socket type. */
        IP,        /**< dummy for IP */
        HOPOPTS,   /**< IP6 hop-by-hop options */
        ICMP,      /**< control message protocol */
        IGMP,      /**< group mgmt protocol */
        GGP,       /**< gateway^2 (deprecated) */
        IPV4,      /**< IPv4 encapsulation */
        IPIP,      /**< for compatibility */
        TCP,       /**< tcp */
        ST,        /**< Stream protocol II */
        EGP,       /**< exterior gateway protocol */
        PIGP,      /**< private interior gateway */
        RCCMON,    /**< BBN RCC Monitoring */
        NVPII,     /**< network voice protocol*/
        PUP,       /**< pup */
        ARGUS,     /**< Argus */
        EMCON,     /**< EMCON */
        XNET,      /**< Cross Net Debugger */
        CHAOS,     /**< Chaos*/
        UDP,       /**< user datagram protocol */
        MUX,       /**< Multiplexing */
        MEAS,      /**< DCN Measurement Subsystems */
        HMP,       /**< Host Monitoring */
        PRM,       /**< Packet Radio Measurement */
        IDP,       /**< xns idp */
        TRUNK1,    /**< Trunk-1 */
        TRUNK2,    /**< Trunk-2 */
        LEAF1,     /**< Leaf-1 */
        LEAF2,     /**< Leaf-2 */
        RDP,       /**< Reliable Data */
        IRTP,      /**< Reliable Transaction */
        TP,        /**< tp-4 w/ class negotiation */
        BLT,       /**< Bulk Data Transfer */
        NSP,       /**< Network Services */
        INP,       /**< Merit Internodal */
        SEP,       /**< Sequential Exchange */
        THREEPC,   /**< Third Party Connect */
        IDPR,      /**< InterDomain Policy Routing */
        XTP,       /**< XTP */
        DDP,       /**< Datagram Delivery */
        CMTP,      /**< Control Message Transport */
        TPXX,      /**< TP++ Transport */
        IL,        /**< IL transport protocol */
        IPV6,      /**< IP6 header */
        ROUTING,   /**< IP6 routing header */
        FRAGMENT,  /**< IP6 fragmentation header */
        SDRP,      /**< Source Demand Routing */
        IDRP,      /**< InterDomain Routing*/
        RSVP,      /**< resource reservation */
        GRE,       /**< General Routing Encap. */
        MHRP,      /**< Mobile Host Routing */
        BHA,       /**< BHA */
        ESP,       /**< IP6 Encap Sec. Payload */
        AH,        /**< IP6 Auth Header */
        INLSP,     /**< Integ. Net Layer Security */
        SWIPE,     /**< IP with encryption */
        NHRP,      /**< Next Hop Resolution */
        ICMPV6,    /**< ICMP6 */
        NONE,      /**< IP6 no next header */
        DSTOPTS,   /**< IP6 destination option */
        AHIP,      /**< any host internal protocol */
        CFTP,      /**< CFTP */
        HELLO,     /**< "hello" routing protocol */
        SATEXPAK,  /**< SATNET/Backroom EXPAK */
        KRYPTOLAN, /**< Kryptolan */
        RVD,       /**< Remote Virtual Disk */
        IPPC,      /**< Pluribus Packet Core */
        ADFS,      /**< Any distributed FS */
        SATMON,    /**< Satnet Monitoring */
        VISA,      /**< VISA Protocol */
        IPCV,      /**< Packet Core Utility */
        CPNX,      /**< Comp. Prot. Net. Executive */
        CPHB,      /**< Comp. Prot. HeartBeat */
        WSN,       /**< Wang Span Network */
        PVP,       /**< Packet Video Protocol */
        BRSATMON,  /**< BackRoom SATNET Monitoring */
        ND,        /**< Sun net disk proto (temp.) */
        WBMON,     /**< WIDEBAND Monitoring */
        WBEXPAK,   /**< WIDEBAND EXPAK */
        EON,       /**< ISO cnlp */
        VMTP,      /**< VMTP */
        SVMTP,     /**< Secure VMTP */
        VINES,     /**< Banyon VINES */
        TTP,       /**< TTP */
        IGP,       /**< NSFNET-IGP */
        DGP,       /**< dissimilar gateway prot. */
        TCF,       /**< TCF */
        IGRP,      /**< Cisco/GXS IGRP */
        OSPFIGP,   /**< OSPFIGP */
        SRPC,      /**< Strite RPC protocol */
        LARP,      /**< Locus Address Resoloution */
        MTP,       /**< Multicast Transport */
        AX25,      /**< AX.25 Frames */
        IPEIP,     /**< IP encapsulated in IP */
        MICP,      /**< Mobile Int.ing control */
        SCCSP,     /**< Semaphore Comm. security */
        ETHERIP,   /**< Ethernet IP encapsulation */
        ENCAP,     /**< encapsulation header */
        APES,      /**< any private encr. scheme */
        GMTP,      /**< GMTP*/
        PIM,       /**< Protocol Independent Mcast */
        IPCOMP,    /**< payload compression (IPComp) */
        PGM,       /**< PGM */
        SCTP,      /**< SCTP */
        DIVERT,    /**< divert pseudo-protocol */
        RAW        /**< raw IP packet */
    };

    /**
     * @brief Construct an unspecified socket.
     * @details
     * The unspecified socket is an invalid socket without any associated
     * resource. It cannot be used as-is, but can be used as a move-target for
     * another socket.
     */
    basic_socket();

    /**
     * @brief Construct an socket.
     * @param domain [in] The communication domain.
     * @param type [in] The socket type.
     * @param protocol [in] The protocol type.
     */
    basic_socket(domain_t domain, type_t type, protocol_t protocol);

    /**
     * Move constructor.
     * @details The socket ownership is transferred.
     */
    basic_socket(basic_socket&& rhs) = default;

    /**
     * @brief Move assignment.
     * @details The socket ownership is transferred.
     */
    basic_socket& operator=(basic_socket&& rhs) = default;

    /**
     * @brief Destructor
     */
    virtual ~basic_socket();

    /**
     * The socket file-descriptor handle.
     */
    std::shared_ptr<mud::core::handle> handle();

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
     * @brief Close the socket connection.
     */
    void close();

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
                 std::shared_ptr<mud::core::handle> handle);

    /**
     * @brief The error code of a socket operation. This has to be
     * called straight after the execution of the operation!
     */
    int error() const;

private:
    /* The socket descriptor. */
    std::shared_ptr<mud::core::handle> _handle;

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
