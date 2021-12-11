#ifndef _MUDLIB_IO_UDP_H_
#define _MUDLIB_IO_UDP_H_

#include <istream>
#include <memory>
#include <mud/event/event_loop.h>
#include <mud/io/ip.h>
#include <mud/io/ns.h>
#include <ostream>
#include <string>

BEGIN_MUDLIB_IO_NS

/**
 * The User Datagram  Protocol (UDP).
 */

namespace udp {

    /**
     * @brief The definition of a UDP endpoint.
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
    class MUDLIB_IO_API socket : public mud::io::ip::socket
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
        /** Platform specific implementation.  */
        class impl;
        struct impl_deleter
        {
            void operator()(impl*) const;
        };
        std::unique_ptr<impl, impl_deleter> _impl;
    };

    /**
     * @brief Controller for communicating UDP connections.
     *
     * The event controller class for communicating with a connected socket. It
     * used the event-loop to be notified of any incoming messages.
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
        communicator(mud::event::event_loop& event_loop =
                         mud::event::event_loop::global());

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
        udp::socket _socket;

        /** The event-loop. */
        mud::event::event_loop& _event_loop;

        /** The on_receive handler. */
        on_receive_func _on_receive_func;
    };

} // namespace udp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_UDP_H_ */
