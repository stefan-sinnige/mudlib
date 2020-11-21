#ifndef _MUDLIB_IO_TCP_H_
#define _MUDLIB_IO_TCP_H_

#include <memory>
#include <string>
#include <netinet/in.h>
#include <mud/io/ns.h>
#include <mud/io/ip.h>

BEGIN_MUDLIB_IO_NS

/**
 * The Transmission Control Protocol (TCP).
 */

namespace tcp {

/* Forward declarations. */
class acceptor;

/**
 * @brief The definition of a TCP endpoint.
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
     * @param port [in] The TCP port.
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
     * @brief The TCP port of the endpoint.
     */
    uint16_t port() const;

private:
    /** The address */
    mud::io::ip::address _address;

    /** The port */
    uint16_t _port;
};

/**
 * @brief The TCP socket.
 */
class socket : public mud::io::ip::socket
{
public:
    /**
     * @brief Constructor to create a standard TCP protocol.
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
     * Destructor.
     */
    virtual ~socket();

    /**
     * @brief Get the stream object to read from the socket.
     * @return The stream object.
     */
    std::istream& istr();

    /**
     * @brief Get the stream object to write to the socket.
     * @return The stream object.
     */
    std::ostream& ostr();

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
    /**
     * Constructor to be used by the @c acceptor when creating a client socket.
     */
    socket(mud::io::basic_socket::domain_t domain,
            mud::io::basic_socket::type_t type,
            mud::io::basic_socket::protocol_t protocol,
            std::unique_ptr<mud::io::kernel_handle> handle);

    /**
     * Friend class to @c acceptor in order to access private constructor.
     */
    friend class mud::io::tcp::acceptor;

    /** Platform specific implementation.  */
    class impl;
    std::unique_ptr<impl> _impl;
};

/**
 * @brief Controller for accepting inbound TCP connections.
 */

class acceptor
{
public:
    /**
     * Constructor.
     * @param socket [in] the socket to be used for accepting connections.
     */
    acceptor(mud::io::tcp::socket& socket);

    /**
     * Destructor.
     */
    virtual ~acceptor();

    /**
     * @brief Open the socket connection to start listening.
     * @throw std::system_error
     */
    void open(const endpoint& endpoint);

    /**
     * @brief Accept a connection.
     * @return The connected client.
     * @throw std::system_error
     *
     * The method may block if a connection is not ready to be accepted.
     */
    socket accept();

private:
    /** The socket to use for accepting connections. */
    tcp::socket& _socket;
};

/**
 * @brief Controller for creating outbound TCP connections.
 */

class connector
{
public:
    /**
     * Constructor.
     * @param socket [in] the socket to be used for establishing a connection.
     */
    connector(mud::io::tcp::socket& socket);

    /**
     * Destructor.
     */
    virtual ~connector();

    /**
     * @brief Connect to an endpoint.
     * @throw std::system_error
     */
    void connect(const endpoint& endpoint);

private:
    /** The socket to use for accepting connections. */
    tcp::socket& _socket;
};

} // namespace tcp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_TCP_H_ */

