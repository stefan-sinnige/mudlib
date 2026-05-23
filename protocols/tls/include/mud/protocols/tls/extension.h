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

#ifndef _MUDLIB_TLS_EXTENSION_H_
#define _MUDLIB_TLS_EXTENSION_H_

#include <iostream>
#include <memory>
#include <vector>
#include <mud/protocols/tls/ns.h>
#include <mud/crypto/types.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The base definition of an TLS extension
 * @see RFC 8446: Section 4.2
 */
class MUDLIB_TLS_API extension
{
public:
    /**
     * @brief The supported  TLS extensions
     * @details
     */
    enum class type_t : uint16_t
    {
        server_name = 0,
        max_fragment_length = 1,
        unassigned = 0xffff
    };

    /**
     * @brief Destructor.
     */
    virtual ~extension() = default;

    /**
     * @brief Return the extension type.
     */
    type_t extension_type() const { return _type; }

protected:
    /**
     * Constructor
     * @param extension_type The extension type
     */
    extension(type_t extension_type);

private:
    /** The extension type */
    type_t _type;
};

/**
 * @brief The TLS server-name extension.
 */
class server_name: public extension
{
public:
    /**
     * @brief Create an empty server-name extension.
     */
    server_name();

private:
    /* Friends */
    friend std::istream& operator>>(std::istream& istr, server_name& ext);
    friend std::ostream& operator>>(std::ostream& istr, const server_name& ext);
};

/**
 * @brief Read a TLS server-name extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, server_name& ext);

/**
 * @brief Write a TLS server-name extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const server_name& ext);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_EXTENSION_H_ */
