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

#ifndef _MUDLIB_CRYPTO_MAC_H_
#define _MUDLIB_CRYPTO_MAC_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/*
 * Forward declaration.
 */
class mac_streambuf;

/**
 * @brief An interface for a Message Authentication Code (MAC) algorithm.
 * @details
 * A MAC algorithm is binary information (usually short) used for authenticating
 * and verifyring the integerity of a message. Formally, it consists of a
 * triplet of algorithms that define
 *  - Key generation
 *  - Signing
 *  - Verification
 */
class MUDLIB_CRYPTO_API basic_mac
{
public:
    /**
     * @brief Return the mac specifications.
     */
    virtual std::string specs() const = 0;

    /**
     * @brief Default constructor initialises the mac.
     * @brief keying The keying material.
     */
    basic_mac(const mac_material_t& keying);

    /**
     * @brief No default constructor.
     */
    basic_mac() = delete;

    /**
     * @brief Not copyable
     */
    basic_mac(const basic_mac&) = delete;

    /**
     * @brief Not moveable
     */
    basic_mac(basic_mac&&) = delete;

    /**
     * @brief Not copy-assignable
     */
    basic_mac& operator=(const basic_mac&) = delete;

    /**
     * @brief Not move-assignable
     */
    basic_mac& operator=(basic_mac&&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~basic_mac();

    /**
     * @brief Return the keying material.
     */
    const mac_material_t& keying() const { return _keying; }

    /**
     * @brief Update the MAC with a chunk of binary input data.
     * @param chunk The chunk of data to add to the MAC.
     */
    virtual void update(const data_t& chunk) = 0;

    /**
     * @brief Close or finalise the MAC.
     */
    virtual void close() = 0;

    /**
     * @brief Reset the algorithm.
     * @param keying The keying material.
     */
    virtual void reset(const mac_material_t& keying) = 0;

    /**
     * @brief Return the calculated message tag.
     */
    virtual const data_t& tag() const = 0;

    /**
     * @brief Create and return a filtering stream buffer for calculating MAC
     * values.
     * @param chain The chained stream buffer.
     * @details
     * The stream buffer can be used in conjunction with any @c std::stream
     * to calculate the MAC while the dat is streamed.
     */
    virtual mac_streambuf* sbuf(std::streambuf* chain);

    /**
     * @brief Return the associated stream buffer.
     * @return A pointer to the previously created stream-buffer or @c nullptr.
     */
    virtual mac_streambuf* sbuf() const { return _sbuf; }

private:
    /* The stream buffer */
    mac_streambuf* _sbuf = nullptr;

    /* The keying material */
    mac_material_t _keying;
};

/**
 * @brief Stream buffer to apply MAC calculations.
 */
class MUDLIB_CRYPTO_API mac_streambuf: public std::streambuf
{
public:
    /**
     * @brief Create a streambuffer that calculates a MAC.
     * @param chain The chained stream buffer.
     * @details
     * Using the @c chain stream-buffer for reading and writing, the @c
     * mac_streambuf is a filtering stream buffer that calculates the MAC
     * value for all input and output stream data. 
     */
    mac_streambuf(std::streambuf* chain, basic_mac& mac);

    /**
     * @brief Destructor
     * Close and destruct this stream buffer, but not the chained stream buffer.
     */
    virtual ~mac_streambuf();

    /**
     * @brief Explicitely close the stream buffer.
     * @details
     * When the stream buffer is closed it shall no longer be usable and the
     * final MAC value will be finalised.
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

    /** The mac algorithm */
    basic_mac& _mac;

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
 * @brief Factory for MACs, indexed by the MAC specification.
 */
typedef ::mud::core::factory<std::string,
                             mud::crypto::basic_mac,
                             mud::crypto::mac_material_t>
        mac_factory;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_MAC_H_ */
