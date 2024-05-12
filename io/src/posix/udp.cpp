#if defined(_WIN32)
typedef short sa_family_t;
    #define RECVFROM_CAST (char*)
    #define SENDTO_CAST (const char*)
    #include <io.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #define RECVFROM_CAST (void*)
    #define SENDTO_CAST (const void*)
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif
#include <string.h>
#include "mud/io/exception.h"
#include "mud/io/ip.h"
#include "mud/io/streambuf.h"
#include "mud/io/udp.h"

using namespace std::placeholders;

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with sockets. The read and write operations
 * to be used are the POSIX 'sendto' and 'recvfrom'.
 */

namespace _udp {

    class streambuf : public mud::io::basic_streambuf
    {
    public:
        /* Constructor for UDP specific stream-buffer.
         * @param [in] socket  The associated socket.
         * @param [in] handle  The handle to use.
         * @param [in] source_endpoint  The handle to use.
         * @param [in] bufsize The initial buffer size.
         * @param [in] putbacksize The size of the putback buffer.
         */
        streambuf(udp::socket& socket,
                  const std::unique_ptr<mud::core::handle>& handle,
                  udp::endpoint& source_endpoint,
                  udp::endpoint& destination_endpoint, size_t bufsize = 10,
                  size_t putbacksize = 4);

        /* UDP specific read and write functions. */
        ssize_t read(void* buffer, size_t count) override;
        ssize_t write(const void* buffer, size_t count) override;

    private:
        /* The reference to the socket. */
        udp::socket& _socket;

        /* The associated source endpoint */
        udp::endpoint& _source_endpoint;

        /* The associated destination endpoint */
        udp::endpoint& _destination_endpoint;
    };

    streambuf::streambuf(udp::socket& socket,
                         const std::unique_ptr<mud::core::handle>& handle,
                         udp::endpoint& source_endpoint,
                         udp::endpoint& destination_endpoint, size_t bufsize,
                         size_t putbacksize)
      : mud::io::basic_streambuf(handle, bufsize, putbacksize), _socket(socket),
        _source_endpoint(source_endpoint),
        _destination_endpoint(destination_endpoint)
    {}

    ssize_t streambuf::read(void* buf, size_t count)
    {
        // Initialise the socket address to hold the peer address.
        struct sockaddr_in addr;
        ::memset(&addr, 0, sizeof(sockaddr_in));
        socklen_t addr_sz = sizeof(addr);

        // Receive.
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nread = ::recvfrom(hndl, RECVFROM_CAST buf, count, 0,
                                   (struct sockaddr*)&addr, &addr_sz);
        if (nread >= 0) {
            // Establish the destination endpoint.
            udp::endpoint destination(addr.sin_addr.s_addr,
                                      ntohs(addr.sin_port));
            _destination_endpoint = destination;
        }
        return nread;
    }

    ssize_t streambuf::write(const void* buf, size_t count)
    {
        // Initialise the socket address to send to.
        struct sockaddr_in addr;
        ::memset(&addr, 0, sizeof(sockaddr_in));
        addr.sin_family = static_cast<sa_family_t>(_socket.domain());
        addr.sin_port = htons(_destination_endpoint.port());
        addr.sin_addr.s_addr = _destination_endpoint.address();

        // Send.
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nwrite = ::sendto(hndl, SENDTO_CAST buf, count, 0,
                                  (struct sockaddr*)&addr, sizeof(addr));
        if (nwrite >= 0) {
            // Establish the source endpoint.
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            ::memset(&addr, 0, sizeof(sockaddr_in));
            if (::getsockname(hndl, (struct sockaddr*)&addr, &len) != 0) {
                throw std::system_error(
                    errno, std::system_category(),
                    "retrieving UDP local endpoint details");
            }
            udp::endpoint source(addr.sin_addr.s_addr, ntohs(addr.sin_port));
            _source_endpoint = source;
        }
        return nwrite;
    }

} // namespace _udp

/**
 * @brief Implementation class for POSIX compliant @c UDP socket.
 */

class udp::socket::impl
{
public:
    /**
     * Constructor.
     */
    impl(udp::socket& socket, const std::unique_ptr<mud::core::handle>&);

    /**
     * Destructor.
     */
    ~impl();

    /**
     * Bind to an endpoint.
     */
    void bind(const endpoint& endpoint);

    /**
     * The source endpoint.
     */
    const endpoint& source_endpoint() const;

    /**
     * The destination endpoint.
     */
    const endpoint& destination_endpoint() const;

    /**
     * The stream for reading.
     */
    std::istream& istr();

    /**
     * The stream for writing, using the endpoint from the read-buffer as
     * destination address.
     */
    std::ostream& ostr();

    /**
     * The stream for writing to a specified endpoint.
     */
    std::ostream& ostr(const endpoint& endpoint);

private:
    /* The reference to the socket. */
    udp::socket& _socket;

    /** Reference to the socket handle. */
    const std::unique_ptr<mud::core::handle>& _handle;

    /** The stream for reading. */
    std::istream _istr;

    /** The stream for writing. */
    std::ostream _ostr;

    /** The stream buffer for reading */
    std::unique_ptr<_udp::streambuf> _read_buffer;

    /** The stream buffer for writing */
    std::unique_ptr<_udp::streambuf> _write_buffer;

    /** The source (local peer) endpoint details. */
    endpoint _source_endpoint;

    /** The destination (remote peer) endpoint details. */
    endpoint _destination_endpoint;
};

udp::socket::impl::impl(udp::socket& socket,
                        const std::unique_ptr<mud::core::handle>& handle)
  : _socket(socket), _handle(handle), _istr(nullptr), _ostr(nullptr)
{
    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer = std::unique_ptr<_udp::streambuf>(new _udp::streambuf(
        _socket, _handle, _source_endpoint, _destination_endpoint, 4096, 16));
    _write_buffer = std::unique_ptr<_udp::streambuf>(new _udp::streambuf(
        _socket, _handle, _source_endpoint, _destination_endpoint, 4096, 16));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());

    /* Set-up to reuse address to eliminate TIME_WAIT conditions. */
    _socket.option<bool, mud::io::ip::reuse_address>(true);
}

udp::socket::impl::~impl() {}

void
udp::socket::impl::bind(const endpoint& endpoint)
{
    /* Bind the socket to the endpoint. */
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = static_cast<sa_family_t>(_socket.domain());
    addr.sin_port = htons(endpoint.port());
    addr.sin_addr.s_addr = endpoint.address();
    int sckt = mud::core::internal_handle<int>(_socket.handle());
    if (::bind(sckt, (struct sockaddr*)&addr, len) != 0) {
        throw std::system_error(errno, std::system_category(),
                                "binding UDP endpoint");
    }

    /* Establish the source endpoint details */
    if (::getsockname(sckt, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(errno, std::system_category(),
                                "retrieving TCP local endpoint details");
    }
    udp::endpoint local_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    _source_endpoint = local_endpoint;
}

const udp::endpoint&
udp::socket::impl::source_endpoint() const
{
    return _source_endpoint;
}

const udp::endpoint&
udp::socket::impl::destination_endpoint() const
{
    return _destination_endpoint;
}

std::istream&
udp::socket::impl::istr()
{
    return _istr;
}

std::ostream&
udp::socket::impl::ostr()
{
    return _ostr;
}

std::ostream&
udp::socket::impl::ostr(const endpoint& endpoint)
{
    _destination_endpoint = endpoint;
    return _ostr;
}

void
udp::socket::impl_deleter::operator()(udp::socket::impl* ptr) const
{
    delete ptr;
}

/** The endpoint */

udp::endpoint::endpoint() : _address(), _port(0) {}

udp::endpoint::endpoint(const mud::io::ip::address& address, uint16_t port)
  : _address(address), _port(port)
{}

udp::endpoint::endpoint(const endpoint& rhs)
{
    operator=(rhs);
}

udp::endpoint&
udp::endpoint::operator=(const endpoint& rhs)
{
    if (this != &rhs) {
        _address = rhs._address;
        _port = rhs._port;
    }
    return *this;
}

udp::endpoint::~endpoint() {}

const mud::io::ip::address&
udp::endpoint::address() const
{
    return _address;
}

uint16_t
udp::endpoint::port() const
{
    return _port;
}

/** The explicit specialisation of POSIX sockets. */

udp::socket::socket()
  : ip::socket(basic_socket::domain_t::INET, basic_socket::type_t::DGRAM,
               basic_socket::protocol_t::INTRINSIC)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

udp::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                    basic_socket::protocol_t protocol)
  : ip::socket(domain, type, protocol)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

udp::socket::socket(socket&& rhs) : ip::socket(std::move(rhs))
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

udp::socket&
udp::socket::operator=(udp::socket&& rhs)
{
    if (this != &rhs) {
        ip::socket::operator=(std::move(rhs));
        _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
    }
    return *this;
}

udp::socket::~socket() {}

void
udp::socket::bind(const endpoint& endpoint)
{
    _impl->bind(endpoint);
}

const udp::endpoint&
udp::socket::source_endpoint() const
{
    return _impl->source_endpoint();
}

const udp::endpoint&
udp::socket::destination_endpoint() const
{
    return _impl->destination_endpoint();
}

std::istream&
udp::socket::istr()
{
    return _impl->istr();
}

std::ostream&
udp::socket::ostr()
{
    return _impl->ostr();
}

std::ostream&
udp::socket::ostr(const endpoint& endpoint)
{
    return _impl->ostr(endpoint);
}

/** The communicator */

udp::communicator::communicator(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _on_receive_func(nullptr)
{}

udp::communicator&
udp::communicator::operator=(communicator&& rhs)
{
    if (&rhs != this) {
        _socket = std::move(rhs._socket);
        _on_receive_func = rhs._on_receive_func;
    }
    return *this;
}

udp::communicator::~communicator()
{
    close();
}

void
udp::communicator::open(udp::socket&& socket)
{
    _socket = std::move(socket);
    _event_loop.register_handler(mud::event::event(
        _socket.handle(), mud::event::event::signal_type::READING,
        std::bind(&communicator::on_ready_receive, this)));
}

void
udp::communicator::close()
{
    _event_loop.deregister_handler(mud::event::event(_socket.handle()));
    _socket.close();
}

std::ostream&
udp::communicator::ostr()
{
    return _socket.ostr();
}

std::ostream&
udp::communicator::ostr(const endpoint& endpoint)
{
    return _socket.ostr(endpoint);
}

std::istream&
udp::communicator::istr()
{
    return _socket.istr();
}

const udp::endpoint&
udp::communicator::source_endpoint() const
{
    return _socket.source_endpoint();
}

const udp::endpoint&
udp::communicator::destination_endpoint() const
{
    return _socket.destination_endpoint();
}

void
udp::communicator::on_receive(on_receive_func func)
{
    _on_receive_func = func;
}

mud::event::event::return_type
udp::communicator::on_ready_receive()
{
    // Call the registered function handler.
    if (_on_receive_func != nullptr) {
        _on_receive_func();
    }

    // Continue receiving. while the socket is still open
    if (_socket.handle() != nullptr) {
        return mud::event::event::return_type::CONTINUE;
    } else {
        return mud::event::event::return_type::REMOVE;
    }
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
