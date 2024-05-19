#ifndef _MUDLIB_HTTP_CLIENT_H_
#define _MUDLIB_HTTP_CLIENT_H_

#include <future>
#include <mud/http/ns.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/io/tcp.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The client part of an HTTP connection pair. The client connects
 * to a server, sends a request and waits for a response.
 * This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API client
{
public:
    /**
     * @brief Constructor of an new HTTP client.
     * @param[in] event_loop The event-loop to register the TCP socket to.
     */
    client(
        mud::event::event_loop& event_loop = mud::event::event_loop::global());

    /**
     * @brief Destructor.
     */
    virtual ~client() = default;

    /**
     * @brief Send a request and receive the response.
     * @param[in] endpoint The endpoint to connect to.
     * @param[in] req The request message.
     * @return A future to the reponse message.
     */
    std::future<mud::http::response> request(
        const mud::io::tcp::endpoint& endpoint, const mud::http::request& req);

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

END_MUDLIB_HTTP_NS

#endif /* _MUDLIB_HTTP_CLIENT_H_ */
