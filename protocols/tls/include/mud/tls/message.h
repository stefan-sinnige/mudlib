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

#ifndef _MUDLIB_TLS_MESSAGE_H_
#define _MUDLIB_TLS_MESSAGE_H_

#include <iostream>
#include <mud/tls/ns.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The definition of an TLS message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_TLS_API message
{
public:
    /**
     * The TLS message type
     */
    enum class type
    {
    };

    /**
     * Copy constructor.
     */
    message(const message& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~message() = default;

    /**
     * Assign an TLS mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    message& operator=(const message& rhs) = default;

    /**
     * The message type.
     */
    message::type type() const;

protected:
    /**
     * Construction of a message of a certain type.
     */
    message(enum message::type);

private:
    /**
     * Data members
     */
    enum message::type _type;
};

/** Read an TLS message from an input stream. */
std::istream&
operator>>(std::istream&, message&);

/** Write an TLS message to an output stream. */
std::ostream&
operator<<(std::ostream&, const message&);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_MESSAGE_H_ */
