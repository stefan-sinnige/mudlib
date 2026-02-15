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

#ifndef _MUDLIB_HTTP_COMMUNICATOR_H_
#define _MUDLIB_HTTP_COMMUNICATOR_H_

#include <mud/http/ns.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/io/tcp.h>
#include <mud/protocols/communicator.h>

BEGIN_MUDLIB_HTTP_NS

namespace communicator {

/**
 * @brief The server-side HTTP communication protocol.
 *
 * @details
 * HTTP (Hypertext Transfer Protocol) is a highly adopted communication protocol
 * and primarily used in the internet domain. The protocol is request-response
 * based where a client initiates a TCP connection to a server and issues a
 * request to the server. The server will then respond to that request.
 */
class MUDLIB_HTTP_API server:
    public mud::protocols::layered_communicator<mud::io::tcp::socket>
{
public:
    /**
     * @brief Create a service-side HTTP communicator.
     *
     * @details
     * Create a communicator that specifically handles the HTTP communication
     * on the server-side.
     *
     * @param lower The communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layered_communicator.
     */
    server(communicator<mud::io::tcp::socket>& lower);

    /**
     * @brief Destructor.
     */
    virtual ~server() = default;
};

/**
 * @brief The client-side HTTP communication protocol.
 *
 * @details
 * HTTP (Hypertext Transfer Protocol) is a highly adopted communication protocol
 * and primarily used in the internet domain. The protocol is request-response
 * based where a client initiates a TCP connection to a server and issues a
 * request to the server. The server will then respond to that request.
 */
class MUDLIB_HTTP_API client:
    public mud::protocols::layered_communicator<mud::io::tcp::socket>
{
public:
    /**
     * @brief Create a service-side HTTP communicator.
     *
     * @details
     * Create a communicator that specifically handles the HTTP communication
     * on the client-side.
     *
     * @param lower The communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layered_communicator.
     */
    client(communicator<mud::io::tcp::socket>& lower);

    /**
     * @brief Destructor.
     */
    virtual ~client() = default;

    /**
     * @brief Send a request to the server.
     *
     * @details
     * Send the specified HTTP request to the server. Any response will be
     * received through the @c on_receive handler.
     *
     * @param req The request to send.
     */
    void request(const mud::http::request& req);
};

}; // namespace communicator

END_MUDLIB_HTTP_NS

#endif /* _MUDLIB_HTTP_COMMUNICATOR_H_ */
