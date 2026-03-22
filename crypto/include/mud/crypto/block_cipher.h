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

#ifndef _MUDLIB_CRYPTO_BLOCK_CIPHER_H_
#define _MUDLIB_CRYPTO_BLOCK_CIPHER_H_

#include <sstream>
#include <string_view>
#include <mud/crypto/ns.h>
#include <mud/crypto/padding.h>
#include <mud/crypto/types.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief An interface for a symmetric block cipher algorithm.
 * @details
 * The cipher functions for a block cipher algorithm. A block cipher generally
 * uses a symmetric key for both enciphering and decipphering text. When used
 * in conjunction with an operating mode, the key may be different for each
 * block.
 */
class MUDLIB_CRYPTO_API basic_algorithm
{
public:
    /**
     * @brief Return the algorithm specifications.
     */
    virtual constexpr std::string_view specs() const = 0;

    /**
     * @brief Encrypt plain text into cipher text using a key.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param key The algorithm key.
     */
    virtual void encrypt(const data_t& in, data_t& out, const key_t& key) = 0;

    /**
     * @brief Decrypt cipher text into plain text using a key.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param key The algorithm key.
     */
    virtual void decrypt(const data_t& in, data_t& out, const key_t& key) = 0;
};

/**
 * @brief An inteface for a block cipher operating mode.
 * @details
 * The functions for an algortihm block cipher mode. The operating mode defines
 * how keys and block ciphers are used when multiple blocks are required. This
 * would commonly use some form of feedback and initial vectors.
 */
class MUDLIB_CRYPTO_API basic_mode
{
public:
    /**
     * @brief The cipher to use for encryption and decryption. 
     * @details
     * A forward cipher is usually used for encryption from plain text into
     * cipher text. A reverse cipher is usually used for decryption of the
     * cipher text back into plain text. Some modes use the forward cipher
     * both for encryption and decryption.
     */
    enum class direction_t {
        forward,
        reverse
    };

    /**
     * @brief Return the mode specifications.
     */
    virtual constexpr std::string_view specs() const = 0;

    /**
     * @brief Set the block size
     */
    void block_size(size_t size) { _block_size = size; }

    /**
     * @brief Get the block size
     */
    size_t block_size() const { return _block_size; }

    /**
     * @brief Retrieve the next block for processing.
     * @param bptr The pointer to the start of the block
     * @param bend The pointer past the end of all data
     * @return The block to be processed.
     * @details
     * A data block is assembled from the input data pointers and is to be of a
     * specified block-size. The block returned might be empty if there is no
     * more data available (@c bptr >= @c bend).
     * Default implementation copies the @c block_size amount of bytes from
     * @c bptr to the data block to be returned.
     */
    virtual data_t next(const uint8_t* bptr, const uint8_t* end);

    /**
     * @brief Prelude a block for encryption.
     * @param input The block to be encrypted.
     * @return The block to be encrypted.
     * @details
     * Perform any prelude action before encrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be encrypted.
     * Returns any possible altered input block that is ready to be encrypted.
     * Default implementation returns @c input as-is.
     */
    virtual data_t prelude_encrypt(const data_t& input);

    /**
     * @brief Postlude a block after encryption.
     * @param output The encrypted block.
     * @return The final encrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     * Default implementation returns @c output as-is.
     */
    virtual data_t postlude_encrypt(const data_t& output);

    /**
     * @brief Prelude a block for decryption.
     * @param input The block to be decrypted.
     * @return The block to be decrypted.
     * @details
     * Perform any prelude action before decrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be decrypted.
     * Returns any possible altered input block that is ready to be decrypted.
     * Default implementation returns @c input as-is.
     */
    virtual data_t prelude_decrypt(const data_t& input);

    /**
     * @brief Postlude a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     * Default implementation returns @c output as-is.
     */
    virtual data_t postlude_decrypt(const data_t& output);

    /**
     * @brief Return the key for the next block operation.
     * @details
     * Based on the implementation of the mode, the key may be modified during
     * the preparation of each block operations.
     */
    key_t& key() { return _key; }
    const key_t& key() const { return _key; }

    /**
     * @brief Set the padding type.
     * @param type The padding type to apply.
     */
    void padding(basic_padding::type_t type);

    /**
     * @brief Return the padding mode.
     */
    basic_padding::type_t padding() const;

protected:
    /**
     * @brief Create basic mode.
     * @param key The key.
     * @details
     * Create the basic-mode with PCKS#7 padding.
     */
    basic_mode(size_t block_size, const key_t& key)
        : _block_size(block_size)
        , _key(key)
    {};

    /**
     * @brief Return the padding algorithm.
     */
    const basic_padding& padder() const {
        return *_padder;
    }

private:
    /** The block size */
    size_t _block_size;

    /** The key */
    key_t _key;

    /** The padding algorithm */
    std::unique_ptr<basic_padding> _padder;
};

/**
 * @brief Electronic Codebook (ECB) mode.
 * @details
 * Default padding type is PKCS#7.
 */
class MUDLIB_CRYPTO_API ecb: public basic_mode
{
public:
    /** The name of the mode */
    static constexpr std::string_view name = "ECB";

    /** The direction for encryption and decryption. */
    static constexpr direction_t encryption_direction = direction_t::forward;
    static constexpr direction_t decryption_direction = direction_t::reverse;

    /**
     * @brief Create an ECB mode.
     * @param block_size The block size.
     * @param key The key.
     */
    ecb(size_t block_size, const key_t& key);

    /**
     * @brief ECB modes do not use an initialisation vector.
     */
    ecb(size_t, const key_t&, const iv_t&) = delete;

    /**
     * @brief ECB modes do not use a counter.
     */
    ecb(size_t, const key_t&, const counter_t&) = delete;

    /**
     * @brief ECB modes do not use a nonce and counter.
     */
    ecb(size_t, const key_t&, const nonce_t&, const counter_t&) = delete;

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /**
     * @brief Retrieve the next block for processing.
     * @param bptr The pointer to the start of the block
     * @param bend The pointer past the end of all data
     * @return The block to be processed.
     * @details
     * A data block is assembled from the input data pointers and is to be of a
     * specified block-size. The block returned might be empty if there is no
     * more data available (@c bptr >= @c bend).
     * Default implementation copies the @c block_size amount of bytes from
     * @c bptr to the data block to be returned.
     */
    virtual data_t next(const uint8_t* bptr, const uint8_t* end) override;

    /**
     * @brief Prelude a block for encryption.
     * @param input The block to be encrypted.
     * @return The block to be encrypted.
     * @details
     * Perform any prelude action before encrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be encrypted.
     * Returns any possible altered input block that is ready to be encrypted.
     */
    virtual data_t prelude_encrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after encryption.
     * @param output The encrypted block.
     * @return The final encrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_encrypt(const data_t& output) override;

    /**
     * @brief Prelude a block for decryption.
     * @param input The block to be decrypted.
     * @return The block to be decrypted.
     * @details
     * Perform any prelude action before decrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be decrypted.
     */
    virtual data_t prelude_decrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_decrypt(const data_t& output) override;

private:
    /** Flag to indicate that a final block is to be processed. */
    bool _final;

    /** Flag to indicate there is no more data */
    bool _eos;
};

/**
 * @brief Cipher Block Chaining (CBC) mode.
 * @details
 * Default padding type is PKCS#7.
 */
class MUDLIB_CRYPTO_API cbc: public basic_mode
{
public:
    /** The name of the mode */
    static constexpr std::string_view name = "CBC";

    /** The direction for encryption and decryption. */
    static constexpr direction_t encryption_direction = direction_t::forward;
    static constexpr direction_t decryption_direction = direction_t::reverse;

    /**
     * @brief CBC mode needs additional parameters.
     */
    cbc(size_t, const key_t&) = delete;

    /**
     * @brief Create a CBC mode.
     * @param block_size The block size.
     * @param key The key.
     * @param iv The initialisation vector.
     */
    cbc(size_t block_size, const key_t& key, const iv_t& iv);

    /**
     * @brief CBC modes do not use a counter.
     */
    cbc(size_t, const key_t&, const counter_t&) = delete;

    /**
     * @brief CBC modes do not use a nonce and counter.
     */
    cbc(size_t, const key_t&, const nonce_t&, const counter_t&) = delete;

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /**
     * @brief Retrieve the next block for processing.
     * @param bptr The pointer to the start of the block
     * @param bend The pointer past the end of all data
     * @return The block to be processed.
     * @details
     * A data block is assembled from the input data pointers and is to be of a
     * specified block-size. The block returned might be empty if there is no
     * more data available (@c bptr >= @c bend).
     * Default implementation copies the @c block_size amount of bytes from
     * @c bptr to the data block to be returned.
     */
    virtual data_t next(const uint8_t* bptr, const uint8_t* end) override;

    /**
     * @brief Prelude a block for encryption.
     * @param input The block to be encrypted.
     * @return The block to be encrypted.
     * @details
     * Perform any prelude action before encrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be encrypted.
     * Returns any possible altered input block that is ready to be encrypted.
     */
    virtual data_t prelude_encrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after encryption.
     * @param output The encrypted block.
     * @return The final encrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_encrypt(const data_t& output) override;

    /**
     * @brief Prelude a block for decryption.
     * @param input The block to be decrypted.
     * @return The block to be decrypted.
     * @details
     * Perform any prelude action before decrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be decrypted.
     */
    virtual data_t prelude_decrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_decrypt(const data_t& output) override;

private:
    /** The initialisation vector */
    iv_t _iv;

    /** The previous cipher block */
    data_t _previous;

    /** The temporary cipher block */
    data_t _tmp;

    /** Flag to indicate that a final block is to be processed. */
    bool _final;

    /** Flag to indicate there is no more data */
    bool _eos;
};

/**
 * @brief Cipher Feedback (CFB-128) mode.
 * @details
 * By default, no padding is applied.
 */
class MUDLIB_CRYPTO_API cfb: public basic_mode
{
public:
    /** The name of the mode */
    static constexpr std::string_view name = "CFB";

    /** The direction for encryption and decryption. */
    static constexpr direction_t encryption_direction = direction_t::forward;
    static constexpr direction_t decryption_direction = direction_t::forward;

    /**
     * @brief CFB mode needs additional parameters.
     */
    cfb(size_t, const key_t&) = delete;

    /**
     * @brief Create a CFB mode.
     * @param block_size The block size.
     * @param key The key.
     * @param iv The initialisation vector.
     */
    cfb(size_t block_size, const key_t& key, const iv_t& iv);

    /**
     * @brief CFB modes do not use a counter.
     */
    cfb(size_t, const key_t&, const counter_t&) = delete;

    /**
     * @brief CFB modes do not use a nonce and counter.
     */
    cfb(size_t, const key_t&, const nonce_t&, const counter_t&) = delete;

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /**
     * @brief Retrieve the next block for processing.
     * @param bptr The pointer to the start of the block
     * @param bend The pointer past the end of all data
     * @return The block to be processed.
     * @details
     * A data block is assembled from the input data pointers and is to be of a
     * specified block-size. The block returned might be empty if there is no
     * more data available (@c bptr >= @c bend).
     * Default implementation copies the @c block_size amount of bytes from
     * @c bptr to the data block to be returned.
     */
    virtual data_t next(const uint8_t* bptr, const uint8_t* end) override;

    /**
     * @brief Prelude a block for encryption.
     * @param input The block to be encrypted.
     * @return The block to be encrypted.
     * @details
     * Perform any prelude action before encrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be encrypted.
     */
    virtual data_t prelude_encrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after encryption.
     * @param output The encrypted block.
     * @return The final encrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_encrypt(const data_t& output) override;

    /**
     * @brief Prelude a block for decryption.
     * @param input The block to be decrypted.
     * @return The block to be decrypted.
     * @details
     * Perform any prelude action before decrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be decrypted.
     */
    virtual data_t prelude_decrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_decrypt(const data_t& output) override;

private:
    /** The initialisation vector */
    iv_t _iv;

    /** The previous cipher block */
    data_t _previous;

    /** The text block */
    data_t _text;
};

/**
 * @brief Counter (CTR) mode
 * @details
 * By default, no padding is applied.
 */
class MUDLIB_CRYPTO_API ctr: public basic_mode
{
public:
    /** The name of the mode */
    static constexpr std::string_view name = "CTR";

    /** The direction for encryption and decryption. */
    static constexpr direction_t encryption_direction = direction_t::forward;
    static constexpr direction_t decryption_direction = direction_t::forward;

    /**
     * @brief CTR mode needs additional parameters.
     */
    ctr(size_t, const key_t& k) = delete;

    /**
     * @brief CTR modes do not use an initialisation vector.
     */
    ctr(size_t, const key_t&, const iv_t&) = delete;

    /**
     * @brief CTR modes.
     * @param block_size The block size.
     * @param key The key.
     * @param counter The initial counter
     */
    ctr(size_t block_size, const key_t&, const counter_t& counter);

    /**
     * @brief CTR modes do not use a nonce and counter.
     */
    ctr(size_t, const key_t&, const nonce_t&, const counter_t&) = delete;

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return name; }

    /**
     * @brief Prelude a block for encryption.
     * @param input The block to be encrypted.
     * @return The block to be encrypted.
     * @details
     * Perform any prelude action before encrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be encrypted.
     */
    virtual data_t prelude_encrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after encryption.
     * @param output The encrypted block.
     * @return The final encrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_encrypt(const data_t& output) override;

    /**
     * @brief Prelude a block for decryption.
     * @param input The block to be decrypted.
     * @return The block to be decrypted.
     * @details
     * Perform any prelude action before decrypting the @c input block. This may
     * alter the mode's internal state and may alter the block to be decrypted.
     */
    virtual data_t prelude_decrypt(const data_t& input) override;

    /**
     * @brief Postlude a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     */
    virtual data_t postlude_decrypt(const data_t& output) override;

private:
    /** The counter */
    counter_t _counter;

    /** The text block */
    data_t _text;
};

/**
 * @brief A block-cipher is a cryptographic algorithm that operates on a fixed
 * length of bits.
 * @tparam Algorithm The block cipher algorithm type.
 * @tparam Mode The operating mode
 * @details
 * The @c block_cipher class defines the traits of a typical cryptographic
 * algorithm that operates on a fixed length of bits. Block ciphers are often
 * the basic cornerstone of advanced cryptographic protocols. By itself, the
 * block cipher algorithm is deterministic, where the same input shall provide
 * the same output. But in combination with an operating mode, a more advanced
 * protocol can be built. The operating mode defines how the output of one
 * block can be fed into the system to enhance the security of the overall
 * cryptographic protocol.
 */
template<typename Algorithm, typename Mode>
class block_cipher
{
public:
    /** The type of block cipher algorithm */
    typedef Algorithm algorithm_t;

    /** The algorithm oeprating mode type */
    typedef Mode mode_t;

    /** The block-size as defined by the @c algorithm_t */
    inline static constexpr size_t block_size = Algorithm::block_size;

    /** The key-size as defined by the @c algorithm_t */
    inline static constexpr size_t key_size = Algorithm::key_size;

    /** The name of the @c algorithm_t */
    inline static constexpr std::string_view algorithm_name = Algorithm::name;

    /** The name of the @c mode_t */
    inline static constexpr std::string_view mode_name = Mode::name;

    /**
     * @brief Create a block cipher.
     * @param key The key to use.
     * @details
     * Create a block cipher whose operating mode does not require additional
     * parameters.
     */
    block_cipher(const key_t& key);

    /**
     * @brief Create a block cipher.
     * @param key The key to use.
     * @param iv The initialisation vector.
     * @details
     * Create a block cipher whose operating mode requires an initialisation
     * vector.
     */
    block_cipher(const key_t& k, const iv_t& iv);

    /**
     * @brief Create a block cipher.
     * @param counter The initial counter
     * @details
     * Create a block cipher whose operating mode requires a initial counter.
     */
    block_cipher(const key_t& k, const counter_t& counter);

    /**
     * @brief Create a block cipher.
     * @param key The key to use.
     * @param nonce The nonce.
     * @param counter The counter.
     * @details
     * Create a block cipher whose operating mode requires a nonce and a
     * counter.
     */
    block_cipher(const key_t& key, const nonce_t& nonce,
                 const counter_t& counter);

    /**
     * @brief Return the block cipher specifications.
     */
    std::string specs() const;

    /**
     * @brief Encrypt plain text into cipher text using a key.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param key The algorithm key.
     */
    void encrypt(const data_t& plain, data_t& cipher);

    /**
     * @brief Decrypt cipher text into plain text using a key.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param key The algorithm key.
     */
    void decrypt(const data_t& cipher, data_t& plain);

    /**
     * @brief Set the padding type.
     * @param type The padding type to apply.
     * @details
     * Overrides the default padding type of the underlying mode.
     */
    void padding(basic_padding::type_t type) { _mode.padding(type); }

    /**
     * @brief Return the padding type.
     */
    basic_padding::type_t padding() const { return _mode.padding(); }

private:
    algorithm_t _algorithm;
    mode_t _mode;
};

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::block_cipher(const key_t& key)
    : _mode(block_size, key)
{
}

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::block_cipher(const key_t& key, const iv_t& iv)
    : _mode(block_size, key, iv)
{
}

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::block_cipher(const key_t& key,
        const counter_t& counter)
    : _mode(block_size, key, counter)
{
}

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::block_cipher(const key_t& key,
        const nonce_t& nonce, const counter_t& counter)
    : _mode(block_size, key, nonce, counter)
{
}

template<typename Algorithm, typename Mode>
void
block_cipher<Algorithm, Mode>::encrypt(const data_t& plain, data_t& cipher)
{
    cipher.clear();
    auto* bptr = plain.data();
    auto* bend = plain.data() + plain.size();
    while (true) {
        /* Prelude the next block for encryption */
        auto input = _mode.next(bptr, bend);
        input = _mode.prelude_encrypt(input);
        if (input.size() == 0) {
            break;
        }

        /* Perform the encryption */
        data_t output;
        if (_mode.encryption_direction == basic_mode::direction_t::forward) {
            _algorithm.encrypt(input, output, _mode.key());
        }
        else {
            _algorithm.decrypt(input, output, _mode.key());
        }

        /* Postlude the result */
        output = _mode.postlude_encrypt(output);
        cipher.append(output);
        bptr += block_size;
    }
}

template<typename Algorithm, typename Mode>
void
block_cipher<Algorithm, Mode>::decrypt(const data_t& plain, data_t& cipher)
{
    auto* bptr = plain.data();
    auto* bend = plain.data() + plain.size();
    while (true) {
        /* Prelude the next block for decryption */
        auto input = _mode.next(bptr, bend);
        input = _mode.prelude_decrypt(input);
        if (input.size() == 0) {
            break;
        }

        /* Perform the decryption */
        data_t output;
        if (_mode.decryption_direction == basic_mode::direction_t::forward) {
            _algorithm.encrypt(input, output, _mode.key());
        }
        else {
            _algorithm.decrypt(input, output, _mode.key());
        }

        /* Postlude the result */
        output = _mode.postlude_decrypt(output);
        cipher.append(output);
        bptr += block_size;
    }
}

template<typename Algorithm, typename Mode>
std::string
block_cipher<Algorithm, Mode>::specs() const
{
    std::stringstream sstr;
    sstr << _algorithm.specs() << '-' << key_size << '-' << _mode.specs();
    return sstr.str();
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_BLOCK_CIPHER_H_ */
