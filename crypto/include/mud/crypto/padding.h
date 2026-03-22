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

#ifndef _MUDLIB_CRYPTO_PADDING_H_
#define _MUDLIB_CRYPTO_PADDING_H_

#include <string_view>
#include <mud/crypto/ns.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief An interface for padding algorithms.
 * @details
 * Padding is the process of adding arbitrary data. Cryptographic protocols,
 * like block ciphers and hash functions often rely on padding to ensure that
 * the data is of a certain size.
 *
 * Padded data is often distinguishable and can be automatically added and
 * removed as part of the cryptographic process. However, some padding types
 * cannot distinguish padded data from actual relevant data. Those types would
 * require other means to distinguish the padded data (for example by adding
 * a relevant data length as part of the message format).
 */
class MUDLIB_CRYPTO_API basic_padding
{
public:
    /**
    * @brief The suported padding types.
    */
    enum class type_t {
        none,
        pkcs7
    };

    /**
     * @brief Destructir
     */
    virtual ~basic_padding() = default;

    /**
     * @brief Return the padding specifications.
     */
    virtual constexpr std::string_view specs() const = 0;

    /**
     * @brief Add padding to data.
     * @param data The data to add the padding to.
     * @return The data with padding.
     */
    virtual data_t pad(const data_t& data) const = 0;

    /**
     * @brief Remove padding from data.
     * @param data The data to remove the padding from.
     * @return The data without padding.
     */
    virtual data_t unpad(const data_t& data) const = 0;

    /**
     * @brief Return the block-size.
     */
    size_t block_size() const { return _block_size; }

    /**
     * @brief Return the padding type.
     */
    type_t type() const { return _type; }

protected:
    /**
     * @brief Constructor defining the padding type.
     * @param type The padding type.
     */
    basic_padding(type_t type, size_t block_size)
        : _type(type), _block_size(block_size)
    {}

private:
    /** The padding type */
    type_t _type;

    /** The block size */
    size_t _block_size;
};

/**
 * @brief No padding
 * @details
 * Padding algorithm that does not perform any padding.
 */
class MUDLIB_CRYPTO_API none_padding : public basic_padding
{
public:
    /** The name of the padding algorithm */
    static constexpr std::string_view name = "none";

    /**
     * @brief Construct the no-padding algorithm.
     */
    none_padding(size_t block_size = 0)
        : basic_padding(basic_padding::type_t::none, block_size)
    {}

    /**
     * @brief Return the padding specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /** 
     * @brief Add padding to data.
     * @param data The data to add the padding to.
     * @return The data with padding.
     * @details
     * The data returned is the same as @c data.
     */
    data_t pad(const data_t& data) const override { return data; }
        
    /** 
     * @brief Remove padding from data.
     * @param data The data to remove the padding from.
     * @return The data without padding.
     * @details
     * The data returned is the same as @c data.
     */
    data_t unpad(const data_t& data) const override { return data; }
};

/**
 * @brief PKCS#7 padding
 * @details
 * PKCS#7 padding is defined as adding between 1 and @c block_size number of
 * bytes, where each byte has the value of the number of padded bytes:
 *
 * When adding padded data:
 *   - If the data size is smaller than @c block_size, pad up to the
 *     @c block_size
 *   - If the data size is equal to the @c block_size, or 0, add an entire
 *     block of padded data equal to the @c block_size.

 * When removing padded data:
 *   - Remove the number of bytes from the data that is equal to the value of
 *     the last byte.
 *   - Any padded byte removed should be equal in value.
 *
 * A @c padding_error exception is thrown when any discrepancy with the padding
 * algorithm is detected.
 */
class MUDLIB_CRYPTO_API pkcs7_padding : public basic_padding
{
public:
    /** The name of the padding algorithm */
    static constexpr std::string_view name = "PKCS#7";

    /**
     * @brief Construct the PKCS#7 padding algorithm.
     * @param block_size The block-siae of the data.
     */
    pkcs7_padding(size_t block_size);

    /**
     * @brief Return the padding specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /** 
     * @brief Add padding to data.
     * @param data The data to add the padding to.
     * @return The data with padding.
     * @details
     * The data returned is the same as @c data.
     */
    data_t pad(const data_t& data) const override;
        
    /** 
     * @brief Remove padding from data.
     * @param data The data to remove the padding from.
     * @return The data without padding.
     * @details
     * The data returned is the same as @c data.
     */
    data_t unpad(const data_t& data) const override;
};

/**
 * @brief The factory of padding algorithms.
 */
typedef ::mud::core::factory<mud::crypto::basic_padding::type_t,
                             mud::crypto::basic_padding,
                             size_t>
        padding_factory;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_PADDING_H_ */
