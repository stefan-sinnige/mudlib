/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#if defined(_WIN32)
    #include <io.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define RECVFROM_CAST (char*)
    #define SENDTO_CAST (const char*)
    #define WOULDBLOCK WSAEWOULDBLOCK
    #ifndef MUDLIB_SSIZE_T
        #define MUDLIB_SSIZE_T
typedef long ssize_t;
    #endif
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #define RECVFROM_CAST (void*)
    #define SENDTO_CAST (const void*)
    #define WOULDBLOCK EWOULDBLOCK
#endif
#include "mud/core/handle.h"
#include <fcntl.h>
#include <system_error>

BEGIN_MUDLIB_CORE_NS

#if defined(_WIN32)
/* Winsock initialiser. */
struct WSAInitialiser
{
    WSAInitialiser()
    {
        WORD version = MAKEWORD(2, 2);
        WSADATA wsa;
        if (WSAStartup(version, &wsa) != 0) {
            exit(1);
        }
    }
    ~WSAInitialiser() { WSACleanup(); }
} g_wsa_initialiser;
#endif

/**
 * Implementation of a @signal signalling resource using a UDP socket.
 */
template<>
class select_handle::signal::impl
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
    std::shared_ptr<mud::core::handle> handle() const;

    /**
     * Send a signal to the resource.
     */
    void trigger();

    /**
     * Receive a signal.
     */
    bool capture();

private:
    /** The socket handle */
    std::shared_ptr<mud::core::handle> _handle;

    /** The address and port of the bound UDP socket. */
    struct sockaddr_in _addr;
};

template<>
void
select_handle::signal::impl_deleter::operator()(signal::impl* ptr) const
{
    delete ptr;
}

select_handle::signal::impl::impl()
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
    if (::bind(fd, (struct sockaddr*)&addr, len) < 0) {
        throw std::system_error(errno, std::system_category(), "bind");
    }

    // Make the socket non-blocking
#if defined(_WIN32)
    u_long mode = true;
    if (::ioctlsocket(fd, FIONBIO, &mode) != 0) {
        throw std::system_error(::WSAGetLastError(), std::system_category(),
                                "setting socket option (non-blocking)");
    }
#else
    int flags;
    if ((flags = ::fcntl(fd, F_GETFL, 0)) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "setting socket option (non-blocking)");
    }
    flags |= O_NONBLOCK;
    if (::fcntl(fd, F_SETFL, flags) < 0) {
        throw std::system_error(errno, std::system_category(),
                                "setting socket option (non-blocking)");
    }
#endif

    // Get address
    len = sizeof(_addr);
    ::memset(&_addr, 0, len);
    if (::getsockname(fd, (struct sockaddr*)&_addr, &len) != 0) {
        throw std::system_error(errno, std::system_category(), "getsockname");
    }

    // Save the handle
    _handle = std::make_shared_ptr<mud::core::select_handle>(fd);

    // Logging
    LOG(log);
    DEBUG(log) << "Select socket fd: " << fd << std::endl;
}

select_handle::signal::impl::~impl()
{
    if (_handle != nullptr) {
#if defined(_WIN32)
        ::closesocket(mud::core::internal_handle<int>(_handle));
#else
        ::close(mud::core::internal_handle<int>(_handle));
#endif
        _handle.reset();
    }
}

void
select_handle::signal::impl::trigger()
{
    LOG(log);
    TRACE(log) << "Select socket trigger fd: "
               << mud::core::internal_handle<int>(_handle) << std::endl;

    // Write over UDP
    struct sockaddr_in write_addr;
    ::memcpy(&write_addr, &_addr, sizeof(write_addr));
    char ch = 'N';
    ssize_t nwrite =
        ::sendto(mud::core::internal_handle<int>(_handle), SENDTO_CAST & ch, 1,
                 0, (struct sockaddr*)&write_addr, sizeof(write_addr));
    if (nwrite < 0) {
        throw std::system_error(errno, std::system_category(), "sendto");
    }
}

bool
select_handle::signal::impl::capture()
{
    LOG(log);
    TRACE(log) << "Select socket capture fd: "
               << mud::core::internal_handle<int>(_handle) << std::endl;

    // Read from UDP
    struct sockaddr_in recv_addr;
    ::memcpy(&recv_addr, &_addr, sizeof(recv_addr));
    socklen_t len = sizeof(recv_addr);
    char ch;
    int nread =
        ::recvfrom(mud::core::internal_handle<int>(_handle), RECVFROM_CAST & ch,
                   1, 0, (struct sockaddr*)&recv_addr, &len);
#if defined(_WIN32)
    int error = ::GetLastError();
#else
    int error = errno;
#endif
    if (nread < 0 && error != WOULDBLOCK) {
        throw std::system_error(error, std::system_category(), "recvfrom");
    }
    return nread == 1;
}

std::shared_ptr<mud::core::handle>
select_handle::signal::impl::handle() const
{
    return _handle;
}

/** The explicit implementation for self signalling resources. */

template<>
select_handle::signal::signal()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

template<>
select_handle::signal::~signal()
{}

template<>
std::shared_ptr<mud::core::handle>
select_handle::signal::handle() const
{
    return _impl->handle();
}

template<>
void
select_handle::signal::trigger()
{
    _impl->trigger();
}

template<>
bool
select_handle::signal::capture()
{
    return _impl->capture();
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
