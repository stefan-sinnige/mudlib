#ifndef _MUDLIB_HTTP_SERVER_H_
#define _MUDLIB_HTTP_SERVER_H_

#include <memory>
#include <mud/http/ns.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/io/tcp.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The server part of an HTTP connection pair. The server accepts
 * connections from a client and responds to requests from a client.
 * This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API server
{
public:
    /** Function definition for the @c on_request handler. The request message
     * is passed in and the response is expected to be returned. */
    typedef std::function<mud::http::response(const mud::http::request&)>
        on_request_func;

    /**
     * @brief Constructor of an new HTTP server.
     * @param[in] event_loop The event-loop to register the listening socket to.
     */
    server(
        mud::event::event_loop& event_loop = mud::event::event_loop::global());

    /**
     * @brief Destructor.
     */
    virtual ~server() = default;

    /**
     * @brief Start the server. Any incoming connection request is handled
     * asynchroneously.
     * @param[in] endpoint The endpoint to bind to.
     */
    void start(const mud::io::tcp::endpoint& endpoint);

    /**
     * @brief Stop the server.
     */
    void stop();

    /**
     * @brief Register a handler when a request has been received.
     * @param[in] func The handler function
     */
    void on_request(on_request_func func);

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

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_SERVER_H_ */
