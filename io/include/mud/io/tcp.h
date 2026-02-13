#ifndef _MUDLIB_IO_TCP_H_
#define _MUDLIB_IO_TCP_H_

#include <istream>
#include <memory>
#include <mud/io/ip.h>
#include <mud/io/ns.h>
#include <mud/core/event.h>
#include <mud/core/object.h>
#include <mud/protocols/communicator.h>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>

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
        endpoint() = default;

        /**
         * @brief Constructor.
         * @param address The address of the endpoint.
         * @param port The TCP port.
         */
        endpoint(const mud::io::ip::address& address, uint16_t port);

        /**
         * @brief Copy constructor.
         */
        endpoint(const endpoint& rhs) = default;

        /**
         * @brief Assinging operator.
         */
        endpoint& operator=(const endpoint& rhs) = default;

        /**
         * @brief Destructor.
         */
        virtual ~endpoint() = default;

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
        uint16_t _port = 0;
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
         * @brief Construct an unspecified socket.
         * @details
         * The unspecified socket is an invalid socket without any associated
         * resource. It cannot be used as-is, but can be used as a move-target
         * for another socket.
         *
         * @code
         * tcp::socket socket(nullptr);
         * socket = std::move(other_socket);
         * @endcode
         */
       socket(std::nullptr_t);

        /**
         * @brief Construct a specialised socket.
         * @param domain The communication domain.
         * @param type The socket type.
         * @param protocol The protocol type.
         */
        socket(mud::io::basic_socket::domain_t domain,
               mud::io::basic_socket::type_t type,
               mud::io::basic_socket::protocol_t protocol);

        /**
         * @brief Move constructor.
         * @details The socket ownership is transferred.
         */
        socket(socket&&) = default;

        /**
         * @brief Move assignment.
         * @details The socket ownership is transferred.
         */
        socket& operator=(socket&&) = default;

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

        /**
         * Non-copyable.
         */
        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

    private:
        /**
         * Constructor to be used by the @c acceptor when creating a client
         * socket.
         */
        socket(mud::io::basic_socket::domain_t domain,
               mud::io::basic_socket::type_t type,
               mud::io::basic_socket::protocol_t protocol,
               std::shared_ptr<mud::core::handle> handle);

        /**
         * Set the source and destination end points.
         */
        void source_endpoint(const endpoint&);
        void destination_endpoint(const endpoint&);

        /**
         * Friend class to @c acceptor, @c connector and @c communicator in
         * order to access private functionality.
         */
        friend class mud::io::tcp::acceptor;
        friend class mud::io::tcp::connector;
        friend class mud::io::tcp::communicator;

        /** Platform specific implementation.  */
        class impl;
        struct impl_deleter
        {
            void operator()(impl*) const;
        };
        std::unique_ptr<impl, impl_deleter> _impl;
    };

    /**
     * @brief Controller for accepting inbound TCP connections.
     *
     * The event controller class for opening a listening socket on a local
     * endpoint. When the listening socket registers an incoming connection
     * from a peer, the event-loop invokes the @c accepted notification,
     * while passing the client socket ownership. Continues listening to
     * other incoming connection requests until destructed.
     */
    class MUDLIB_IO_API acceptor : public mud::core::object
    {
    public:
        /**
         * Constructor.
         */
        acceptor();

        /**
         * Destructor.
         */
        virtual ~acceptor();

        /**
         * @brief Open the socket connection to start listening.
         * @param endpoint The end-point to listen on.
         * @throw std::system_error
         */
        void open(const endpoint& endpoint);

        /**
         * @brief Close the socket that is listening for new connections.
         */
        void close();

        /** 
         * @brief Return an accepted connection.
         * @return The socket asscociated to an accepted onnection, or an
         * invalid socket (the associated handle is a @c nullptr) if there are
         * no connections available.
         */
        tcp::socket connection();

        /**
         * @brief The notification topic when a connection has been accepted.
         * @details
         * When a notification has been triggered that the @c accepted has
         * accepted a connection, a subscriber to that notfication can invoke
         * the @c connection function to extract the connected socket.
         * @return The notification ID.
         */
        const mud::core::uuid& accepted() const {
            return _accepted;
        }

        /**
         * @brief Return the event that should be actioned when the @c Device is
         * being signalled for an accepted connection.
         *
         * @details
         * The event that is returned is to be used by an @c event_loop. The
         * @c acceptor will publish the @c accepted notification when handling
         * the event and to notify any attached object of the accepted
         * connection.
         *
         * @return The event.
         */
        mud::core::event& event();

        /**
         * Non-copyable.
         */
        acceptor(const acceptor&) = delete;
        acceptor& operator=(const acceptor&) = delete;

        /*
         * Non-moveable
         */
        acceptor(acceptor&& rhs) = delete;
        acceptor& operator=(acceptor&& rhs) = delete;

    private:
        /** Event handler when a peer is connected. */
        void on_ready_accept(const mud::core::message&);

        /** The connected state of the listening socket */
        bool _connected;

        /** The socket used for listening for incoming connections. */
        tcp::socket _listen;

        /** The accept event. */
        mud::core::event _accept_event;

        /** The accepted notification. */
        mud::core::uuid _accepted;

        /** The mutex to protect the access to the queue. */
        std::mutex  _mutex;

        /** The queue of accepted connection that are ready to be returned. */
        std::queue<tcp::socket> _pending;
    };

    /**
     * @brief Controller for creating outbound TCP connections.
     *
     * The event controller class for connecting to a peer a socket to a remote
     * endpoint. When the connection has been establisged to a peer, the
     * event-loop invokes the @c on_connect handler, while passing the
     * ownership.
     */
    class MUDLIB_IO_API connector: public mud::core::object
    {
    public:
        /**
         * Constructor.
         */
        connector();

        /**
         * @brief Move constructor.
         */
        connector(connector&& rhs) = default;

        /**
         * @brief Move assignment.
         */
        connector& operator=(connector&&) = default;

        /**
         * Destructor.
         */
        virtual ~connector();

        /**
         * @brief Open the socket connection to initiate a connection request.
         * @param endpoint The end-point to connect to.
         * @throw std::system_error if the connection cannot be attempted
         * @details This connection will be attempted in the background. Once
         * a connection has been established, it shall invoke the @c connected
         * notification. The connected socket is accessible through the @c
         * connection method.
         */
        void open(const endpoint& endpoint);

        /**
         * @brief Get the socket that has established the connection.
         * @return The connected socket.
         */
        tcp::socket& connection() {
            return _socket;
        }

        /**
         * @brief The notification topic when a connection has been established.
         * @return The notification ID.
         */
        const mud::core::uuid& connected() const {
            return _connected;
        }

        /**
         * @brief Return the event that should be actioned when the @c Device is
         * being signalled that a connection has been established.
         *
         * @details
         * The event that is returned is to be used by an @c event_loop. The
         * @c connector will publish on the @c connected notification  when
         * handling the event and to notify any attached object of the
         * established connection.
         *
         * @return The event.
         */
        mud::core::event& event();

        /**
         * Non-copyable.
         */
        connector(const connector&) = delete;
        connector& operator=(const connector&) = delete;

    private:
        /** Event handler when a peer has accepted the connection. */
        void on_ready_connect(const mud::core::message&);

        /** The socket to use for accepting connections. */
        tcp::socket _socket;

        /** The connect event. */
        mud::core::event _connect_event;

        /** The connected notification. */
        mud::core::uuid _connected;
    };

    /**
     * @brief Controller for communicating TCP connections.
     *
     * @details
     * After a TCP connection has been established (either on the server side
     * through the @c acceptor or or the client side through the @c connector
     * controllers), the communication can be set-up using the communication
     * controller class. This allows the TCP socket to trigger the communication
     * layer whenever it detects that it has received a message from the peer
     * connection.
     *
     * This @c end_communicator does not perform any protocol specific flow
     * control  semantics, but that can be implemented through the protocol
     * communication layering.
     *
     * @see The @c protocols library.
     */
    class MUDLIB_IO_API communicator:
        public mud::protocols::end_communicator<socket>
    {
    public:
        /**
         * @brief Constructor.
         */
        communicator();

        /**
         * @brief Construct a communicator while moving the contents from
         * another instance.
         *
         * @param other The communicator to move from.
         */
        communicator(communicator&&) = default;

        /**
         * @brief Initialise a communicator while moving the contents from
         * another instance.
         *
         * @param other The communicator to move from.
         * @return Reference to itself.
         */
        communicator& operator=(communicator&&) = default;

        /**
         * @brief Destructor. Closes the communication.
         */
        virtual ~communicator();

        /**
         * @brief Open the socket connection to start communication..
         * @param socket The socket to usen communications.
         * @throw std::system_error
         */
        void open(socket&& socket) override;

        /**
         * @brief Close the communication channel.
         */
        void close() override;

        /**
         * @brief Get the stream object to read from the socket.
         * @return The stream object.
         */
        std::istream& istr() override;

        /**
         * @brief Get the stream object to write to the socket.
         * @return The stream object.
         */
        std::ostream& ostr() override;

        /**
         * @brief Return the associated socket device.
         *
         * @details
         * Return the associated socket device, but this is only relevant after
         * the communicator has been opened.
         *
         * @return Return the socket device.
         */
        socket& device() override;

        /**
         * @brief Return the event that should be actioned when the @c Device is
         * being signalled.
         *
         * @details
         * The event that is returned is to be used by an @c event_loop to
         * invoke the @c received ntoficiation on the communicator. Any
         * layered communicator on a higher protocol layer will then be
         * invoked as necessary, based on the message being received.
         *
         * @return The event.
         */
        virtual mud::core::event& event() override;

    private:
        /** Event handler when there is data available. */
        void on_ready_receive(const mud::core::message&);

        /** The receive event. */
        mud::core::event _receive_event;

        /** The connected state */
        bool _connected = false;

        /** The socket used for communications. */
        tcp::socket _socket;
    };

} // namespace tcp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_TCP_H_ */
