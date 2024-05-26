#if defined(_WIN32)
typedef short sa_family_t;
    #define RECV_CAST (char*)
    #define SEND_CAST (const char*)
    #include <io.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #define RECV_CAST (void*)
    #define SEND_CAST (const void*)
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
#endif
#include <string.h>
#include "mud/io/exception.h"
#include "mud/io/ip.h"
#include "mud/io/streambuf.h"
#include "mud/io/tcp.h"

using namespace std::placeholders;

BEGIN_MUDLIB_IO_NS

/**
 * Converstion structures defined in the socket.cpp
 */
extern int g_domains[];
extern int g_types[];
extern int g_protocols[];

/**
 * Define the stream buffer to use with sockets. The read and write operations
 * to be used are the POSIX 'recv' and 'recv'.
 */

namespace _tcp {

    class streambuf : public mud::io::basic_streambuf
    {
    public:
        /* Constructor for TCP specific stream-buffer.
         * @param [in] socket  The reference to the socket.
         * @param [in] handle  The handle to use.
         * @param [in] bufsize The initial buffer size.
         * @param [in] putbacksize The size of the putback buffer.
         */
        streambuf(tcp::socket& socket,
                  const std::unique_ptr<mud::core::handle>& handle,
                  size_t bufsize = 10, size_t putbacksize = 4);

        /* TCP specific read and write functions. */
        ssize_t read(void* buffer, size_t count) override;
        ssize_t write(const void* buffer, size_t count) override;

    private:
        /* The reference to the socket. */
        tcp::socket& _socket;
    };

    streambuf::streambuf(tcp::socket& socket,
                         const std::unique_ptr<mud::core::handle>& handle,
                         size_t bufsize, size_t putbacksize)
      : mud::io::basic_streambuf(handle, bufsize, putbacksize), _socket(socket)
    {}

    ssize_t streambuf::read(void* buf, size_t count)
    {
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nread;
        int tries = 10;
        while ((nread = ::recv(hndl, RECV_CAST buf, count, 0)) <= 0 && tries-- > 0) {
            int error = errno;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                fd_set rdfs;
                FD_ZERO(&rdfs);
                FD_SET(hndl, &rdfs);
                struct timeval  tv;
                tv.tv_sec = 0;
                tv.tv_usec = 100 * 1000;
                if (::select(hndl+1, &rdfs, nullptr, nullptr, &tv) <= 0) {
                    return -1;
                }
                if (! FD_ISSET(hndl, &rdfs)) {
                    return -1;
                }
            }
            else {
                break;
            }
        }
        return nread;
    }

    ssize_t streambuf::write(const void* buf, size_t count)
    {
        int hndl = mud::core::internal_handle<int>(handle());
        return ::send(hndl, SEND_CAST buf, count, 0);
    }

} // namespace _tcp

/**
 * @brief Implementation class for POSIX compliant @c TCP socket.
 */

class tcp::socket::impl
{
public:
    /**
     * Constructor.
     */
    impl(tcp::socket& socket, const std::unique_ptr<mud::core::handle>&);

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The stream for reading.
     */
    std::istream& istr();

    /**
     * The stream for writing.
     */
    std::ostream& ostr();

    /**
     *  Get the endpoints of the socket connection.
     */
    endpoint& source_endpoint();

    /**
     * Get the destination endpoint of the socket connection.
     */
    endpoint& destination_endpoint();

private:
    /** Reference to the socket. */
    tcp::socket& _socket;

    /** Reference to the socket handle. */
    const std::unique_ptr<mud::core::handle>& _handle;

    /** The stream for reading. */
    std::istream _istr;

    /** The stream for writing. */
    std::ostream _ostr;

    /** The stream buffer for reading */
    std::unique_ptr<_tcp::streambuf> _read_buffer;

    /** The stream buffer for writing */
    std::unique_ptr<_tcp::streambuf> _write_buffer;

    /** The source (local peer) endpoint details. */
    endpoint _source_endpoint;

    /** The destination (remote peer) endpoint details. */
    endpoint _destination_endpoint;
};

tcp::socket::impl::impl(tcp::socket& socket,
                        const std::unique_ptr<mud::core::handle>& handle)
  : _socket(socket), _handle(handle), _istr(nullptr), _ostr(nullptr)
{
    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer = std::unique_ptr<_tcp::streambuf>(
        new _tcp::streambuf(_socket, _handle, 4096, 16));
    _write_buffer = std::unique_ptr<_tcp::streambuf>(
        new _tcp::streambuf(_socket, _handle, 4096, 16));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());
}

tcp::socket::impl::~impl() {}

std::istream&
tcp::socket::impl::istr()
{
    return _istr;
}

std::ostream&
tcp::socket::impl::ostr()
{
    return _ostr;
}

tcp::endpoint&
tcp::socket::impl::source_endpoint()
{
    return _source_endpoint;
}

tcp::endpoint&
tcp::socket::impl::destination_endpoint()
{
    return _destination_endpoint;
}

void
tcp::socket::impl_deleter::operator()(tcp::socket::impl* ptr) const
{
    delete ptr;
}

/** The endpoint */

tcp::endpoint::endpoint() : _address(), _port(0) {}

tcp::endpoint::endpoint(const mud::io::ip::address& address, uint16_t port)
  : _address(address), _port(port)
{}

tcp::endpoint::endpoint(const endpoint& rhs)
{
    operator=(rhs);
}

tcp::endpoint&
tcp::endpoint::operator=(const endpoint& rhs)
{
    if (this != &rhs) {
        _address = rhs._address;
        _port = rhs._port;
    }
    return *this;
}

tcp::endpoint::~endpoint() {}

const mud::io::ip::address&
tcp::endpoint::address() const
{
    return _address;
}

uint16_t
tcp::endpoint::port() const
{
    return _port;
}

/** The explicit specialisation of POSIX sockets. */

tcp::socket::socket()
  : ip::socket(basic_socket::domain_t::INET, basic_socket::type_t::STREAM,
               basic_socket::protocol_t::INTRINSIC)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

tcp::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                    basic_socket::protocol_t protocol)
  : ip::socket(domain, type, protocol)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

tcp::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                    basic_socket::protocol_t protocol,
                    std::unique_ptr<mud::core::handle> hndl)
  : ip::socket(domain, type, protocol, std::move(hndl))
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

tcp::socket::socket(socket&& rhs) : ip::socket(std::move(rhs)), _impl(nullptr)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
}

tcp::socket&
tcp::socket::operator=(tcp::socket&& rhs)
{
    if (this != &rhs) {
        ip::socket::operator=(std::move(rhs));
        _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, handle()));
    }
    return *this;
}

tcp::socket::~socket() {}

std::istream&
tcp::socket::istr()
{
    return _impl->istr();
}

std::ostream&
tcp::socket::ostr()
{
    return _impl->ostr();
}

const tcp::endpoint&
tcp::socket::source_endpoint() const
{
    return _impl->source_endpoint();
}

const tcp::endpoint&
tcp::socket::destination_endpoint() const
{
    return _impl->destination_endpoint();
}

void
tcp::socket::source_endpoint(const tcp::endpoint& endpoint)
{
    _impl->source_endpoint() = endpoint;
}

void
tcp::socket::destination_endpoint(const tcp::endpoint& endpoint)
{
    _impl->destination_endpoint() = endpoint;
}

/** The acceptor */

tcp::acceptor::acceptor(mud::event::event_loop& event_loop)
  : _connected(false), _event_loop(event_loop), _on_accept_func(nullptr)
{
    /* Set-up socket options to
     *   - reuse address to eliminate TIME_WAIT conditions.
     *   - enable non-blocking socket I/O */
    _listen.option<bool, mud::io::ip::reuse_address>(true);
    _listen.option<bool, mud::io::ip::nonblocking>(true);
}

tcp::acceptor&
tcp::acceptor::operator=(acceptor&& rhs)
{
    if (&rhs != this) {
        _connected = rhs._connected;
        rhs._connected = false;
        _listen = std::move(rhs._listen);
        _on_accept_func = rhs._on_accept_func;
    }
    return *this;
}

tcp::acceptor::~acceptor()
{
    close();
}

void
tcp::acceptor::open(const endpoint& endpoint)
{
    /* Bind the socket to the endpoint. */
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = static_cast<sa_family_t>(
        g_domains[static_cast<int>(_listen.domain())]);
    addr.sin_port = htons(endpoint.port());
    addr.sin_addr.s_addr = endpoint.address();
    int lstn = mud::core::internal_handle<int>(_listen.handle());
    if (::bind(lstn, (struct sockaddr*)&addr, len) != 0) {
        throw std::system_error(_listen.error(), std::system_category(),
                                "binding TCP endpoint");
    }

    /* Establish the source endpoint details */
    if (::getsockname(lstn, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(_listen.error(), std::system_category(),
                                "retrieving TCP local endpoint details");
    }
    tcp::endpoint local_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    _listen.source_endpoint(local_endpoint);

    /* Prepare to listen. */
    if (::listen(lstn, 8) != 0) {
        throw std::system_error(_listen.error(), std::system_category(),
                                "listening for TCP connections");
    }
    _connected = true;

    /* Register the event handler to be invoked when a client connects */
    _event_loop.register_handler(mud::event::event(
        _listen.handle(), mud::event::event::signal_type::READING,
        std::bind(&acceptor::on_ready_accept, this)));
}

void
tcp::acceptor::close()
{
    _connected = false;
    if (_listen.handle() != nullptr) {
        /* Deregister the listening socket from the event-loop */
        _event_loop.deregister_handler(mud::event::event(_listen.handle()));
        _listen.close();
    }
}

bool
tcp::acceptor::connected() const
{
    return _connected;
}

void
tcp::acceptor::on_accept(on_accept_func func)
{
    _on_accept_func = func;
}

mud::event::event::return_type
tcp::acceptor::on_ready_accept()
{
    // Accept the connection. This may block if there is no client ready.
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(sockaddr_in));
    int lstn = mud::core::internal_handle<int>(_listen.handle());
    int fd = ::accept(lstn, (struct sockaddr*)&addr, &len);
    if (fd < 0) {
        throw std::system_error(_listen.error(), std::system_category(),
                                "accepting TCP connection");
    }

    // Create the client socket connection.
    std::unique_ptr<mud::core::handle> handle;
    handle =
        std::unique_ptr<mud::core::handle>(new mud::core::select_handle(fd));
    tcp::socket client(_listen.domain(), _listen.type(), _listen.protocol(),
                       std::move(handle));

    // Establish the source and destination endpoint details
    int clnt = mud::core::internal_handle<int>(client.handle());
    if (::getsockname(clnt, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(client.error(), std::system_category(),
                                "retrieving TCP local endpoint details");
    }
    tcp::endpoint local_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    client.source_endpoint(local_endpoint);

    if (::getpeername(clnt, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(client.error(), std::system_category(),
                                "retrieving TCP remote endpoint details");
    }
    tcp::endpoint remote_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    client.destination_endpoint(remote_endpoint);

    // Call the registered function handler.
    if (_on_accept_func != nullptr) {
        _on_accept_func(std::move(client));
    }

    // Keep on accepting new connections. while the socket is still open
    if (_listen.handle() != nullptr) {
        return mud::event::event::return_type::CONTINUE;
    } else {
        return mud::event::event::return_type::REMOVE;
    }
}

/** The connector */

tcp::connector::connector(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _on_connect_func(nullptr)
{
    /* Set-up socket option to enable non-blocking I/O */
    _socket.option<bool, mud::io::ip::nonblocking>(true);
}

tcp::connector&
tcp::connector::operator=(connector&& rhs)
{
    if (&rhs != this) {
        _socket = std::move(rhs._socket);
        _on_connect_func = rhs._on_connect_func;
    }
    return *this;
}

tcp::connector::~connector() {}

void
tcp::connector::open(const endpoint& endpoint)
{
    /* Connect to the endpoint. */
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = static_cast<sa_family_t>(
        g_domains[static_cast<int>(_socket.domain())]);
    addr.sin_port = htons(endpoint.port());
    addr.sin_addr.s_addr = endpoint.address();
    int sckt = mud::core::internal_handle<int>(_socket.handle());
    if (::connect(sckt, (struct sockaddr*)&addr, len) != 0) {
#if defined(_WIN32)
        if (_socket.error() != WSAEWOULDBLOCK) {
            throw std::system_error(_socket.error(), std::system_category(),
                                    "connecting TCP endpoint");
        }
#else
        if (_socket.error() != EINPROGRESS) {
            throw std::system_error(_socket.error(), std::system_category(),
                                    "connecting TCP endpoint");
        }
#endif
    }

    /* Register the event handler to be invoked when a connection has been
     * established. */
    _event_loop.register_handler(mud::event::event(
        _socket.handle(), mud::event::event::signal_type::WRITING,
        std::bind(&connector::on_ready_connect, this)));
}

void
tcp::connector::on_connect(on_connect_func func)
{
    _on_connect_func = func;
}

mud::event::event::return_type
tcp::connector::on_ready_connect()
{
    /* Deregister the connecting socket from the event-loop */
    _event_loop.deregister_handler(mud::event::event(_socket.handle()));

    // Establish the source and destination endpoint details
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int sckt = mud::core::internal_handle<int>(_socket.handle());
    if (::getsockname(sckt, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(_socket.error(), std::system_category(),
                                "retrieving TCP local endpoint details");
    }
    tcp::endpoint local_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    _socket.source_endpoint(local_endpoint);

    if (::getpeername(sckt, (struct sockaddr*)&addr, &len) != 0) {
        throw std::system_error(_socket.error(), std::system_category(),
                                "retrieving TCP remote endpoint details");
    }
    tcp::endpoint remote_endpoint(addr.sin_addr.s_addr, ntohs(addr.sin_port));
    _socket.destination_endpoint(remote_endpoint);

    // Call the registered function handler.
    if (_on_connect_func != nullptr) {
        _on_connect_func(std::move(_socket));
    }

    // Done
    return mud::event::event::return_type::REMOVE;
}

/** The communicator */

tcp::communicator::communicator(mud::event::event_loop& event_loop)
  : _connected(false), _event_loop(event_loop), _on_receive_func(nullptr)
{}

tcp::communicator&
tcp::communicator::operator=(communicator&& rhs)
{
    if (&rhs != this) {
        _connected = rhs._connected;
        rhs._connected = false;
        _socket = std::move(rhs._socket);
        _on_receive_func = rhs._on_receive_func;
    }
    return *this;
}

tcp::communicator::~communicator()
{
    close();
}

void
tcp::communicator::open(tcp::socket&& socket)
{
    _connected = true;
    _socket = std::move(socket);
    _event_loop.register_handler(mud::event::event(
        _socket.handle(), mud::event::event::signal_type::READING,
        std::bind(&communicator::on_ready_receive, this)));
}

void
tcp::communicator::close()
{
    _connected = false;
    if (_socket.handle() != nullptr) {
        _event_loop.deregister_handler(mud::event::event(_socket.handle()));
        _socket.close();
    }
}

bool
tcp::communicator::connected() const
{
    return _connected;
}

std::ostream&
tcp::communicator::ostr()
{
    return _socket.ostr();
}

std::istream&
tcp::communicator::istr()
{
    return _socket.istr();
}

void
tcp::communicator::on_receive(on_receive_func func)
{
    _on_receive_func = func;
}

mud::event::event::return_type
tcp::communicator::on_ready_receive()
{
    // Call the registered function handler.
    if (_on_receive_func != nullptr) {
        _on_receive_func();
    }

    // Continue receiving while the socket is still open
    if (_socket.handle() != nullptr) {
        return mud::event::event::return_type::CONTINUE;
    } else {
        return mud::event::event::return_type::REMOVE;
    }
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
