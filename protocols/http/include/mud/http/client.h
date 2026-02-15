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
     */
    client();

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
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_HTTP_NS

#endif /* _MUDLIB_HTTP_CLIENT_H_ */
