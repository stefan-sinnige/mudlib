#ifndef _MUDLIB_IO_UDP_H_
#define _MUDLIB_IO_UDP_H_

#include <istream>
#include <memory>
#include <mud/io/ip.h>
#include <mud/io/ns.h>
#include <mud/event/event.h>
#include <mud/protocols/communicator.h>
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
         * @param address The address of the endpoint.
         * @param port The UDP port.
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
         * @param domain The communication domain.
         * @param type The socket type.
         * @param protocol The protocol type.
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
         * @param endpoint The source endpoint.
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
         * @param endpont The endpoint to write to.
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
     * @details
     * After a UDP connection has been established, the communication can be
     * set-up using the communication controller class. This allows the UDP
     * socket to trigger the communication layer whenever it detects that it
     * has received a message from the peer connection.
     *
     * This @c end_communicator does not perform any protocol specific flow
     * control  semantics, but that can be implemented through the protocol
     * communication layering.
     *
     * @see The @c protocols library.
     * The event controller class for communicating with a connected socket. It
     * used the event-loop to be notified of any incoming messages.
     */
    class MUDLIB_IO_API communicator:
        public mud::protocols::end_communicator<socket>
    {
    public:
        /**
         * Constructor.
         */
        communicator();

        /**
         * @brief Construct a communicator while moving the contents from
         * another instance.
         *
         * @param other The communicator to move from.
         */
        communicator(communicator&&);

        /**
         * @brief Initialise a communicator while moving the contents from
         * another instance.
         *
         * @param other The communicator to move from.
         * @return Reference to itself.
         */
        communicator& operator=(communicator&&);

        /**
         * Destructor. Closes the communication.
         */
        virtual ~communicator();

        /**
         * @brief Open the socket connection to start communication..
         * @param socket The socket to use in communications.
         * @throw std::system_error
         */
        void open(socket&& socket) override;

        /**
         * @brief Close the communication channel.
         */
        void close() override;

        /**
         * @brief Return the connected state.
         */
        bool connected() const;

        /**
         * @brief Get the stream object to read from the socket.
         * @return The stream object.
         */
        std::istream& istr() override;

        /**
         * @brief Get the stream object to write to the socket. The socket has
         * been previously been used to receive data from a peer, such that the
         * destination address matches that peer.
         * @return The stream object.
         */
        std::ostream& ostr() override;

        /**
         * @brief Get the stream object to write to the socket.
         * @param endpont The endpoint to write to.
         * @return The stream object.
         */
        std::ostream& ostr(const endpoint& endpoint);

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
         * invoke the @c receive_impulse trigger on the communicator. Any
         * layered communicator on a higher protocol layer will then be
         * invoked as necessary, based on the message being received.
         *
         * @return The event.
         */
        virtual mud::event::event event() override;

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

    private:
        /** Event handler when there is data available. */
        mud::event::event::return_type on_ready_receive();

        /** The connected state */
        bool _connected;

        /** The socket used for communications. */
        udp::socket _socket;
    };

} // namespace udp

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_UDP_H_ */
