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
     * @brief Pad data at the begin.
     * @param sz The total size of the data after padding.
     * @param val The value of each padded byte.
     * @details
     * If the total size of the buffer is smaller than @c sz, the buffer will
     * be padded at the start of the buffer such that the total size will be @c
     * sz bytes. Each padded byte will have a value equal to @c val.
     */
    void pad_begin(size_t sz, uint8_t val = 0x00);

    /**
     * @brief Pad data at the end.
     * @param sz The total size of the data after padding.
     * @param val The value of each padded byte.
     * @details
     * If the total size of the buffer is smaller than @c sz, the buffer will
     * be padded at the end of the buffer such that the total size will be @c sz
     * bytes. Each padded byte will have a value equal to @c val.
     */
    void pad_end(size_t sz, uint8_t val = 0x00);

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

    /**
     * @brief Return the byte at the n-th position.
     * @param n The byte position (0 to size-1) to return, where 0 is the left
     * most position.
     * @return The byte at the position.
     */
    uint8_t operator[](size_t n) const;
    uint8_t& operator[](size_t n);

    /**
     * @brief Bit-wise OR assignment on equal sized blocks.
     * @param rhs The data to OR with.
     * @return The reference to this object.
     */
    data_t& operator|=(const data_t& rhs);

    /**
     * @brief Bit-wise AND assignment on equal sized blocks.
     * @param rhs The data to AND with.
     * @return The reference to this object.
     */
    data_t& operator&=(const data_t& rhs);

    /**
     * @brief Bit-wise XOR assignment on equal sized blocks.
     * @param rhs The data to XOR with.
     * @return The reference to this object.
     */
    data_t& operator^=(const data_t& rhs);

    /**
     * @brief Bit-wise left-shift assignment.
     * @param pos The amount of bits to shift.
     * @return The reference to this object.
     */
    data_t& operator<<=(size_t pos);

    /**
     * @brief Bit-wise right-shift assignment.
     * @param pos The amount of bits to shift.
     * @return The reference to this object.
     */
    data_t& operator>>=(size_t pos);

private:
    /** Friend operations */
    friend std::ostream& operator<<(std::ostream& ostr, const data_t& data);
    friend std::istream& operator>>(std::istream& istr, data_t& data);
    friend data_t operator|(const data_t& lhs, const data_t& rhs);
    friend data_t operator&(const data_t& lhs, const data_t& rhs);
    friend data_t operator^(const data_t& lhs, const data_t& rhs);
    friend data_t operator<<(const data_t& lhs, size_t pos);
    friend data_t operator>>(const data_t& lhs, size_t pos);
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
 * @brief Bitwise OR on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the OR of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
data_t operator|(const data_t& lhs, const data_t& rhs);

/**
 * @brief Bitwise AND on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the AND of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
data_t operator&(const data_t& lhs, const data_t& rhs);

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
 * @brief Bitwise shift left.
 * @param lhs The data to shift.
 * @param pos The amount of bits to shift.
 * @return The shifted result.
 */
data_t operator<<(const data_t& lhs, size_t pos);

/**
 * @brief Bitwise shift right.
 * @param lhs The data to shift.
 * @param pos The amount of bits to shift.
 * @return The shifted result.
 */
data_t operator>>(const data_t& lhs, size_t pos);

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
     * @brief Inherit all @c data_t constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Copy construct from a @c data_t object.
     * @param rhs The data to copy from.
     */
    key_t(const data_t& rhs): data_t(rhs) {}

    /**
     * @brief Move construct fro ma @c data_t objectt.
     * @param rhs The data to move from.
     */
    key_t(data_t&& rhs): data_t(rhs) {}
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
     * @brief Inherit all @c data_t constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Copy construct from a @c data_t object.
     * @param rhs The data to copy from.
     */
    iv_t(const data_t& rhs): data_t(rhs) {}

    /**
     * @brief Move construct fro ma @c data_t objectt.
     * @param rhs The data to move from.
     */
    iv_t(data_t&& rhs): data_t(rhs) {}
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
     * @brief Inherit all @c data_t constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Copy construct from a @c data_t object.
     * @param rhs The data to copy from.
     */
    nonce_t(const data_t& rhs): data_t(rhs) {}

    /**
     * @brief Move construct fro ma @c data_t objectt.
     * @param rhs The data to move from.
     */
    nonce_t(data_t&& rhs): data_t(rhs) {}
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
     * @brief Inherit all @c data_t constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Copy construct from a @c data_t object.
     * @param rhs The data to copy from.
     */
    counter_t(const data_t& rhs): data_t(rhs) {}

    /**
     * @brief Move construct fro ma @c data_t objectt.
     * @param rhs The data to move from.
     */
    counter_t(data_t&& rhs): data_t(rhs) {}

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

/**
 * @brief The type of padding.
 */
enum class padding_t {
    none,
    pkcs7
};

/**
 * @brief Keying material container.
 * @details
 * The keying material is a combination of various types that the underlying
 * cipher operations require. If a cipher operation requires certain keying
 * material that is not supplied or is incorrect, an exception shall be raised.
 */
class MUDLIB_CRYPTO_API material_t
{
public:
    /**
     * @brief Create no keying material.
     */
    material_t() = default;
#if 0
    /**
     * @brief Create a keying material combination.
     * @param key The key.
     */
    material_t(const key_t& key)
        : _key(key)
    {}

    /**
     * @brief Create a keying material combination.
     * @param key The key.
     * @param iv The initialisation vector.
     */
    material_t(const key_t& key, const iv_t& iv)
        : _key(key)
        , _iv(iv)
    {}

    /**
     * @brief Create a keying material combination.
     * @param key The key.
     * @param counter The initial counter.
     */
    material_t(const key_t& key, const counter_t& counter)
        : _key(key)
        , _counter(counter)
    {}

    /**
     * @brief Create a keying material combination.
     * @param key The key.
     * @param nonce The nonce.
     * @param counter The initial counter.
     */
    material_t(const key_t& key, const nonce_t& nonce, const counter_t& counter)
        : _key(key)
        , _nonce(nonce)
        , _counter(counter)
    {}
#endif
    /**
     * @brief Copy constructor.
     * @param rhs The material to copy from.
     */
    material_t(const material_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The material to move from.
     */
    material_t(material_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The material to copy from.
     */
    material_t& operator=(const material_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The material to move from.
     */
    material_t& operator=(material_t&& rhs) = default;

    /**
     * @brief Destructor.
     */
    ~material_t() = default;

    /**
     * @brief Set the key.
     * @param value The key value.
     */
    void key(const key_t& value) {  _key = value; }

    /**
     * @brief Return the key.
     */
    const key_t& key() const { return _key; }

    /**
     * @brief Set the initial vector.
     * @param value The initial vector value.
     */
    void iv(const iv_t& value) {  _iv = value; }

    /**
     * @brief Return the initial vector.
     */
    const iv_t& iv() const { return _iv; }

    /**
     * @brief Set the nonce.
     * @param value The nonce value.
     */
    void nonce(const nonce_t& value) {  _nonce = value; }

    /**
     * @brief Return the nonce.
     */
    const nonce_t& nonce() const { return _nonce; }

    /**
     * @brief Set the counter.
     * @param value The counter value.
     */
    void counter(const counter_t& value) {  _counter = value; }

    /**
     * @brief Return the counter.
     */
    const counter_t& counter() const { return _counter; }

    /**
     * @brief Set the additional authenticated data.
     * @param value The additional authenticated value.
     */
    void aad(const data_t& value) {  _aad = value; }

    /**
     * @brief Return the additional authenticated data.
     */
    const data_t& aad() const { return _aad; }

    /**
     * @brief Set the padding type.
     * @param value The padding type.
     */
    void padding(padding_t value) {  _padding = value; }

    /**
     * @brief Return the padding type.
     */
    padding_t padding() const { return _padding; }

private:
    /** The key */
    key_t _key;

    /** The initialisation vector */
    iv_t _iv;

    /** The nonce. */
    nonce_t _nonce;

    /** The counter. */
    counter_t _counter;

    /** The additional authenticated data. */
    data_t _aad;

    /** The padding type. */
    padding_t _padding = padding_t::none;
};

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_TYPES_H_ */
