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
 * @brief The SHA type algorithm.
 * @details
 * The SHA-1, SHA-2 and SHA-3 algorithms are accessible through a single
 * interface. The arguments passed at construction define the underlying SHA
 * algorithm type to use.
 *
 * This class can be used stand-alone, but the explicit @c typedef definitions
 * of supported algorithms is recommended. See the @c sha template and its
 * explicit specialisations for more information.
 */
class MUDLIB_CRYPTO_API sha_algorithm : public basic_hash
{
public:
    /**
     * @brief Construct a SHA algorithm
     * @param block_size The block size to operate on (bits).
     * @param word_size The word size (bits), for example @c 32 or @c 64.
     * @param digest_size The digest size (bits).
     * @throw mud::crypto::crypto_error if the combination of parameters yields
     * an unsupported algorithm.
     */
    sha_algorithm(size_t block_size, size_t word_size, size_t digest_size);

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
     * @brief Update the hash with a chunk of binary input data.
     * @param chunk The chunk of data to add to the hash.
     */
    virtual void update(const mud::core::buffer& chunk) override;

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
    /* Block-size */
    size_t _block_size;

    /* Word-size */
    size_t _word_size;

    /* Digest-size */
    size_t _digest_size;

    /**
     * Implementation
     */
    class impl;
    class sha_1_impl;
    template<typename WordType> class sha_2_impl;
    class sha_2_32_impl;
    class sha_2_64_impl;
    class sha_3_impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

/**
 * @brief The SHA digest.
 * @tparam BlockSize The hash block size in bits. Typical values are @c 512 and
 *         @c 1024.
 * @tparam WordSize The size of a word in bits. Typical values are @c 32 and
 *         @c 64
 * @tparam DigestSize The size of teh hash in bits. Typical values are @c 160,
 *         @c 224, @c 256, @c 384 and @c 512
 * @details
 * Determines the SHA digest on binary input data. The digest can be calculated
 * by providing the entire data or in chunks. Note that the chunk sizes are
 * user-defined and do not need to correspond to the underlying algorithm
 * defined block sizes.
 *
 * To illustrate the determination of a SHA-256 digest of a block of data
 * @code
 *    mud::crypto::sha_256 hash;
 *    mud::crypto::mud::core::buffer chunk;
 *    do {
 *        chunk = mydata.next();
 *        hash.update(chunk);
 *    }
 *    while (chunk.size());
 *    hash.close();
 *    std::cout << "SHA-256 hash: " << hash.digest() << std::endl;
 * @endcode
 *
 * The hash can also be determined through a stream-buffer filter. The sample
 * below attaches a SHA-256 filter stream buffer to an input file stream. As
 * the file stream is copied to the console output, the digest is calculated.
 * @code
 *    mud::crypto::sha_256 hash;
 *    std::istream istr(hash.sbuf(myfile.rdbuf());
 *    std::cout << istr.rdbuf();
 *    hash.close();
 *    std::cout << "SHA-256 hash: " << hash.digest() << std::endl;
 * @endcode
 */
template<size_t BlockSize, size_t WordSize, size_t DigestSize>
class MUDLIB_CRYPTO_API sha : public sha_algorithm
{
public:
    /**
     * @brief Construct a SHA-2 based calculator.
     */
    sha();

    /**
     * @brief Construct a SHA-2 based calculator.
     * @param hashing The hashing material.
     */
    sha(const hash_material_t& hashing);
};

template<size_t BlockSize, size_t WordSize,
         size_t DigestSize>
sha<BlockSize, WordSize, DigestSize>::sha()
    : sha_algorithm(BlockSize, WordSize, DigestSize)
{
}

template<size_t BlockSize, size_t WordSize,
         size_t DigestSize>
sha<BlockSize, WordSize, DigestSize>::sha(const hash_material_t& /* hashing */)
    : sha_algorithm(BlockSize, WordSize, DigestSize)
{
}

/**
 * @brief SHA-1 digest.
 *
 * @note This digest is no longer considered to be cryptographically secure.
 */
typedef sha<512, 32, 160> sha_1;

/**
 * @brief SHA-224 digist.
 */
typedef sha<512, 32, 224> sha_224;

/**
 * @brief SHA-256 digist.
 */
typedef sha<512, 32, 256> sha_256;

/**
 * @brief SHA-384 digist.
 */
typedef sha<1024, 64, 384> sha_384;

/**
 * @brief SHA-512 digist.
 */
typedef sha<1024, 64, 512> sha_512;

/**
 * @brief SHA-512/224 digist.
 */
typedef sha<1024, 64, 224> sha_512_224;

/**
 * @brief SHA-512/256 digist.
 */
typedef sha<1024, 64, 256> sha_512_256;

/**
 * @brief SHA3-224
 */
typedef sha<1152, 64, 224> sha3_224;

/**
 * @brief SHA3-256
 */
typedef sha<1088, 64, 256> sha3_256;

/**
 * @brief SHA3-384
 */
typedef sha<832, 64, 384> sha3_384;

/**
 * @brief SHA3-512
 */
typedef sha<576, 64, 512> sha3_512;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_SHA_H_ */
