#ifndef _MUDLIB_IO_TCP_H_
#define _MUDLIB_IO_TCP_H_

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <mud/io/ns.h>
#include <mud/io/ip.h>
#include <mud/event/event_loop.h>

BEGIN_MUDLIB_IO_NS

/**
 * The Transmission Control Protocol (TCP).
 */

namespace tcp {

/* Forward declarations. */
class acceptor;
class connector;
class communicator;

/**
 * @brief The definition of a TCP endpoint.
 */
class MUDLIB_IO_API endpoint
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
class MUDLIB_IO_API socket : public mud::io::ip::socket
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
            std::unique_ptr<mud::core::handle> handle);

    /**
     * Set the source and destination end points.
     */
    void source_endpoint(const endpoint&);
    void destination_endpoint(const endpoint&);

    /**
     * Friend class to @c acceptor, @c connector and @c communicator in order
     * to access private functionality.
     */
    friend class mud::io::tcp::acceptor;
    friend class mud::io::tcp::connector;
    friend class mud::io::tcp::communicator;

    /** Platform specific implementation.  */
    class impl;
    struct impl_deleter {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

/**
 * @brief Controller for accepting inbound TCP connections.
 *
 * The event controller class for opening a listening socket on a local
 * endpoint. When the listening socket registers an incoming connection
 * from a peer, the event-loop invokes the @c on_accept handler,
 * while passing the client socket ownership. Continues listening to
 * other incoming connection requests until destructed.
 */

class MUDLIB_IO_API acceptor
{
public:
    /** Function definition for the @c on_accept handler. */
    typedef std::function<void(socket&&)> on_accept_func;

    /**
     * Constructor.
     * @param event_loop [in] the event-loop to register the socket to.
     */
    acceptor(mud::event::event_loop& event_loop
            = mud::event::event_loop::global());

    /**
     * @brief Move constructor.
     */
    acceptor(acceptor&& rhs) = default;

    /**
     * @brief Move assignment.
     */
    acceptor& operator=(acceptor&&);

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

    /**
     * Non-copyable.
     */
    acceptor(const acceptor&) = delete;
    acceptor& operator=(const acceptor&) = delete;

private:
    /** Event handler when a peer is connected. */
    mud::event::event::return_type on_ready_accept();

    /** The socket used for listening for incoming connections. */
    tcp::socket _listen;

    /** The event-loop. */
    mud::event::event_loop& _event_loop;

    /** The on_accept handler. */
    on_accept_func _on_accept_func;
};

/**
 * @brief Controller for creating outbound TCP connections.
 *
 * The event controller class for connecting to a peer a socket to a remote
 * endpoint. When the connection has been establisged to a peer, the
 * event-loop invokes the @c on_connect handler, while passing the
 * ownership.
 */

class MUDLIB_IO_API connector
{
public:
    /** Function definition for the @c on_connect handler. */
    typedef std::function<void(socket&&)> on_connect_func;

    /**
     * Constructor.
     * @param event_loop [in] the event-loop to register the socket to.
     */
    connector(mud::event::event_loop& event_loop
            = mud::event::event_loop::global());

    /**
     * @brief Move constructor.
     */
    connector(connector&& rhs) = default;

    /**
     * @brief Move assignment.
     */
    connector& operator=(connector&&);

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

    /**
     * Non-copyable.
     */
    connector(const connector&) = delete;
    connector& operator=(const connector&) = delete;

private:
    /** Event handler when a peer has accepted the connection. */
    mud::event::event::return_type on_ready_connect();

    /** The socket to use for accepting connections. */
    tcp::socket _socket;

    /** The event-loop. */
    mud::event::event_loop& _event_loop;

    /** The on_connect handler. */
    on_connect_func _on_connect_func;
};

/**
 * @brief Controller for communicating TCP connections.
 *
 * The event controller class for communicating with a connected socket. It
 * uses the event-loop to be notified of any incoming messages.
 */

class MUDLIB_IO_API communicator
{
public:
    /** Function definition for the @c on_receive handler. */
    typedef std::function<void()> on_receive_func;

    /**
     * Constructor.
     * @param event_loop [in] The event-loop to register the socket to.
     */
    communicator(
            mud::event::event_loop& event_loop
            = mud::event::event_loop::global());

    /**
     * @brief Move constructor.
     */
    communicator(communicator&& rhs) = default;

    /**
     * @brief Move assignment.
     */
    communicator& operator=(communicator&&);

    /**
     * Destructor. Closes the communication.
     */
    virtual ~communicator();

    /**
     * @brief Open the socket connection to start communication..
     * @param socket [in] The socket to use in communications.
     * @throw std::system_error
     */
    void open(socket&& socket);

    /**
     * @brief Close the communication channel.
     */
    void close();

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
     * @brief Register a handler when a message has been receiveed.
     * @param func [in] The handler function.
     */
    void on_receive(on_receive_func func);

    /**
     * Non-copyable.
     */
    communicator(const communicator&) = delete;
    communicator& operator=(const communicator&) = delete;

private:
    /** Event handler when there is data available. */
    mud::event::event::return_type on_ready_receive();

    /** The socket used for communications. */
    tcp::socket _socket;

    /** The event-loop. */
    mud::event::event_loop& _event_loop;

    /** The on_receive handler. */
    on_receive_func _on_receive_func;
};

} // namespace tcp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_TCP_H_ */

