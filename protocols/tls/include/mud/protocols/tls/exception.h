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

#ifndef _MUDLIB_TLS_EXCEPTION_H_
#define _MUDLIB_TLS_EXCEPTION_H_

#include <exception>
#include <string>
#include <mud/protocols/tls/ns.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief Exception for an invalid TLS message
 */
class invalid_message : public std::exception
{
public:
    /**
     * @brief Default constructor.
     * @details
     * Creates a TLS message to indicate an invalid message without any
     * additional context.
     */
    invalid_message() = default;

    /**
     * @brief Constructor with an explanatory message.
     * @param msg The explanatory message.
     * @details
     * Creates a TLS message to indicate an invalid message with an explanatory
     * message.
     */
    invalid_message(const std::string& msg) : _what(msg) {}

    /**
     * @brief Destructor.
     */
    virtual ~invalid_message() = default;

    /**
     * @brief Return the explanatory string.
     */
    const char* what() const noexcept override { return _what.c_str(); }

private:
    /** The explanatory string */
    std::string _what;
};

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_EXCEPCTION_H_ */
