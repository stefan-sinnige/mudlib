#if defined(WINDOWS) && defined(NATIVE)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #define RECVFROM_CAST (char*)
    #define SENDTO_CAST   (const char*)
    #ifndef MUDLIB_SSIZE_T
        #define MUDLIB_SSIZE_T
        typedef long ssize_t;
    #endif
#else
    #include <unistd.h>
    #define RECVFROM_CAST (void*)
    #define SENDTO_CAST   (const void*)
#endif
#include "posix/select_self.h"
#include <system_error>
#include <fcntl.h>

BEGIN_MUDLIB_EVENT_NS

#if defined(WINDOWS) && defined(NATIVE)
/* Winsock initialiser. */
struct WSAInitialiser
{
    WSAInitialiser() {
        WORD version = MAKEWORD(2, 2);
        WSADATA wsa;
        if (WSAStartup(version, &wsa) != 0) {
            exit(1);
        }
    }
    ~WSAInitialiser() {
        WSACleanup();
    }
} g_wsa_initialiser;
#endif

/**
 * Implementation of a @select_self signalling resource using a UDP socket.
 */
class select_self::impl
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The handle.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * Send a signal to the resource.
     */
    void send();

    /**
     * Receive a signal.
     */
    void receive();

private:
    /** The socket handle */
    std::unique_ptr<mud::core::handle> _handle;

    /** The address and port of the bound UDP socket. */
    struct sockaddr_in _addr;
};

void
select_self::impl_deleter::operator()(select_self::impl* ptr) const
{
    delete ptr;
}

select_self::impl::impl()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create socket
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        throw std::system_error(errno, std::system_category(), "socket");
    }

    // Bind to port 0 to allocate a free port
    if (::bind(fd, (struct sockaddr*) &addr, len) < 0) {
        throw std::system_error(errno, std::system_category(), "bind");
    }

    // Get address
    len = sizeof(_addr);
    ::memset(&_addr, 0, len);
    if (::getsockname(fd, (struct sockaddr*)&_addr, &len) != 0) {
        throw std::system_error(errno, std::system_category(), "getsockname");
    }

    // Save the handle
    _handle = std::unique_ptr<mud::core::handle>(new mud::core::int_handle(fd));
}

select_self::impl::~impl()
{
    if (_handle != nullptr) {
#if defined(WINDOWS) && defined(NATIVE)
        ::closesocket(mud::core::internal_handle<int>(_handle));
#else
        ::close(mud::core::internal_handle<int>(_handle));
#endif
        _handle.reset(nullptr);
    }
}

void
select_self::impl::send()
{
    // Write over UDP
    struct sockaddr_in write_addr;
    ::memcpy(&write_addr, &_addr, sizeof(write_addr));
    char ch = 'N';
    ssize_t nwrite = ::sendto(
                    mud::core::internal_handle<int>(_handle), SENDTO_CAST & ch, 1, 0,
                    (struct sockaddr*)&write_addr, sizeof(write_addr));
    if (nwrite < 0) {
        throw std::system_error(errno, std::system_category(), "sendto");
    }
}

void
select_self::impl::receive()
{
    // Read from UDP
    struct sockaddr_in recv_addr;
    ::memcpy(&recv_addr, &_addr, sizeof(recv_addr));
    socklen_t len = sizeof(recv_addr);
    char ch;
    int nread = ::recvfrom(
                    mud::core::internal_handle<int>(_handle), RECVFROM_CAST &ch, 1, 0,
                    (struct sockaddr*)&recv_addr, &len);
    if (nread <= 0) {
        throw std::system_error(errno, std::system_category(), "recvfrom");
    }
}

const std::unique_ptr<mud::core::handle>&
select_self::impl::handle() const
{
    return _handle;
}

/** The explicit implementation for self signalling resources. */

select_self::select_self()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

select_self::~select_self()
{
}

const std::unique_ptr<mud::core::handle>&
select_self::handle() const
{
    return _impl->handle();
}

void
select_self::send()
{
    _impl->send();
}

void
select_self::receive()
{
    _impl->receive();
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
