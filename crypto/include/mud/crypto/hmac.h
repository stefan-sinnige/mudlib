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

#ifndef _MUDLIB_CRYPTO_HMAC_H_
#define _MUDLIB_CRYPTO_HMAC_H_

#include <cstring>
#include <memory>
#include <mud/crypto/ns.h>
#include <mud/crypto/hash.h>
#include <mud/crypto/mac.h>
#include <mud/crypto/sha.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief Base interface extension for a Hash based Message Authentication Code
 * (HMAC)
 */
class MUDLIB_CRYPTO_API basic_hmac: public basic_mac
{
public:
    /**
     * @brief Default constructor initialises the mac.
     * @brief keying The keying material.
     */
    basic_hmac(const mac_material_t& keying);

    /**
     * @brief Return te underlying hash-algorithm hash size.
     */
    virtual size_t hash_size() const = 0;
};

/**
 * @brief The Hash based Message Authentication Code (HMAC)
 * @tparam HashAlgorithm The Hash algorithm.
 * @details
 * The Hash based Message Authentication Code (HMAC) uses a hash function for
 * message authentication and integrity.
 *
 * To illustrate the determination of a HMAC of a block of data
 * @code
 *    mud::crypto::mac_material_t material;
 *    mud::crypto::hmac<mud::crypto::sha_256> hmac(material);
 *    mud::crypto::mud::core::buffer chunk;
 *    do {
 *        chunk = mydata.next();
 *        hmac.update(chunk);
 *    }
 *    while (chunk.size());
 *    hmac.close();
 *    std::cout << "HMAC: " << hash.tag() << std::endl;
 * @endcode
 *
 * The hash can also be determined through a stream-buffer filter. The sample
 * below attaches a SHAKE-128 filter stream buffer to an input file stream. As
 * the file stream is copied to the console output, the digest is calculated.
 * @code
 *    mud::crypto::mac_material_t material;
 *    mud::crypto::hmac<mud::crypto::sha_256> hmac(material);
 *    std::istream istr(hmac.sbuf(myfile.rdbuf());
 *    std::cout << istr.rdbuf();
 *    hmac.close();
 *    std::cout << "HMAC: " << hmac.tag() << std::endl;
 * @endcode
 *
 * This implementation is conform RFC-2104.
 */
template<typename HashAlgorithm>
class hmac: public basic_hmac
{
public:
    /* The hash algorithm */
    typedef HashAlgorithm hash_algorithm_t;

    /**
     * @brief Default constructor initialises the mac.
     * @brief keying The keying material.
     */
    hmac(const mac_material_t& keying);

    /**
     * @brief Destructor.
     */
    ~hmac() = default;

    /**
     * @brief Return the HKDF specification.
     */
    std::string specs() const override;

    /**
     * @brief Return te underlying hash-algorithm hash size.
     */
    size_t hash_size() const override;

    /**
     * @brief Update the MAC with a chunk of buffer input data.
     * @param chunk The chunk of data to add to the MAC.
     */
    void update(const mud::core::buffer& chunk) override;

    /**
     * @brief Update the MAC with a chunk of buffer view input data.
     * @param chunk The chunk of data to add to the MAC.
     */
    void update(const mud::core::buffer_view& chunk) override;

    /**
     * @brief Close or finalise the MAC.
     */
    void close() override;

    /**
     * @brief Reset the algorithm.
     * @param keying The keying material.
     */
    void reset(const mac_material_t& keying) override;

    /**
     * @brief Return the calculated message tag.
     */
    const mud::core::buffer& tag() const override;

private:
    /* The specificaton */
    std::string _specs;

    /* The hashing algorithm */
    hash_algorithm_t _hash;

    /* The key */
    key_t _key;

    /* The message tag */
    mud::core::buffer _tag;
};

template<typename HashAlgorithm>
hmac<HashAlgorithm>::hmac(const mac_material_t& keying)
    : basic_hmac(keying)
{
    reset(keying);
}

template<typename HashAlgorithm>
std::string
hmac<HashAlgorithm>::specs() const
{
    return _specs;
}

template<typename HashAlgorithm>
size_t
hmac<HashAlgorithm>::hash_size() const
{
    return _hash.hash_size();
}

template<typename HashAlgorithm>
void
hmac<HashAlgorithm>::update(const mud::core::buffer& chunk)
{
    _hash.update(chunk.view(chunk.cbegin(), chunk.cend()));
}

template<typename HashAlgorithm>
void
hmac<HashAlgorithm>::update(const mud::core::buffer_view& chunk)
{
    _hash.update(chunk);
}

template<typename HashAlgorithm>
void
hmac<HashAlgorithm>::close()
{
    /* Finalise the inner digest */
    _hash.close();
    mud::core::buffer inner_digest = _hash.hash();
    _hash.reset(keying().hash_material());

    /* Outer digest is one block of permutated key and padding */
    mud::core::buffer buf(_hash.block_size());
    memset(buf.data(), 0x5c, buf.size());
    for (int i = 0; i < _key.size(); ++i) {
        buf[i] = _key[i] ^ 0x5c;
    }

    /* Final MAC is the hash of the outer digest block and the inner digest */
    _hash.update(buf);
    _hash.update(inner_digest);
    _hash.close();

    /* Final tag, with possible truncation */
    if ((keying().tag_size() > 0) &&
        (keying().tag_size() < _hash.hash_size()))
    {
        _tag = mud::core::buffer(keying().tag_size());
        memcpy(_tag.data(), _hash.hash().data(), keying().tag_size());
    }
    else {
        _tag = _hash.hash();
    }
}

template<typename HashAlgorithm>
void
hmac<HashAlgorithm>::reset(const mac_material_t& keying)
{
    /* Reset the hash */
    _hash.reset(keying.hash_material());

    /* Define the specification as HMAC followed by the hash specification */
    _specs = "HMAC-" + _hash.specs();

    /* If the key is larger than the block-size, perform a hash of the key
     * data to ensure that the key is smaller than or equal to the hash size. */
    if (keying.key().size() > _hash.block_size()) {
        _hash.update(keying.key());
        _hash.close();
        _key = _hash.hash();
        _hash.reset(keying.hash_material());
    }
    else {
        _key = keying.key();
    }

    /* Inner digest is one block of permutated key and padding */
    mud::core::buffer buf(_hash.block_size());
    memset(buf.data(), 0x36, buf.size());
    for (int i = 0; i < _key.size(); ++i) {
        buf[i] = _key[i] ^ 0x36;
    }
    _hash.update(buf);

    /* The message data can follow now through (repeated) @c update. */
    _tag .clear();
}

template<typename HashAlgorithm>
const mud::core::buffer&
hmac<HashAlgorithm>::tag() const
{
    return _tag;
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_HMAC_H_ */
