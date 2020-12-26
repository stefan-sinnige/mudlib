#ifndef _MUDLIB_IO_TCP_H_
#define _MUDLIB_IO_TCP_H_

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <netinet/in.h>
#include <mud/io/ns.h>
#include <mud/io/ip.h>
#include <mud/io/kernel_event_loop.h>

BEGIN_MUDLIB_IO_NS

/**
 * The Transmission Control Protocol (TCP).
 */

namespace tcp {

/* Forward declarations. */
class acceptor;
class connector;

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
     * @brief Move assignment.
     */
    socket& operator=(socket&&);

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
    /**
     * Constructor to be used by the @c acceptor when creating a client socket.
     */
    socket(mud::io::basic_socket::domain_t domain,
            mud::io::basic_socket::type_t type,
            mud::io::basic_socket::protocol_t protocol,
            std::unique_ptr<mud::io::kernel_handle> handle);

    /**
     * Set the source and destination end points.
     */
    void source_endpoint(const endpoint&);
    void destination_endpoint(const endpoint&);

    /**
     * Friend class to @c acceptor and @connector in order to access private
     * functionality.
     */
    friend class mud::io::tcp::acceptor;
    friend class mud::io::tcp::connector;

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
    /** Function definition for the @c on_accept handler. */
    typedef std::function<void(socket&&)> on_accept_func;

    /**
     * Constructor.
     * @param event_loop [in] the event-loop to register the socket to.
     */
    acceptor(mud::io::kernel_event_loop& event_loop
            = mud::io::kernel_event_loop::global());

    /**
     * Destructor.
     */
    virtual ~acceptor();

    /**
     * @brief Open the socket connection to start listening.
     * @param endpoint [in] The end-point to listen on.
     * @throw std::system_error
     */
    void open(const endpoint& endpoint);

    /**
     * @brief Register a handler when a connection has been accepted.
     * @param func [in] The handler function.
     */
    void on_accept(on_accept_func func);

private:
    /** Event handler when a peer is connected. */
    void on_ready_accept();

    /** The socket used for listening for incoming connections. */
    tcp::socket _listen;

    /** The event-loop. */
    mud::io::kernel_event_loop& _event_loop;

    /** The on_accept handler. */
    on_accept_func _on_accept_func;
};

/**
 * @brief Controller for creating outbound TCP connections.
 */

class connector
{
public:
    /** Function definition for the @c on_connect handler. */
    typedef std::function<void(socket&&)> on_connect_func;

    /**
     * Constructor.
     * @param event_loop [in] the event-loop to register the socket to.
     */
    connector(mud::io::kernel_event_loop& event_loop
            = mud::io::kernel_event_loop::global());

    /**
     * Destructor.
     */
    virtual ~connector();

    /**
     * @brief Open he socket connection to initiate a connection request.
     * @param endpoint [in] The end-point to connect to.
     * @throw std::system_error
     */
    void open(const endpoint& endpoint);

    /**
     * @brief Register a handler when a connection has been made.
     * @param func [in] The handler function.
     */
    void on_connect(on_connect_func func);

private:
    /** Event handler when a peer has accepted the connection. */
    void on_ready_connect();

    /** The socket to use for accepting connections. */
    tcp::socket _socket;

    /** The event-loop. */
    mud::io::kernel_event_loop& _event_loop;

    /** The on_connect handler. */
    on_connect_func _on_connect_func;
};

} // namespace tcp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_TCP_H_ */

