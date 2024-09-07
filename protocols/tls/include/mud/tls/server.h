#ifndef _MUDLIB_TLS_SERVER_H_
#define _MUDLIB_TLS_SERVER_H_

#include <memory>
#include <mud/tls/ns.h>
#include <mud/io/tcp.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The server part of an TLS connection pair. The server accepts
 * connections from a client and responds to requests from a client.
 * This conforms with
 *    RFC xxxx
 */
class MUDLIB_TLS_API server
{
public:
    /**
     * @brief Constructor of an new TLS server.
     * @param[in] event_loop The event-loop to register the listening socket to.
     */
    server(
        mud::event::event_loop& event_loop = mud::event::event_loop::global());

    /**
     * @brief Destructor.
     */
    virtual ~server() = default;

    /**
     * Non-copyable.
     */
    server(const server&) = delete;
    server& operator=(const server&) = delete;

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

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_SERVER_H_ */
