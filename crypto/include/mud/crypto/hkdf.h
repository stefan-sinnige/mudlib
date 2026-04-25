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

#ifndef _MUDLIB_CRYPTO_HKDF_H_
#define _MUDLIB_CRYPTO_HKDF_H_

#include <memory>
#include <mud/crypto/ns.h>
#include <mud/crypto/hmac.h>
#include <mud/crypto/kdf.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief The HMAC based Key Derivation Function (HKDF).
 * @details
 * The HMAC based Key Derivation Function (HKDF) is a general purpose key
 * derivation function to produce strong, high-entropy keys from input source
 * material. 
 *
 * This implementation is conform RFC-5869.
 */
class MUDLIB_CRYPTO_API hkdf: public basic_kdf
{
public:
    /**
     * @brief Return the HKDF specification.
     */
    virtual std::string specs() const override;

    /**
     * @brief Default constructor initialises the kdf.
     * @brief keying The keying material.
     */
    hkdf(const kdf_material_t& keying);

    /**
     * @brief Destructor.
     */
    virtual ~hkdf();

    /**
     * @brief Return a new cryptographic key.
     * @param key_size The desired key-size. If @c 0 (or not defined), the key
     * size as defined in the keying material shall be used.
     * @param info Additional contextual information (optional).
     * @details
     * If required, the KDF will extract information from the keying material
     * and additional information.
     *
     * This method can be called repeatedly to generate multiple keys.
     */
    virtual key_t key(size_t key_size = 0, const data_t& info = data_t())
            override;

private:
    /* The HMAC algorithm */
    std::unique_ptr<basic_hmac> _hmac;

    /* The pseudo-random key */
    key_t _prk;
};

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_HKDF_H_ */
