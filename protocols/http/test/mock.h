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

#ifndef _MUDLIB_HTTP_MOCK_H_
#define _MUDLIB_HTTP_MOCK_H_

#include "mud/http/client.h"
#include "mud/http/server.h"
#include "mud/io/tcp.h"
#include "mud/test.h"

namespace mock {

/**
 * Mock HTTP server
 */
class server: public mud::http::server
{
public:
    /*
     * Constructor.
     */
    server();

    /*
     * Specify the response to reply with.
     */
    void response(const mud::http::response& resp);

protected:
    /*
     * Handle incoming request.
     */
    mud::http::response request(const mud::http::request& req) override;

private:
    /* The response to reply with */
    mud::http::response _resp;
};

/**
 * Mock HTTP client
 */
class client: public mud::http::client
{
public:
    /*
     * Constructor.
     */
    client();
};

}; // namespace mock

/* clang-format on */

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_MOCK_H_ */
