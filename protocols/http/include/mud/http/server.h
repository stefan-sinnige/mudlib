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
     * @param req The HTTP request message as received by a client.
     * @return The HTTP response to return to the client.
     *
     * @details
     * When one of the attached clients have received an HTTP request, the
     * server will need to response with an appropriate response. The client
     * will invoke this function and wait for a response to be returned that
     * can then be passed back to the connected peer.
     *
     * This function should adhere to a thread-safe execution model as it can
     * be invoked by multiple connected clients simultaneously.
     */
    virtual mud::http::response request(
            const mud::http::request& req);

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
