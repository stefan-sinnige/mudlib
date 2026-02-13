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
#include "mud/core/event_loop.h"
#include "mud/core/exception.h"
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
         * @param [in] handle  The handle to use.
         * @param [in] bufsize The initial buffer size.
         * @param [in] putbacksize The size of the putback buffer.
         */
        streambuf(std::shared_ptr<mud::core::handle> handle,
                  size_t bufsize = 10, size_t putbacksize = 4);

        /* TCP specific read and write functions. */
        ssize_t read(void* buffer, size_t count) override;
        ssize_t write(const void* buffer, size_t count) override;
    };

    streambuf::streambuf(std::shared_ptr<mud::core::handle> handle,
                         size_t bufsize, size_t putbacksize)
      : mud::io::basic_streambuf(handle, bufsize, putbacksize)
    {}

    ssize_t streambuf::read(void* buf, size_t count)
    {
        LOG(log);
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nread;
        int tries = 10;
        while ((nread = ::recv(hndl, RECV_CAST buf, count, 0)) < 0
               && tries-- > 0)
        {
            if (errno == EINTR) {
                /* Receive was interrupted, try again */
                continue;
            }
            else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Not ready to receive, wait a bit (5 seconds maximum). This
                 * has the side-effect that this read will be blocked at most
                 * by this time interval. */
                fd_set rdfs;
                FD_ZERO(&rdfs);
                FD_SET(hndl, &rdfs);
                struct timeval  tv;
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                if (::select(hndl+1, &rdfs, nullptr, nullptr, &tv) <= 0) {
                    /* Still nothing available, or low-level errors */
                    ERROR(log) << "No data available on socket " << hndl
                               << ": " << strerror(errno) << std::endl;
                    return -1;
                }
                if (! FD_ISSET(hndl, &rdfs)) {
                    ERROR(log) << "No data available on socket " << hndl
                               << std::endl;
                    return -1;
                }
                continue;
            }
            else {
                /* Anything else is an error */
                ERROR(log) << "Error on socket " << hndl << strerror(errno)
                               << std::endl;
                return -1;
            }
        }

        /* We may have received less than asked for, for example, the peer is
         * still sending more packets. We may need subsequent calls to get more
         * data if needed, dictated by more calls to `underflow` or related. */
        TRACE(log) << "Received " << nread << " bytes on TCP socket " << hndl
                   << std::endl;
        return nread;
    }

    ssize_t streambuf::write(const void* buf, size_t count)
    {
        LOG(log);
        int hndl = mud::core::internal_handle<int>(handle());
        ssize_t nwrite = 0;
        int tries = 10;
        while ((nwrite = ::send(hndl, SEND_CAST buf, count, 0)) < 0
               && tries -- > 0)
        {
            if (errno == EINTR) {
                /* Send was interrupted, try again */
                continue;
            }
            else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Not ready to write, wait a bit (1 millisecond) */
                fd_set wrfs;
                FD_ZERO(&wrfs);
                FD_SET(hndl, &wrfs);
                struct timeval  tv;
                tv.tv_sec = 0;
                tv.tv_usec = 1 * 1000;
                if (::select(hndl+1, nullptr, &wrfs, nullptr, &tv) <= 0) {
                    /* Still not ready to write, or low-level errors */
                    ERROR(log) << "Socket " << hndl << " not ready to write"
                               << std::endl;
                    return -1;
                }
                if (! FD_ISSET(hndl, &wrfs)) {
                    ERROR(log) << "Socket " << hndl << " not ready to write"
                               << std::endl;
                    return -1;
                }
                continue;
            }
            else if (errno == EMSGSIZE) {
                /* The block is too large, halve it and try again */
                if (count <= 1) {
                    /* Smallest block possible was not small enough */
                    return -1;
                }
                count >>= 1;
                continue;
            }
            else {
                /* Anything else is an error. */
                ERROR(log) << "Error on socket " << hndl << strerror(errno)
                               << std::endl;
                return -1;
            }
        }
        /* We may have written less than asked for, for example, the output
         * buffer is full. We may need subsequent calls to flush more data if
         * needed dictated by more calls to `underflow` or related. */
        TRACE(log) << "Sent " << nwrite << " bytes on TCP socket " << hndl
                   << std::endl;
        return nwrite;
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
    impl(std::shared_ptr<mud::core::handle>);

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
    /** Reference to the socket handle. */
    std::shared_ptr<mud::core::handle> _handle;

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

tcp::socket::impl::impl(std::shared_ptr<mud::core::handle> handle)
  : _handle(handle), _istr(nullptr), _ostr(nullptr)
{
    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer = std::unique_ptr<_tcp::streambuf>(
        new _tcp::streambuf(_handle, 4096, 16));
    _write_buffer = std::unique_ptr<_tcp::streambuf>(
        new _tcp::streambuf(_handle, 4096, 16));
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

tcp::endpoint::endpoint(const mud::io::ip::address& address, uint16_t port)
  : _address(address), _port(port)
{}

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
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(handle()));
}

tcp::socket::socket(std::nullptr_t)
{
}

tcp::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                    basic_socket::protocol_t protocol)
  : ip::socket(domain, type, protocol)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(handle()));
}

tcp::socket::socket(basic_socket::domain_t domain, basic_socket::type_t type,
                    basic_socket::protocol_t protocol,
                    std::shared_ptr<mud::core::handle> hndl)
  : ip::socket(domain, type, protocol, hndl)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(handle()));
}

tcp::socket::~socket() {}

std::istream&
tcp::socket::istr()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->istr();
}

std::ostream&
tcp::socket::ostr()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->ostr();
}

const tcp::endpoint&
tcp::socket::source_endpoint() const
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->source_endpoint();
}

const tcp::endpoint&
tcp::socket::destination_endpoint() const
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->destination_endpoint();
}

void
tcp::socket::source_endpoint(const tcp::endpoint& endpoint)
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    _impl->source_endpoint() = endpoint;
}

void
tcp::socket::destination_endpoint(const tcp::endpoint& endpoint)
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    _impl->destination_endpoint() = endpoint;
}

/** The acceptor */

tcp::acceptor::acceptor()
  : _connected(false)
{
    _accept_event = mud::core::event(mud::core::uuid(), _listen.handle(),
            mud::core::event::signal_type::READING);

    /* Set-up socket options to
     *   - reuse address to eliminate TIME_WAIT conditions.
     *   - enable non-blocking socket I/O */
    _listen.option<bool, mud::io::ip::reuse_address>(true);
    _listen.option<bool, mud::io::ip::nonblocking>(true);
}

tcp::acceptor::~acceptor()
{
    detach();
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
    LOG(log);
    INFO(log) << "TCP listening to " << local_endpoint.address().str() << ":"
              << local_endpoint.port() << " [" << _accept_event.topic() << "]"
              << std::endl;
    if (::listen(lstn, 8) != 0) {
        throw std::system_error(_listen.error(), std::system_category(),
                                "listening for TCP connections");
    }
    _connected = true;

    /* Add the acceptor to the event loop */
    attach(_accept_event.topic(), &acceptor::on_ready_accept);
    mud::core::event_loop::global().add(std::move(_accept_event));
}

void
tcp::acceptor::close()
{
    if (_connected) {
        _connected = false;
        if (_listen.handle() != nullptr) {
            detach();
            _listen.close();
        }
    }
}

mud::core::event&
tcp::acceptor::event()
{
    return _accept_event;
}

void
tcp::acceptor::on_ready_accept(const mud::core::message& msg)
{
    // Bail out if not connected
    if (!_connected) {
        detach();
        return;
    }

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
    std::shared_ptr<mud::core::handle> handle;
    handle = std::make_shared<mud::core::select_handle>(fd);
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
    LOG(log);
    INFO(log) << "TCP accepted connection "
              << remote_endpoint.address().str() << ":"
              << remote_endpoint.port() << std::endl;

    // Add the socket to the queue and publish that a connection is ready.
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pending.push(std::move(client));
    }
    mud::core::broker::publish(_accepted);

    // Remove the event when the socket is closed.
    if (_listen.handle() == nullptr) {
        detach();
    }
}

tcp::socket
tcp::acceptor::connection()
{
    std::lock_guard<std::mutex> lock(_mutex);

    // If there are no pending connections to be handled, return an invalid one.
    if (_pending.size() == 0) {
        tcp::socket invalid(nullptr);
        return invalid;
    }

    // Pop a connection.
    tcp::socket connection = std::move(_pending.front());
    _pending.pop();
    return connection;
}

/** The connector */

tcp::connector::connector()
{
    _connect_event = mud::core::event(mud::core::uuid(), _socket.handle(),
            mud::core::event::signal_type::WRITING);

    /* Set-up socket option to enable non-blocking I/O */
    _socket.option<bool, mud::io::ip::nonblocking>(true);
}

tcp::connector::~connector()
{
    detach();
}

void
tcp::connector::open(const endpoint& endpoint)
{
    LOG(log);
    INFO(log) << "TCP connecting to " << endpoint.address().str() << ":"
              << endpoint.port() << " [" << _connect_event.topic() << "]"
              << std::endl;

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

    /* Add the connector to the event loop */
    attach(_connect_event.topic(), &connector::on_ready_connect);
    mud::core::event_loop::global().add(std::move(_connect_event));
}

mud::core::event&
tcp::connector::event()
{
    return _connect_event;
}

void
tcp::connector::on_ready_connect(const mud::core::message& msg)
{
    LOG(log);
    INFO(log) << "TCP connected" << std::endl;

    /* Remove the connector from the event-loop */
    detach();

    /* Establish the source and destination endpoint details */
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

    /* Call the notification */
    mud::core::broker::publish(connected());
}

/** The communicator */

tcp::communicator::communicator()
    : _socket(nullptr)
{
}

tcp::communicator::~communicator()
{
    detach();
    close();
}

void
tcp::communicator::open(tcp::socket&& socket)
{
    _connected = true;
    _socket = std::move(socket);

    /* Publish that a connection has been made. */
    mud::core::broker::publish(connected());

    /* Register the communicator to the event loop */
    _receive_event = mud::core::event(mud::core::uuid(), _socket.handle(),
            mud::core::event::signal_type::READING);
    attach(_receive_event.topic(), &communicator::on_ready_receive);
    mud::core::event_loop::global().add(std::move(_receive_event));
}

void
tcp::communicator::close()
{
    if (_connected) {
        _connected = false;
        if (_socket.handle() != nullptr) {
            detach();
            _socket.close();
    
            /* Issue a disconnection event */
            mud::core::broker::publish(disconnected());
        }
    }
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

tcp::socket&
tcp::communicator::device()
{
    return _socket;
}

mud::core::event&
tcp::communicator::event()
{
    return _receive_event;
}

void
tcp::communicator::on_ready_receive(const mud::core::message& msg)
{
    // Bail out if not connected
    if (!_connected) {
        return;
    }

    // If no data present, assume that the connection is closed.
    if (istr().peek() == std::char_traits<char>::eof()) {
        close();
        return;
    }

    // Publish the notification
    mud::core::broker::publish(received());

    // Remove handler when socket is closed.
    if (_socket.handle() == nullptr) {
        detach();
    }
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
