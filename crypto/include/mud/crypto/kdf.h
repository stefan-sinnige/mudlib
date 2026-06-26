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

#ifndef _MUDLIB_CRYPTO_KDF_H_
#define _MUDLIB_CRYPTO_KDF_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief An interface for a Key Derivation Function (KDF) algorithm.
 * @details
 * A KDF algorithm derives strong secret keys from multiple input sources. These
 * include passphrases, hashes, salts and the number of algorithmic iterations
 * to be used. Modern algorithms often use a concept of extract-then-expand that
 * takes the input from the keying material to extract a pseudo-random key. The
 * expand phase uses this generated key to expand it into one or more keys as
 * the output of the KDF algorithm.
 */
class MUDLIB_CRYPTO_API basic_kdf
{
public:
    /**
     * @brief Return the kdf specifications.
     */
    virtual std::string specs() const = 0;

    /**
     * @brief Default constructor initialises the kdf.
     * @param keying The keying material.
     */
    basic_kdf(const kdf_material_t& keying);

    /**
     * @brief No default constructor.
     */
    basic_kdf() = delete;

    /**
     * @brief Not copyable
     */
    basic_kdf(const basic_kdf&) = delete;

    /**
     * @brief Not moveable
     */
    basic_kdf(basic_kdf&&) = delete;

    /**
     * @brief Not copy-assignable
     */
    basic_kdf& operator=(const basic_kdf&) = delete;

    /**
     * @brief Not move-assignable
     */
    basic_kdf& operator=(basic_kdf&&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~basic_kdf();

    /**
     * @brief Return a cryptographic key.
     * @param key_size The desired key-size. If @c 0 (or not defined), the key
     * size as defined in the keying material shall be used.
     * @param info Additional contextual information (optional).
     * @details
     * If required, the KDF will extract information from the keying material
     * and additional information.
     *
     * Depending on the underlying implementation, this method can be called
     * repeatedly.
     */
    virtual key_t key(size_t key_size = 0, const mud::core::buffer& info = mud::core::buffer()) = 0;

    /**
     * @brief Return the keying material.
     */
    const kdf_material_t& keying() const { return _keying; }

private:
    /* The keying material */
    kdf_material_t _keying;
};

/**
 * @brief Factory for KDFs, indexed by the KDF specification.
 */
typedef ::mud::core::factory<std::string,
                             mud::crypto::basic_kdf,
                             mud::crypto::kdf_material_t>
        kdf_factory;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_KDF_H_ */
