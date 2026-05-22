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
#include <mud/protocols/tls/ns.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The base definition of an TLS handshake message.
 * @see RFC 8446: Section 4
 */
class MUDLIB_TLS_API handshake
{
public:
    /**
     * @brief The TLS handshake type
     * @details
     * The TLS handshake type defines the supported protocol messages. Other
     * values that are not listed are reserved. They may be compatible with
     * older TLS versions but deprecated
     */
    enum class type_t : uint8_t
    {
        client_hello = 1,
        server_hello = 2,
        new_session_ticket = 4,
        end_of_early_data = 5,
        encrypted_extensions = 8,
        certificate = 11,
        certificate_request = 13,
        certificate_verify = 15,
        finished = 20,
        key_update = 24,
        message_hash = 254
    };

    /**
     * Copy constructor.
     */
    handshake(const handshake& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~handshake() = default;

    /**
     * Assign a TLS mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    handshake& operator=(const handshake& rhs) = default;

    /**
     * The handshake type.
     */
    type_t type() const { return _msg_type; }

protected:
    /**
     * @brief Construction of a message of a certain type.
     * @param type The handshake messgae type.
     */
    handshake(type_t type);

private:
    /** The message type */
    type_t _msg_type;

    /** The message length (24-bits) */
    uint32_t _length;
};

/**
 * @brief TLS Client Hello message
 * @details
 */
class client_hello: public handshake
{
public:
    /**
     * @brief Construct a Client Hello message.
     */
    client_hello();

private:
    /** The protocol version */
    uint16_t _protocol_version;
};

/**
 * @brief TLS Server Hello message
 * @details
 */
class server_hello: public handshake
{
public:
    /**
     * @brief Construct a Server Hello message.
     */
    server_hello();

private:
    /** The protocol version */
    uint16_t _protocol_version;
};

/** Read an TLS message from an input stream. */
std::istream&
operator>>(std::istream&, handshake&);

/** Write an TLS message to an output stream. */
std::ostream&
operator<<(std::ostream&, const handshake&);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_MESSAGE_H_ */
