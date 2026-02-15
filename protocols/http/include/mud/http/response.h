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

#ifndef _MUDLIB_HTTP_RESPONSE_H_
#define _MUDLIB_HTTP_RESPONSE_H_

#include <mud/http/message.h>
#include <mud/http/ns.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of an HTTP response message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API response : public message
{
public:
    /**
     * Construct an empty HTTP response message.
     */
    response();

    /**
     * Copy an HTTP response message.
     * @param[in] rhs The message details to copy.
     */
    response(const response& rhs) = default;

    /**
     * Move an HTTP response message.
     * @param[in] rhs The message details to move.
     */
    response(response&& rhs) = default;

    /**
     * Assign an HTTP response message.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    response& operator=(const response& rhs) = default;

    /**
     * Move assign an HTTP response message.
     * @param[in] rhs The message details to move.
     * @return Reference to this message.
     */
    response& operator=(response&& rhs) = default;

    /**
     *  Destructor.
     */
    virtual ~response() = default;

    /**
     * Set the status code.
     * @param[in] value  The status code value to set.
     */
    void status_code(const http::status_code& value) { _status_code = value; }

    /**
     * Get the status code.
     */
    const http::status_code& status_code() const { return _status_code; }
    http::status_code& status_code() { return _status_code; }

    /**
     * Set the reson phrase.
     * @param[in] value  The reason phrase value to set.
     */
    void reason_phrase(const http::reason_phrase& value)
    {
        _reason_phrase = value;
    }
    void reason_phrase(http::reason_phrase_e value)
    {
        _reason_phrase = http::reason_phrase(value);
    }

    /**
     * Get the reason phrase.
     */
    const http::reason_phrase& reason_phrase() const { return _reason_phrase; }
    http::reason_phrase& reason_phrase() { return _reason_phrase; }

private:
    /**
     * The status line
     */
    http::status_code _status_code;
    http::reason_phrase _reason_phrase;
};

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_RESPONSE_H_ */
