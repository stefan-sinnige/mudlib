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

#ifndef _MUDLIB_CRYPTO_SHA_H_
#define _MUDLIB_CRYPTO_SHA_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/hash.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief The SHAKE type algorithm.
 * @details
 * The SHAKE-128 and SHAKE-256 algorithms are accessible through a single
 * interface. The arguments passed at construction define the underlying SHA
 * algorithm type to use.
 *
 * This class can be used stand-alone, but the explicit @c typedef definitions
 * of supported algorithms is recommended. See the @c sha template and its
 * explicit specialisations for more information.
 */
class MUDLIB_CRYPTO_API shake_algorithm : public basic_hash
{
public:
    /**
     * @brief Construct a SHA algorithm
     * @param capacity The capacity bit size
     * @param digest_size The digest size (bits).
     * @throw mud::crypto::crypto_error if the combination of parameters yields
     * an unsupported algorithm.
     */
    shake_algorithm(size_t capacity, size_t digest_bit_size);

    /**
     * @brief Return the hash specifications.
     */
    virtual std::string specs() const override;

    /**
     * @brief Return the hash size (in bytes).
     */
    virtual size_t hash_size() const override;

    /**
     * @brief Return the block size (in bytes).
     */
    virtual size_t block_size() const override;

    /**
     * @brief Update the hash with a chunk of buffer input data.
     * @param chunk The chunk of data to add to the hash.
     */
    virtual void update(const mud::core::buffer& chunk) override;

    /**
     * @brief Update the hash with a chunk of buffer view input data.
     * @param chunk The chunk of data to add to the hash.
     */
    virtual void update(const mud::core::buffer_view& chunk) override;

    /**
     * @brief Close or finalise the hash.
     */
    virtual void close() override;

    /**
     * @brief Return the calculated hash value.
     */
    virtual const mud::core::buffer& hash() const override;

    /**
     * @brief Reset the hashing algorithm.
     * @param hashing The hashing material.
     */
    virtual void reset(const hash_material_t& hashing) override;

private:
    /* Capacity */
    size_t _capacity;

    /* Digest bit size */
    size_t _digest_bit_size;

    /**
     * Implementation
     */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

/**
 * @brief The SHAKE digest.
 * @tparam Capacity The capacity. Typical values are either @c 128 and @c 256.
 * @details
 * Determines the SHAKE digest on binary input data. The digest can be
 * calculated by providing the entire data or in chunks. Note that the chunk
 * sizes are user-defined and do not need to correspond to the underlying
 * algorithm defined block sizes.
 *
 * To illustrate the determination of a SHAKE-128 producing a 512-bit digest of
 * a block of data @code
 *    mud::crypto::shake_128 hash(512);
 *    mud::crypto::mud::core::buffer chunk;
 *    do {
 *        chunk = mydata.next();
 *        hash.update(chunk);
 *    }
 *    while (chunk.size());
 *    hash.close();
 *    std::cout << "SHAKE-128 hash: " << hash.digest() << std::endl;
 * @endcode
 *
 * The hash can also be determined through a stream-buffer filter. The sample
 * below attaches a SHAKE-128 filter stream buffer to an input file stream. As
 * the file stream is copied to the console output, the digest is calculated.
 * @code
 *    mud::crypto::shake_128 hash(512);
 *    std::istream istr(hash.sbuf(myfile.rdbuf());
 *    std::cout << istr.rdbuf();
 *    hash.close();
 *    std::cout << "SHAKE-128 hash: " << hash.digest() << std::endl;
 * @endcode
 */
template<size_t Capacity>
class MUDLIB_CRYPTO_API shake : public shake_algorithm
{
public:
    /**
     * @brief Construct a SHAKE based calculator.
     * @param digest_bit_size The digest bit size.
     */
    shake(size_t digest_bit_size);

    /**
     * @brief Construct a SHAKE based calculator.
     * @param hashing The hashing material.
     * The @digest_size (in bits) is required to be defined in the @c hashing
     * material.
     */
    shake(const hash_material_t& hashing);
};

template<size_t Capacity>
shake<Capacity>::shake(size_t digest_bit_size)
    : shake_algorithm(Capacity, digest_bit_size)
{
}

template<size_t Capacity>
shake<Capacity>::shake(const hash_material_t& hashing)
    : shake_algorithm(Capacity, hashing.digest_size())
{
}

/**
 * @brief SHAKE-128
 */
typedef shake<256> shake_128;

/**
 * @brief SHAKE-256
 */
typedef shake<512> shake_256;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_SHA_H_ */
