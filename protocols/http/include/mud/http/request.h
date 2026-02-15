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

#ifndef _MUDLIB_HTTP_REQUEST_H_
#define _MUDLIB_HTTP_REQUEST_H_

#include <strings.h>
#include <mud/http/message.h>
#include <mud/http/ns.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of an HTTP request message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API request : public message
{
public:
    /**
     * Construct an empty HTTP request message.
     */
    request();

    /**
     * Copy an HTTP request message.
     * @param[in] rhs The message details to copy.
     */
    request(const request& rhs) = default;

    /**
     * Move an HTTP request message.
     * @param[in] rhs The message details to move.
     */
    request(request&& rhs) = default;

    /**
     * Assign an HTTP request message.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    request& operator=(const request& rhs) = default;

    /**
     * Move assign an HTTP request message.
     * @param[in] rhs The message details to move.
     * @return Reference to this message.
     */
    request& operator=(request&& rhs) = default;

    /**
     *  Destructor.
     */
    virtual ~request() = default;

    /**
     * Set the method.
     * @param[in] value  The method value to set.
     */
    void method(http::method_e value) { _method = http::method(value); }
    void method(const std::string& value) { _method = http::method(value); }
    void method(const http::method& value) { _method = value; }

    /**
     * Get the method.
     */
    const http::method& method() const { return _method; }
    http::method& method() { return _method; }

    /**
     * Set the URI.
     * @param[in] value  The URI value to set.
     */
    void uri(const mud::core::uri& value);
    void uri(const std::string& value);

    /**
     * Get the URI.
     */
    const http::uri& uri() const { return _uri; }
    http::uri& uri() { return _uri; }

private:
    /**
     * The request line
     */
    http::method _method;
    http::uri _uri;
};

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_REQUEST_H_ */
