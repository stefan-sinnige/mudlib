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

#ifndef _MUDLIB_CRYPTO_HASH_H_
#define _MUDLIB_CRYPTO_HASH_H_

#include <sstream>
#include <streambuf>
#include <string>
#include <mud/crypto/ns.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/*
 * Forward declaration.
 */
class hash_streambuf;

/**
 * @brief An interface for a hash algorithm.
 * @details
 * A hash algorithm calculates a fixed size binary string over a variable sized
 * binary input string in such a manner that
 *  - The hash value is deterministic, the same input produces the same hash
 *  - It is infeasible to reverse the hash value back to the original input
 *  - It is computationally infeasible to find a different input that produces
 *    a specific hash
 *  - It is computationally infeasible to find any two distinct input that
 *    produces the same hash
 *  - A small change in the input produces a significant change in the hash
 * The hash is usually calculated by providing the input binary string in chunks
 * which may not be related to the underlying block-sizes used by the hash
 * implementation. Once all data has been supplied to the hash function, an
 * explicit @c close will finalise the resulting hash value.
 */
class MUDLIB_CRYPTO_API basic_hash
{
public:
    /**
     * @brief Return the hash specifications.
     */
    virtual std::string specs() const = 0;

    /**
     * @brief Return the hash size.
     */
    virtual size_t hash_size() const = 0;

    /**
     * @brief Default constructor initialises the hash, using default hashing
     * material.
     */
    basic_hash() = default;

    /**
     * @brief Default constructor initialises the hash.
     * @brief hashing The hashing material.
     */
    basic_hash(const material_t& hashing);

    /**
     * @brief Not copyable
     */
    basic_hash(const basic_hash&) = delete;

    /**
     * @brief Not moveable
     */
    basic_hash(basic_hash&&) = delete;

    /**
     * @brief Not copy-assignable
     */
    basic_hash& operator=(const basic_hash&) = delete;

    /**
     * @brief Not move-assignable
     */
    basic_hash& operator=(basic_hash&&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~basic_hash();

    /**
     * @brief Update the hash with a chunk of binary input data.
     * @param chunk The chunk of data to add to the hash.
     */
    virtual void update(const data_t& chunk) = 0;

    /**
     * @brief Close or finalise the hash.
     */
    virtual void close() = 0;

    /**
     * @brief Return the calculated hash value.
     */
    virtual const data_t& hash() const = 0;

    /**
     * @brief Return the hashing material.
     */
    const material_t& hashing() const { return _hashing; }

    /**
     * @brief Create and return a filtering stream buffer for calculating hash
     * values.
     * @param chain The chained stream buffer.
     * @details
     * The stream buffer can be used in conjunction with any @c std::istream
     * to decrypt of @c std::ostream to calculate the hash value while the
     * data is streamed.
     */
    virtual hash_streambuf* sbuf(std::streambuf* chain);

    /**
     * @brief Return the associated stream buffer.
     * @return A pointer to the previously created stream-buffer or @c nullptr.
     */
    virtual hash_streambuf* sbuf() const { return _sbuf; }

private:
    /* The stream buffer */
    hash_streambuf* _sbuf = nullptr;

    /* The hashing material */
    material_t _hashing;
};

/**
 * @brief Stream buffer to apply hash calculations.
 */
class MUDLIB_CRYPTO_API hash_streambuf: public std::streambuf
{
public:
    /**
     * @brief Create a streambuffer that calculates a hash.
     * @param chain The chained stream buffer.
     * @details
     * Using the @c chain stream-buffer for reading and writing, the @c
     * hash_streambuf is a filtering stream buffer that calculates the hash
     * value for all input and output stream data. 
     */
    hash_streambuf(std::streambuf* chain, basic_hash& hash);

    /**
     * @brief Destructor
     * Close and destruct this stream buffer, but not the chained stream buffer.
     */
    virtual ~hash_streambuf();

    /**
     * @brief Explicitely close the stream buffer.
     * @details
     * When the stream buffer is closed it shall no longer be usable and the
     * final hash value will be finalised.
     */
    void close();

protected:
    /**
     * @brief Reading characters from the associated input sequence while
     * decrypting.
     * @return The value pointed to by the @c gptr or @c traits::eof() if no
     * more data is available.
     */
    int underflow() override;

    /**
     * @brief Writing characters to the associated input sequence while
     * encrypting.
     * @param ch The character to store.
     * @return The value @c ch or @c traits::eof() on error.
     */
    int overflow(int c) override;

    /**
     * @brief Synchronise the buffer with the character sequence.
     * @return @c 0 upon success, or @c -1 on failure.
     */
    int sync() override;

private:
    /** The next stream buffer */
    std::streambuf* _chain;

    /** The hash algorithm */
    basic_hash& _hash;

    /** The buffer size (excluding put-back area) */
    const size_t _bufsize;

    /** The putback size */
    const size_t _putbacksize;

    /** The buffer */
    char* _buffer;

    /** Flag to indicate that the stream has reached the end */
    bool _eof;
};

/**
 * @brief Factory for hashes, indexed by the hash specification.
 */
typedef ::mud::core::factory<std::string,
                             mud::crypto::basic_hash,
                             mud::crypto::material_t>
        hash_factory;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_HASH_H_ */
