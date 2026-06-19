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

#ifndef _MUDLIB_TLS_SESSION_H_
#define _MUDLIB_TLS_SESSION_H_

#include <iostream>
#include <memory>
#include <mud/core/buffer.h>
#include <mud/protocols/tls/ns.h>
#include <mud/crypto/types.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The pre-shared key identity
 */
class MUDLIB_TLS_API psk_identity
{
public:
    /**
     * Create an empty pre-shared key identity.
     */
    psk_identity() = default;

    /**
     * @brief Return the identity.
     */
    uint32_t obfuscated_ticket_age() const { return _age; }

    /**
     * @brief Set the identity.
     * @param value The identify value to set
     */
    void obfuscated_ticket_age(uint32_t value) { _age = value; }

    /**
     * @brief Return the identity.
     */
    const mud::core::buffer& identity() const { return _identity; }

    /**
     * @brief Set the identity.
     * @param value The identify value to set
     */
    void identity(const mud::core::buffer& value) { _identity = value; }

    /**
     * @brief Return the identity.
     */
    const mud::core::buffer& binder() const { return _binder; }

    /**
     * @brief Set the binder (HMAC tag).
     * @param value The binder value to set
     */
    void binder(const mud::core::buffer& value) { _binder = value; }

private:
    /** The identity */
    mud::core::buffer _identity;

    /** The binder */
    mud::core::buffer _binder;

    /** The obfuscated ticket age. */
    uint32_t _age = 0;
};

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_SESSION_H_ */
