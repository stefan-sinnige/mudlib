#ifndef _MUDLIB_IO_IP_H_
#define _MUDLIB_IO_IP_H_

#include <inttypes.h>
#include <mud/core/endian.h>
#include <mud/io/ns.h>
#include <mud/io/socket.h>
#include <string>

BEGIN_MUDLIB_IO_NS

/**
 * The Internet Protocol Layer (IPv4).
 */

namespace ip {

    /**
     * @brief The endianness of the Internet Protocol Suite, generally known as
     * the network order.
     */
    const mud::core::endian::endian_t network_order =
        mud::core::endian::endian_t::big;

    /**
     * @brief Convert from host (native) order to network order.
     */
    template<typename Integer>
    Integer to_network_order(Integer value)
    {
        return mud::core::endian::convert(mud::core::endian::native(),
                                          mud::io::ip::network_order, value);
    }

    /**
     * @brief Convert from network order to host (native) order.
     */
    template<typename Integer>
    Integer to_host_order(Integer value)
    {
        return mud::core::endian::convert(mud::io::ip::network_order,
                                          mud::core::endian::native(), value);
    }

    /**
     * @brief The IPv4 address representation.
     *
     * The IPv4 address is a 32-bit structure to represent a network class or a
     * host identifier.
     */
    class MUDLIB_IO_API address
    {
    public:
        /**
         * Structure t specify hints when performing a host name lookup.
         */
        struct hints {
            mud::io::basic_socket::domain_t domain;
            mud::io::basic_socket::type_t type;
            mud::io::basic_socket::protocol_t protocol;
        };

        /**
         * @brief Construct an any-address (0.0.0.0)
         */
        address();

        /**
         * @brief Address specified by 32-bits address.
         * @param addr [in] The 32-bit address (network order).
         */
        address(uint32_t addr);

        /**
         * @brief Address specified by the dotted-decimal notation or host name.
         * @param node [in] The address as a dotted-decimal or a host name.
         * @param criteria [in] Any hints to specify the criteria to lookup host
         * names.
         * If the address is passed as a host-name, a host dependent lookup
         * (e.g. DNS) will occur. The first matched entry will be used.
         */
        address(const std::string& node, const hints& criteria  = {
            mud::io::basic_socket::domain_t::UNSPEC,
            mud::io::basic_socket::type_t::UNSPEC,
            mud::io::basic_socket::protocol_t::UNSPEC});

        /**
         * @brief Copy constructor
         */
        address(const address& rhs);

        /**
         * @brief Destructor
         */
        virtual ~address();

        /**
         * @brief Assignment operator
         */
        address& operator=(const address& rhs);

        /**
         * @brief Type conversion to a network-order address.
         */
        operator uint32_t() const;

        /**
         * @brief Return as a dotted-decimal notation.
         * @return The dotted-decimal notation of the IP address.
         */
        std::string str() const;

    private:
        /* The IP address (network-order) */
        uint32_t m_address;
    };

    /**
     * @brief The IP socket.
     */
    class MUDLIB_IO_API socket : public mud::io::basic_socket
    {
    protected:
        /**
         * @brief Construct an socket.
         * @param domain [in] The communication domain.
         * @param type [in] The socket type.
         * @param protocol [in] The protocol type.
         */
        socket(mud::io::basic_socket::domain_t domain,
               mud::io::basic_socket::type_t type,
               mud::io::basic_socket::protocol_t protocol);

        /**
         * @brief Construct an socket.
         * @param domain [in] The communication domain.
         * @param type [in] The socket type.
         * @param protocol [in] The protocol type.
         * @param handle [in] The handle of an existing socket.
         */
        socket(mud::io::basic_socket::domain_t domain,
               mud::io::basic_socket::type_t type,
               mud::io::basic_socket::protocol_t protocol,
               std::unique_ptr<mud::core::handle> handle);

        /**
         * @brief Move constructor, passing ownership of the socket.
         */
        socket(socket&& rhs);

        /**
         * @brief Move assignment, passing ownership of the socket.
         */
        socket& operator=(socket&& rhs);

        /**
         * @brief Destructor.
         */
        virtual ~socket();

        /**
         * Non-copyable.
         */
        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

        /**
         * @brief Get the source address of the socket connection.
         * @return The source address (local peer) bound to the socket.
         */
        const address& source_address() const;

        /**
         * @brief Get the destination address of the socket connection.
         * @return The destination address (remote peer) of an established
         * connection.
         */
        const address& destination_address() const;

        /**
         * @brief Set a socket option.
         * @tparam Type The type of the option value.
         * @tparam Option The class defining the option to set.
         * @param value The value to set.
         * @return The reference to this object
         * @throw std::system_error
         */
        template<typename Type, typename Option>
        socket& option(Type value)
        {
            Option option;
            option(*this, value);
            return *this;
        }

        /**
         * @brief Get a socket option.
         * @tparam Type The type of the option value.
         * @tparam Option The class defining the option to get.
         * @return The value retrieved.
         * @throw std::system_error
         */
        template<typename Type, typename Option>
        Type option()
        {
            Option option;
            return option(*this);
        }

    protected:
        /**
         * Set the source address of the socket connection.
         * @param peer [in] The source address (local peer).
         */
        virtual void source_address(const address& peer);

        /**
         * Set the destination address of the socket connection.
         * @param peer [in] The source address (remote peer).
         */
        virtual void destination_address(const address& peer);

    private:
        /** The source address (local peer). */
        address _source_address;

        /** The destination address (remote peer). */
        address _destination_address;
    };

    /**
     * @brief Socket option to allow the reuse of local addresses when binding a
     * socket to a valid address.
     */
    class MUDLIB_IO_API reuse_address
    {
    public:
        /** @brief Set the option to reuse socket addresses.
         *  @param value Flag to indicate to reuse socket addresses.
         */
        void operator()(socket&, bool value);

        /**
         * @brief Retrieve the setting of reusing socket addresses.
         * @return true if addressed can be reused.
         */
        bool operator()(socket&);
    };

    /**
     * @brief Socket option to allow the socket not to block on a socket method.
     * Particularly useful when used in combination with @c event_loop
     * non-blocking I/O.
     */
    class MUDLIB_IO_API nonblocking
    {
    public:
        /** @brief Set the option to reuse socket addresses.
         *  @param value Flag to indicate to reuse socket addresses.
         */
        void operator()(socket&, bool value);

        /**
         * @brief Retrieve the setting of reusing socket addresses.
         * @return true if addressed can be reused.
         */
        bool operator()(socket&);
    };

} // namespace ip

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_IP_H_ */
