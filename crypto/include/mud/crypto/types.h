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
     * @brief Resize the data.
     * @param sz The new size.
     * @details
     * If the size is smaller than the current size, the data at the end of
     * the data is truncated. If the size is larger than the current size, the
     * data is appended with zero-based padding.
     */
    void resize(size_t sz);

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

    /**
     * @brief Equality operator.
     * @param rhs The data to compare against.
     * @return @c true if the data is equal.
     */
    bool operator==(const data_t&) const;

    /**
     * @brief In-equality operator.
     * @param rhs The data to compare against.
     * @return @c true if the data is not equal..
     */
    bool operator!=(const data_t&) const;

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
 * @brief Cryptographic random number.
 * @details
 * A cryptographic random number using a non-deterministic uniform random bit
 * generator (e.g. a hardware device).
 */
class MUDLIB_CRYPTO_API random_t : public data_t
{
public:
    /**
     * @brief Inherit all @c data_t constructors.
     */
    using data_t::data_t;
    using data_t::operator=;

    /**
     * @brief Construct a random sequence of data.
     * @param sz The size of the sequence.
     */
    random_t(size_t sz);
};

/**
 * @brief Keying material base container.
 * @details
 * The keying material is a combination of various types that the underlying
 * cipher operations require. If a cipher operation requires certain keying
 * material that is not supplied or is incorrect, an exception shall be raised.
 *
 * Not all material is required for any cipher operation.
 */
class MUDLIB_CRYPTO_API material_t
{
};

/**
 * @brief Keying material container for cipher operations.
 * @details
 * The keying material is a combination of various types that the underlying
 * cipher operations require. If a cipher operation requires certain keying
 * material that is not supplied or is incorrect, an exception shall be raised.
 *
 * Not all material is required for any cipher operation.
 */
class MUDLIB_CRYPTO_API cipher_material_t: public material_t
{
public:
    /**
     * @brief Create no keying material.
     */
    cipher_material_t() = default;

    /**
     * @brief Copy constructor.
     * @param rhs The material to copy from.
     */
    cipher_material_t(const cipher_material_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The material to move from.
     */
    cipher_material_t(cipher_material_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The material to copy from.
     */
    cipher_material_t& operator=(const cipher_material_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The material to move from.
     */
    cipher_material_t& operator=(cipher_material_t&& rhs) = default;

    /**
     * @brief Destructor.
     */
    ~cipher_material_t() = default;

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
     * @brief Set the salt.
     * @param value The salt value.
     */
    void salt(const data_t& value) {  _salt = value; }

    /**
     * @brief Return the salt.
     */
    const data_t& salt() const { return _salt; }

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

    /**
     * @brief Set the block size in bits.
     * @param bits The block-size.
     * Cryptographic operations often operate on a certain block of data, whose
     * size is indicated by the block size. This may only need to be defined if
     * the operation does not use a fixed block-size.
     */
    void block_size(size_t bits) { _block_size = bits; }

    /**
     * @brief Get the block size in bits.
     */
    size_t block_size() const { return _block_size; }

    /**
     * @brief Set the word size in bits.
     * @param bits The word-size.
     * Cryptographic operations may use a concept of a 'word' which usually
     * indicates a certain a fixed-length sequence of bytes in a certain endian
     * representation.
     */
    void word_size(size_t bits) { _word_size = bits; }

    /**
     * @brief Get the word size in bits.
     */
    size_t word_size() const { return _word_size; }

    /**
     * @brief Set the digest size in bits.
     * @param bits The digest-size.
     * Cryptographic hashing operations create a digest (hash) of a certain
     * pre-determined size. Many cryptographics hashing algorithms produce a
     * fixed size. This may only need to be defined if te operation may produce
     * a variable digest size.
     */
    void digest_size(size_t value) { _digest_size = value; }

    /**
     * @brief Get the digest size in bits.
     */
    size_t digest_size() const { return _digest_size; }

private:
    /** The key */
    key_t _key;

    /** The initialisation vector */
    iv_t _iv;

    /** The nonce. */
    nonce_t _nonce;

    /** The counter. */
    counter_t _counter;

    /** The salt. */
    data_t _salt;

    /** The additional authenticated data. */
    data_t _aad;

    /** The padding type. */
    padding_t _padding = padding_t::none;

    /** The block size. */
    size_t _block_size = 0;

    /** The word size. */
    size_t _word_size = 0;

    /** The digest size. */
    size_t _digest_size = 0;
};

/**
 * @brief Keying material container for hash operations.
 * @details
 * The keying material is a combination of various types that the underlying
 * hash operations require. If a hash operation requires certain keying material
 * that is not supplied or is incorrect, an exception shall be raised.
 *
 * Not all material is required for any hash operation.
 */
class MUDLIB_CRYPTO_API hash_material_t: public material_t
{
public:
    /**
     * @brief Create no keying material.
     */
    hash_material_t() = default;

    /**
     * @brief Copy constructor.
     * @param rhs The material to copy from.
     */
    hash_material_t(const hash_material_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The material to move from.
     */
    hash_material_t(hash_material_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The material to copy from.
     */
    hash_material_t& operator=(const hash_material_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The material to move from.
     */
    hash_material_t& operator=(hash_material_t&& rhs) = default;

    /**
     * @brief Destructor.
     */
    ~hash_material_t() = default;

    /**
     * @brief Set the block size in bits.
     * @param bits The block-size.
     * Cryptographic operations often operate on a certain block of data, whose
     * size is indicated by the block size. This may only need to be defined if
     * the operation does not use a fixed block-size.
     */
    void block_size(size_t bits) { _block_size = bits; }

    /**
     * @brief Get the block size in bits.
     */
    size_t block_size() const { return _block_size; }

    /**
     * @brief Set the word size in bits.
     * @param bits The word-size.
     * Cryptographic operations may use a concept of a 'word' which usually
     * indicates a certain a fixed-length sequence of bytes in a certain endian
     * representation.
     */
    void word_size(size_t bits) { _word_size = bits; }

    /**
     * @brief Get the word size in bits.
     */
    size_t word_size() const { return _word_size; }

    /**
     * @brief Set the digest size in bits.
     * @param bits The digest-size.
     * Cryptographic hashing operations create a digest (hash) of a certain
     * pre-determined size. Many cryptographics hashing algorithms produce a
     * fixed size. This may only need to be defined if te operation may produce
     * a variable digest size.
     */
    void digest_size(size_t value) { _digest_size = value; }

    /**
     * @brief Get the digest size in bits.
     */
    size_t digest_size() const { return _digest_size; }

private:
    /** The block size. */
    size_t _block_size = 0;

    /** The word size. */
    size_t _word_size = 0;

    /** The digest size. */
    size_t _digest_size = 0;
};

/**
 * @brief Keying material container for MAC operations.
 * @details
 * The keying material is a combination of various types that the underlying
 * MAC operations require. If a MAC operation requires certain keying material
 * that is not supplied or is incorrect, an exception shall be raised.
 *
 * Not all material is required for any MAC operation.
 */
class MUDLIB_CRYPTO_API mac_material_t: public material_t
{
public:
    /**
     * @brief Create no keying material.
     */
    mac_material_t() = default;

    /**
     * @brief Copy constructor.
     * @param rhs The material to copy from.
     */
    mac_material_t(const mac_material_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The material to move from.
     */
    mac_material_t(mac_material_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The material to copy from.
     */
    mac_material_t& operator=(const mac_material_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The material to move from.
     */
    mac_material_t& operator=(mac_material_t&& rhs) = default;

    /**
     * @brief Destructor.
     */
    ~mac_material_t() = default;

    /**
     * @brief Set the hash material.
     * @param value The hash material value.
     */
    void hash_material(const hash_material_t& value) { _hash_material = value; }

    /**
     * @brief Return the hash material.
     */
    const hash_material_t& hash_material() const { return _hash_material; }

    /**
     * @brief Set the key.
     * @param value The key value.
     */
    void key(const key_t& value) { _key = value; }

    /**
     * @brief Return the key.
     */
    const key_t& key() const { return _key; }

    /**
     * @brief Set the truncated tag size.
     * @param value The tag size value, or @c 0 for the entire tag.
     */
    void tag_size(size_t value) { _tag_size = value; }

    /**
     * @brief Return the truncated tag size.
     * A tag size of @c 0 means no truncation.
     */
    size_t tag_size() const { return _tag_size; }

private:
    /** The hash material */
    hash_material_t _hash_material;

    /** Key */
    key_t _key;

    /** The tag size */
    size_t _tag_size = 0;
};

/**
 * @brief Keying material container for KDF operations.
 * @details
 * The keying material is a combination of various types that the underlying
 * KDF operations require. If a KDF operation requires certain keying material
 * that is not supplied or is incorrect, an exception shall be raised.
 *
 * Not all material is required for any KDF operation.
 */
class MUDLIB_CRYPTO_API kdf_material_t: public material_t
{
public:
    /**
     * @brief Create no keying material.
     */
    kdf_material_t() = default;

    /**
     * @brief Copy constructor.
     * @param rhs The material to copy from.
     */
    kdf_material_t(const kdf_material_t& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The material to move from.
     */
    kdf_material_t(kdf_material_t&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The material to copy from.
     */
    kdf_material_t& operator=(const kdf_material_t& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The material to move from.
     */
    kdf_material_t& operator=(kdf_material_t&& rhs) = default;

    /**
     * @brief Destructor.
     */
    ~kdf_material_t() = default;

    /**
     * @brief Set the MAC algortihm.
     * @param value The MAC algorithm.
     */
    void mac_algorithm(const std::string& value) { _mac_algorithm = value; }

    /**
     * @brief Return the MAC algorithm.
     */
    const std::string& mac_algorithm() const { return _mac_algorithm; }

    /**
     * @brief Set the Message Authentication Code (MAC)  material.
     * @param value The MAC material value.
     */
    void mac_material(const mac_material_t& value) { _mac_material = value; }

    /**
     * @brief Return the Message Authentication Code (MAC)  material.
     */
    const mac_material_t& mac_material() const { return _mac_material; }

    /**
     * @brief Set the input key material.
     * @param value The key value.
     */
    void ikm(const key_t& value) {  _ikm = value; }

    /**
     * @brief Return the input key material.
     */
    const key_t& ikm() const { return _ikm; }

    /**
     * @brief Set the salt.
     * @param value The salt value.
     */
    void salt(const data_t& value) {  _salt = value; }

    /**
     * @brief Return the salt.
     */
    const data_t& salt() const { return _salt; }

    /**
     * @brief Set the additional information.
     * @param value The information value.
     */
    void info(const data_t& value) {  _info = value; }

    /**
     * @brief Return the additional information.
     */
    const data_t& info() const { return _info; }

    /**
     * @brief Set the key size in bits.
     * @param bits The size of the generated output key.
     */
    void key_size(size_t value) { _key_size = value; }

    /**
     * @brief Get the key size in bits.
     */
    size_t key_size() const { return _key_size; }

private:
    /** The MAC algorithm */
    std::string _mac_algorithm;

    /** The MAC material */
    mac_material_t _mac_material;

    /** The input key material */
    key_t _ikm;

    /** The salt. */
    data_t _salt;

    /** The information. */
    data_t _info;

    /** The key size. */
    size_t _key_size = 0;
};

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_TYPES_H_ */
