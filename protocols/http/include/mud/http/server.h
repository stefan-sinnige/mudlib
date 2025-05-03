#ifndef _MUDLIB_HTTP_SERVER_H_
#define _MUDLIB_HTTP_SERVER_H_

#include <memory>
#include <mud/http/ns.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/core/object.h>
#include <mud/io/tcp.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The server part of an HTTP connection pair. The server accepts
 * connections from a client and responds to requests from a client.
 * This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API server : public mud::core::object
{
public:
    /**
     * @brief Constructor of an new HTTP server.
     */
    server();

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
     * @brief Retun the listening end-point.
     */
    const mud::io::tcp::endpoint& endpoint() const;

    /**
     * @brief Process an incoming request.
     *
     * @details
     * When one of the attached clients have received an HTTP request, the
     * server will need to response with an appropriate response. The client
     * will invoke this function and wait for a response to be returned that
     * can then be passed back to the connected peer.
     *
     * This function should adhere to a thread-safe execution model as it can
     * be invoked by multiple connected clients simultaneously.
     *
     * @param req The HTTP request message as received by a client.
     * @param resp The HTTP response to be manipulated to return to the client.
     */
    virtual void on_request(
            const mud::http::request& req,
            mud::http::response& resp) = 0;

    /**
     * Non-copyable.
     */
    server(const server&) = delete;
    server& operator=(const server&) = delete;

private:
    /** Implementation */
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
