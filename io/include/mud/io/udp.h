#ifndef _MUDLIB_IO_UDP_H_
#define _MUDLIB_IO_UDP_H_

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <netinet/in.h>
#include <mud/io/ns.h>
#include <mud/io/ip.h>

BEGIN_MUDLIB_IO_NS

/**
 * The User Datagram  Protocol (UDP).
 */

namespace udp {

/**
 * @brief The definition of a UDP endpoint.
 */
class endpoint
{
public:
    /**
     * @brief Default constructor.
     */
    endpoint();

    /**
     * @brief Constructor.
     * @param address [in] The address of the endpoint.
     * @param port [in] The UDP port.
     */
    endpoint(const mud::io::ip::address& address, uint16_t port);

    /**
     * @brief Copy constructor.
     */
    endpoint(const endpoint& rhs);

    /**
     * @brief Assinging operator.
     */
    endpoint& operator=(const endpoint& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~endpoint();

    /**
     * @brief The address of the endpoint.
     */
    const mud::io::ip::address& address() const;

    /**
     * @brief The UDP port of the endpoint.
     */
    uint16_t port() const;

private:
    /** The address */
    mud::io::ip::address _address;

    /** The port */
    uint16_t _port;
};

/**
 * @brief The UDP socket.
 */
class socket : public mud::io::ip::socket
{
public:
    /**
     * @brief Constructor to create a standard UDP protocol.
     */
    socket();

    /**
     * @brief Construct a specialised socket.
     * @param domain [in] The communication domain.
     * @param type [in] The socket type.
     * @param protocol [in] The protocol type.
     */
    socket(mud::io::basic_socket::domain_t domain,
            mud::io::basic_socket::type_t type,
            mud::io::basic_socket::protocol_t protocol);

    /**
     * @brief Move constructor.
     */
    socket(socket&&);

    /**
     * @brief Move assignment.
     */
    socket& operator=(socket&&);

    /**
     * Destructor.
     */
    virtual ~socket();

    /**
     * @brief Bind the socket connection to a source (local) endpoint.
     * @param endpoint [in] The source endpoint.
     * @throw std::system_error
     */
    void bind(const endpoint& endpoint);

    /**
     * @brief Get the stream object to read from the socket.
     * @return The stream object.
     */
    std::istream& istr();

    /**
     * @brief Get the stream object to write to the socket. The socket has
     * been previously been used to receive data from a peer, such that the
     * destination address matches that peer.
     * @return The stream object.
     */
    std::ostream& ostr();

    /**
     * @brief Get the stream object to write to the socket.
     * @param endpont [in] The endpoint to write to.
     * @return The stream object.
     */
    std::ostream& ostr(const endpoint& endpoint);

    /**
     * @brief Get the source endpoint of the socket connection.
     * @return The source endpoint bound to the socket.
     */
    const endpoint& source_endpoint() const;

    /**
     * @brief Get the destination endpoint of the socket connection.
     * @return The destination endpoint (remote peer) of an established
     * connection.
     */
    const endpoint& destination_endpoint() const;

    /**
     * @brief Set a socket option.
     * @tparam Type The type of the option value.
     * @tparam Option The class defining the option to set.
     * @param value The value to set.
     * @return The reference to this object
     * @throw std::system_error
     */
    template <typename Type, typename Option>
    socket&
    option(Type value) {
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
    template <typename Type, typename Option>
    Type
    option() {
        Option option;
        return option(*this);
    }

    /**
     * Non-copyable.
     */
    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;

private:
    /** Platform specific implementation.  */
    class impl;
    std::unique_ptr<impl> _impl;
};

} // namespace udp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_UDP_H_ */

