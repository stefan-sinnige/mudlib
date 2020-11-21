#include <sys/socket.h>
#include "mud/io/tcp.h"
#include "mud/io/exception.h"
#include "mud/io/streambuf.h"

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with sockets. The read and write operations
 * to be used are the POSIX 'recv' and 'recv'.
 */

class socket_read
{
public:
    ssize_t
    operator()(int fd, void* buf, size_t count) {
        return ::recv(fd, buf, count, 0);
    }
};

class socket_write
{
public:
    ssize_t
    operator()(int fd, const void* buf, size_t count) {
        return ::send(fd, buf, count, 0);
    }
};

typedef mud::io::basic_streambuf<socket_read, socket_write> socket_streambuf;

/**
 * @brief Implementation class for POSIX compliant @c TCP socket.
 */

class tcp::socket::impl
{
public:
    /**
     * Constructor.
     */
    impl(const std::unique_ptr<mud::io::kernel_handle>&);

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

private:
/** Reference to the socket handle. */
const std::unique_ptr<mud::io::kernel_handle>& _handle;

/** The stream for reading. */
std::istream _istr;

/** The stream for writing. */
std::ostream _ostr;

/** The stream buffer for reading */
std::unique_ptr<socket_streambuf> _read_buffer;

/** The stream buffer for writing */
std::unique_ptr<socket_streambuf> _write_buffer;
};

tcp::socket::impl::impl(const std::unique_ptr<mud::io::kernel_handle>& handle)
    : _handle(handle), _istr(nullptr), _ostr(nullptr)
{
    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer  = std::unique_ptr<socket_streambuf>(
                    new socket_streambuf(_handle, 4096, 16));
    _write_buffer = std::unique_ptr<socket_streambuf>(
                    new socket_streambuf(_handle, 4096, 16));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());

}

tcp::socket::impl::~impl()
{
}

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

/** The endpoint */

tcp::endpoint::endpoint()
    : _address(), _port(0)
{
}

tcp::endpoint::endpoint(const mud::io::ip::address& address, uint16_t port)
    : _address(address), _port(port)
{
}

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

tcp::endpoint::~endpoint()
{
}

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
    : ip::socket(basic_socket::domain_t::INET,
              basic_socket::type_t::STREAM,
              basic_socket::protocol_t::INTRINSIC)
{
    _impl = std::unique_ptr<impl>(new impl(handle()));
}

tcp::socket::socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol)
    : ip::socket(domain, type, protocol)
{
    _impl = std::unique_ptr<impl>(new impl(handle()));
}

tcp::socket::socket(
        basic_socket::domain_t domain,
        basic_socket::type_t type,
        basic_socket::protocol_t protocol,
        std::unique_ptr<mud::io::kernel_handle> hndl)
    : ip::socket(domain, type, protocol, std::move(hndl))
{
    _impl = std::unique_ptr<impl>(new impl(handle()));
}

tcp::socket::socket(socket&& rhs)
    : ip::socket(std::move(rhs))
{
    _impl = std::unique_ptr<impl>(new impl(handle()));
}

tcp::socket::~socket()
{
}

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

/** The acceptor */

tcp::acceptor::acceptor(tcp::socket& socket)
    : _socket(socket)
{
    /* Set-up to reuse address to eliminate TIME_WAIT conditions. */
    _socket.option<bool, mud::io::ip::reuse_address>(true);
}

tcp::acceptor::~acceptor()
{
}

void
tcp::acceptor::open(const endpoint& endpoint)
{
    /* Bind the socket to the endpoint. */
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = static_cast<sa_family_t>(_socket.domain());
    addr.sin_port = endpoint.port();
    addr.sin_addr.s_addr = endpoint.address();
    if (::bind(*(_socket.handle()), (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        throw std::system_error(errno, std::system_category(),
                "binding TCP endpoint");
    }

    /* Prepare to listen. */
    if (::listen(*(_socket.handle()), 8) != 0) {
        throw std::system_error(errno, std::system_category(),
                "listening for TCP connections");
    }
}

tcp::socket
tcp::acceptor::accept()
{
    // Accept the connection. This may block if there is no client ready.
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof(sockaddr_in));
    socklen_t addr_sz = sizeof(addr);
    int fd = ::accept(*(_socket.handle()), (struct sockaddr*)&addr, &addr_sz);
    if (fd < 0) {
        throw std::system_error(errno, std::system_category(),
                "accepting TCP connection");
    }

    // Return the client socket connection.
    std::unique_ptr<mud::io::kernel_handle> handle;
    handle = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(fd));
    return tcp::socket(_socket.domain(), _socket.type(), _socket.protocol(),
                    std::move(handle));
}

/** The connector */

tcp::connector::connector(tcp::socket& socket)
    : _socket(socket)
{
}

tcp::connector::~connector()
{
}

void
tcp::connector::connect(const endpoint& endpoint)
{
    /* Connect to the endpoint. */
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = static_cast<sa_family_t>(_socket.domain());
    addr.sin_port = endpoint.port();
    addr.sin_addr.s_addr = endpoint.address();
    if (::connect(*(_socket.handle()), (struct sockaddr*)&addr,
                    sizeof(addr)) != 0)
    {
        throw std::system_error(errno, std::system_category(),
                "connecting TCP endpoint");
    }
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

