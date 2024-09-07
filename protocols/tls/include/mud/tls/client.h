#ifndef _MUDLIB_TLS_CLIENT_H_
#define _MUDLIB_TLS_CLIENT_H_

#include <future>
#include <mud/tls/ns.h>
#include <mud/io/tcp.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The client part of an TLS connection pair. The client connects
 * to a server, sends a request and waits for a response.
 * This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_TLS_API client
{
public:
    /**
     * @brief Constructor of an new TLS client.
     * @param[in] event_loop The event-loop to register the TCP socket to.
     */
    client(
        mud::event::event_loop& event_loop = mud::event::event_loop::global());

    /**
     * @brief Destructor.
     */
    virtual ~client() = default;

    /**
     * Non-copyable.
     */
    client(const client&) = delete;
    client& operator=(const client&) = delete;

private:
    /** Implementation */
    class communicator;
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_TLS_NS

#endif /* _MUDLIB_TLS_CLIENT_H_ */
