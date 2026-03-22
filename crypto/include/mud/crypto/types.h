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

#ifndef _MUDLIB_CRYPTO_TYPES_H_
#define _MUDLIB_CRYPTO_TYPES_H_

#include <mud/crypto/ns.h>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <vector>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief Generic sequence of bytes.
 * @details
 * A generic storage for an arbitrary sequence of bytes. The context in which
 * the data is used determines its purpose. These can be message data that
 * represents cipher or plain text, or it can be extended to be used as a
 * special type like an initialisation vector, or counter object.
 */
class MUDLIB_CRYPTO_API data_t : private std::vector<uint8_t>
{
public:
    /**
     * @brief Constructing a sequence of data of a certain size.
     * @param sz The number of bytes to allocate.
     */
    explicit data_t(size_t sz = 0);

    /**
     * @brief Constructing a sequence of data through an initialiser.
     * @param init The initialiser list, as a container of @c uint8_t elements.
     */
    data_t(std::initializer_list<uint8_t> init);

    /**
     * @brief Construct from a string of bytes.
     * @param str The pointer to the string of bytes
     * @param sz The number of bytes.
     */
    data_t(const uint8_t* d, size_t sz);

    /**
     * @brief Construct from a vector of bytes.
     * @param vec The vector to copy from.
     */
    data_t(const std::vector<uint8_t>& rhs) : std::vector<uint8_t>(rhs) {}

    /**
     * @brief Construct from a vector of bytes.
     * @param vec The vector to move from.
     */
    data_t(std::vector<uint8_t>&& rhs) : std::vector<uint8_t>(rhs) {}

    /**
     * @brief Copy assignment from a vector of bytes.
     * @param vec The vector to copy from.
     * @return Reference to this object.
     */
    data_t& operator=(const std::vector<uint8_t>& rhs) {
        std::vector<uint8_t>::operator=(rhs);
        return *this;
    }

    /**
     * @brief Move assignment from a vector of bytes.
     * @param vec The vector to move from.
     * @return Reference to this object.
     */
    data_t& operator=(std::vector<uint8_t>&& rhs) {
        std::vector<uint8_t>::operator=(rhs);
        return *this;
    }

    /**
     * @brief Copy constructor.
     * @param rhs The item to copy from.
     */
    data_t(const data_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The item to move from.
     */
    data_t(data_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The item to copy from.
     * @return Reference to this object.
     */
    data_t& operator=(const data_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The item to move from.
     * @return Reference to this object.
     */
    data_t& operator=(data_t&& rhs) = default;

    /**
     * @brief Clear the data
     * @details
     * Clear the data and set the size to zero.
     */
    void clear();

    /**
     * @brief Appending data to the end.
     * @param data The data to append
     */
    void append(const data_t& data);

    /**
     * @brief Return the size of the byte storage.
     * @return The size of the storage.
     */
    size_t size() const { return std::vector<uint8_t>::size(); }

    /**
     * @brief Return the pointer to the underlying data.
     * @return Pointer to the begin of the string of bytes.
     */
    uint8_t* data() { return std::vector<uint8_t>::data(); }
    const uint8_t* data() const { return std::vector<uint8_t>::data(); }

private:
    /** Friend operations */
    friend std::ostream& operator<<(std::ostream& ostr, const data_t& data);
    friend std::istream& operator>>(std::istream& istr, data_t& data);
    friend data_t operator^(const data_t& lhs, const data_t& rhs);
};

/**
 * @brief Write cryptographic data as hexadecimal digits.
 * @param ostr The output stream to write to.
 * @param data The data value to output.
 * @return The reference to @c ostr;
 */
std::ostream& operator<<(std::ostream& ostr, const data_t& data);

/**
 * @brief Read hexadecimal digits to cryptographic data.
 * @param istr The input stream to write to.
 * @param data The data value to hold the data.
 * @return The reference to @c istr;
 */
std::istream& operator>>(std::istream& istr, const data_t& data);

/**
 * @brief Bitwise XOR on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the XOR of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
data_t operator^(const data_t& lhs, const data_t& rhs);

/**
 * @brief Cryptographic key.
 * @details
 * A cryptogrpahic key as represented by a data block. The key is generally
 * considered secret.
 */
class MUDLIB_CRYPTO_API key_t : public data_t
{
public:
    /**
     * @brief Inherit all @c block constructors.
     */
    using data_t::data_t;
    using data_t::operator=;
};

/**
 * @brief Cryptographic initialisation vector.
 * @details
 * An initialisation vector is a sequence of bytes that are used to provide a
 * randomised initial state. Cryptographic policies often require restrictions
 * on its usage, for example to only allow an initialisation vector to be
 * uniquely used for a specific key, effectively becoming a @c nonce.
 *
 * The initialisation vector is generally not considered to be secret and can
 * be supplied with the encrypted message.
 */
class MUDLIB_CRYPTO_API iv_t : public data_t
{
public:
    /**
     * @brief Inherit all @c block constructors.
     */
    using data_t::data_t;
    using data_t::operator=;
};

/**
 * @brief Cryptographic nonce.
 * @details
 * A @c nonce is a cryptographic sequence of bytes that is expected to be only
 * be used once.
 *
 * The nonce is generally not considered to be secret and can be supplied with
 * the encrypted message.
 */
class MUDLIB_CRYPTO_API nonce_t : public data_t
{
public:
    /**
     * @brief Inherit all @c block constructors.
     */
    using data_t::data_t;
    using data_t::operator=;
};

/**
 * @brief Cryptographic counter.
 * @details
 * A @c counter is a cryptographic sequence of bytes that is represented as a
 * counter value that is incremented.
 *
 * The counter is generally not considered to be secret and its initial value
 * can be supplied with the encrypted message.
 */
class MUDLIB_CRYPTO_API counter_t : public data_t
{
public:
    /**
     * @brief Inherit all @c block constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Constructing a counter of a certain size.
     * @param sz The number of bytes to allocate.
     * @param value The initial value.
     */
    explicit counter_t(size_t sz, uint64_t value);

    /**
     * @brief Increment the counter value by 1 (prefix).
     * @return reference to this object after the increment has been applied.
     */
    counter_t& operator++();
};

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_TYPES_H_ */
