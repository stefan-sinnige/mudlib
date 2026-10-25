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

#ifndef _MUDLIB_CRYPTO_KECCAK_H_
#define _MUDLIB_CRYPTO_KECCAK_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/hash.h>

BEGIN_MUDLIB_CRYPTO_NS

class keccak
{
public:
    /**
     * Delimiter suffix values
     */
    enum class delimited_suffix_t {
        sha3   = 0x06,
        shake  = 0x1f,
        cshake = 0x04
    };

    /**
     * @brief Constructor.
     * @param capacity The capacity in bits.
     * @param digest_bit_size The size of the digest in bits.
     * @param d The delimited suffix (padding)
     */
    keccak(size_t capacity, size_t digest_bit_size, delimited_suffix_t d);

    /**
     * @brief Destructor.
     */
    keccak() = default;

    /**
     * @brief Update the hash with a chunk of buffer input data.
     */
    void update(const mud::core::buffer& chunk);

    /**
     * @brief Update the hash with a chunk of buffer view input data.
     */
    void update(const mud::core::buffer_view& chunk);

    /**
     * @brief Close or finalise the hash.
     */
    void close();

    /**
     * @brief Return the calculated hash value.
     */
    const mud::core::buffer& hash() const { return _hash; }

    /**
     * @brief Return the calculated hash size.
     */
    size_t hash_size() const { return _hash.size(); }

    /**
     * @brief Return the calculated hash size.
     */
    size_t block_size() const { return _rate; }

private:
    /**
     * @brief Computate the hash on the pre-processed fixed sized message block.
     */
    void computate();

    /**
     * @brief Finalise the hash value.
     */
    void finalise();

    /* The calculated hash */
    mud::core::buffer _hash;

    /* The digest size in bytes */
    size_t _digest_size;

    /* The capacity in bytes */
    size_t _capacity;

    /* The rate (block-size) in bytes */
    size_t _rate;

    /** The delimited suffix */
    delimited_suffix_t _d;

    /* State 5x5xWord (1600-bit) */
    uint64_t _S[5][5];

    /* The message block */
    uint8_t _M[200];

    /* The number of bytes available in the message block */
    size_t _M_sz = 0;
};

END_MUDLIB_CRYPTO_NS

#endif /*  _MUDLIB_CRYPTO_KECCAK_H_ */

/* vi: set ai ts=4 expandtab: */
