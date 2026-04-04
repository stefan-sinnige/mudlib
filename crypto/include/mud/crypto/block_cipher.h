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
#include <streambuf>
#include <string_view>
#include <mud/crypto/ns.h>
#include <mud/crypto/padding.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

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
     * @brief Return the block size used by the algortihm.
     */
    virtual size_t block_size() const = 0;

    /**
     * @brief Return the key size used by the algortihm.
     */
    virtual size_t key_size() const = 0;

    /**
     * @brief Forward cipher (typically encrypt) plain text into cipher text
     * using a key.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param key The algorithm key.
     */
    virtual void forward(const data_t& in, data_t& out, const key_t& key) = 0;

    /**
     * @brief Inverse cipher (typycailly decrypt) text into plain text using a
     * key.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param key The algorithm key.
     */
    virtual void inverse(const data_t& in, data_t& out, const key_t& key) = 0;
};

/**
 * @brief An interface for a block cipher operating mode.
 * @details
 * The functions for an algortihm block cipher mode. The operating mode defines
 * how keys and block ciphers are used when multiple blocks are required. This
 * would commonly use some form of feedback and initial vectors.
 *
 * Most of the modes follow a similar implementation. As the operations occur
 * on cipher and plain text blocks, algorithm follow
 * @code
 *   while data available
 *       block ← next-block()
 *       input ← prelude(block)
 *       output ← algorithm-cipher(input)
 *       block ← postlude(output)
 * @endcode
 * Each interation would retrieve the @em next data block to be processed (plain
 * or cipher text). This data may need to be processed before any @em cipher
 * operation, during a @em prelude step. After the @em cipher operation, a
 * similar action may need to be taken at the @em postlude step. The result is
 * the processed block (plain or cipher text). It is the specific mode that
 * defines these particular sub-action. The @em cipher operation is the specific
 * block cipher algorithm.
 *
 * All operations defined are operating on a block of data (plain or cipher
 * text). The implementation of the mode defines the state between the
 * subsequent block operations.
 */
class MUDLIB_CRYPTO_API basic_mode
{
public:
    /**
     * @brief The cipher to use for encryption and decryption. 
     * @details
     * A forward cipher is usually used for encryption from plain text into
     * cipher text. An inverse cipher is usually used for decryption of the
     * cipher text back into plain text. Some modes use one cipher type for both
     * encryption and decryption.
     */
    enum class direction_t {
        forward,
        inverse
    };

    /**
     * @brief Return the mode specifications.
     */
    virtual constexpr std::string_view specs() const = 0;

    /**
     * @brief Get the block size
     */
    size_t block_size() const { return _algorithm.block_size(); }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    virtual direction_t encryption_direction() const = 0;

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    virtual direction_t decryption_direction() const = 0;

    /**
     * @brief Return the keying material.
     */
    const material_t& keying() const { return _keying; }

    /**
     * @brief Encrypt plain text block into cipher text.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param final Flag to indicate that this is the final block.
     */
    virtual void encrypt(const data_t& plain, data_t& cipher, bool final);

    /**
     * @brief Decrypt cipher text block into plain text.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param final Flag to indicate that this is the final block.
     */
    virtual void decrypt(const data_t& cipher, data_t& plain, bool final);

    /**
     * @brief Return the authentication tag.
     * @details
     * In case of authenticated encryption, return the authentication tag,
     * otherwise return a zero length buffer. The tag should only be valid after
     * the final block have been processed.
     */
    virtual data_t authentication_tag() const { return data_t(0); }

protected:
    /**
     * @brief Create basic mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     * @details
     * Create the basic-mode with PCKS#7 padding.
     */
    basic_mode(const material_t& keying, basic_algorithm& algorithm)
        : _keying(keying)
        , _padder(padding_factory::instance().create(
                _keying.padding(), _algorithm.block_size()))
        , _algorithm(algorithm)
    {};

    /**
     * @brief Return the padding algorithm.
     */
    const basic_padding& padder() const {
        return *_padder;
    }

    /**
     * @brief Return the block-cipher algorithm.
     */
    basic_algorithm& algorithm() const {
        return _algorithm;
    }

    /**
     * @brief Flag to indicate this is the final block.
     */
    bool final() const { return _final; }

    /**
     * @brief Return the next block for a cipher operation.
     * @param input The new data block (plain or cipher text).
     * @return The block to be processed.
     * @details
     * The @c input data block is the next block of data (plain or cipher text)
     * to be processed. The default implementation is to return the same data.
     */
    virtual data_t next(const data_t& input);

    /**
     * @brief Preprocess a block for encryption.
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
     * @brief Postprocess a block after encryption.
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
     * @brief Preprocess a block for decryption.
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
     * @brief Postprocess a block after decryption.
     * @param output The decrypted block.
     * @return The final decrypted block
     * @details
     * Perform any postlude action after encryption. This may alter the  mode's
     * insternal state and may alter the output block as well. Returns any
     * possible altered output block.
     * Default implementation returns @c output as-is.
     */
    virtual data_t postlude_decrypt(const data_t& output);

private:
    /** The keying material */
    material_t _keying;

    /** The algorithm */
    basic_algorithm& _algorithm;

    /** The padding algorithm */
    std::unique_ptr<basic_padding> _padder;

    /** Flag to indicate the processing of the final block. */
    bool _final = false;
};

/**
 * @brief Electronic Codebook (ECB) mode.
 * @details
 * Default padding type is PKCS#7.
 */
class MUDLIB_CRYPTO_API ecb: public basic_mode
{
public:
    /**
     * @brief Create an ECB mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     */
    ecb(const material_t& keying, basic_algorithm& algorithm);

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return "ECB"; }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t encryption_direction() const override {
        return direction_t::forward;
    }

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t decryption_direction() const override {
        return direction_t::inverse;
    }

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
    /** Flag to indicate that padding of the final block had been applied. */
    bool _padded = false;
};

/**
 * @brief Cipher Block Chaining (CBC) mode.
 * @details
 * Default padding type is PKCS#7.
 */
class MUDLIB_CRYPTO_API cbc: public basic_mode
{
public:
    /**
     * @brief Create a CBC mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     */
    cbc(const material_t& keying, basic_algorithm& algorithm);

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return "CBC"; }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t encryption_direction() const override {
        return direction_t::forward;
    }

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t decryption_direction() const override {
        return direction_t::inverse;
    }

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

    /** Flag to indicate that padding of the final block had been applied. */
    bool _padded = false;
};

/**
 * @brief Cipher Feedback (CFB-128) mode.
 * @details
 * By default, no padding is applied.
 */
class MUDLIB_CRYPTO_API cfb: public basic_mode
{
public:
    /**
     * @brief Create a CFB mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     */
    cfb(const material_t& keying, basic_algorithm& algorithm);

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return "CFB"; }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t encryption_direction() const override {
        return direction_t::forward;
    }

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t decryption_direction() const override {
        return direction_t::forward;
    }

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
    /**
     * @brief Create a CTR mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     */
    ctr(const material_t& keying, basic_algorithm& algorithm);

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return "CTR"; }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t encryption_direction() const override {
        return direction_t::forward;
    }

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t decryption_direction() const override {
        return direction_t::forward;
    }

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
 * @brief Galois Counter Mode (GCM) mode
 * @details
 * By default, no padding is applied.
 */
class MUDLIB_CRYPTO_API gcm: public basic_mode
{
public:
    /**
     * @brief Create a GCM mode.
     * @param keying The keying material.
     * @param algorithm The block-cipher algorithm.
     */
    gcm(const material_t& keying, basic_algorithm& algorithm);

    /**
     * @brief Return the mode specifications.
     */
    constexpr std::string_view specs() const override { return "GCM"; }

    /**
     * @brief Get the encryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t encryption_direction() const override {
        return direction_t::forward;
    }

    /**
     * @brief Get the decryption direction.
     * @details
     * Block cipher operation modes may use different different @em direction
     * of the undelrying block-cipher algorithm than the standard @c forward
     * for encryption and @c reverse for decryption.
     */
    direction_t decryption_direction() const override {
        return direction_t::forward;
    }

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

    /**
     * @brief Return the authentication tag.
     * @details
     * In case of authenticated encryption, return the authentication tag,
     * otherwise return a zero length buffer. The tag should only be valid after
     * the final block have been processed.
     */
    virtual data_t authentication_tag() const override;

private:
    /**
     * @brief Calculate the authentication tag.
     */
    void authenticate();

    /**
     * @brief Calculate the hash of a block of data.
     * @param h The hash key
     * @param a The additional data
     * @param c The cipher text
     * @return The calculated hash value
     * @details
     * The hashing function is typically invoked in two manners:
     *   - Initial as @f$ X_0 = ghash(H, \{\}, IV) @f$
     *   _ For each cipher block as @f X_m = ghash(H, X_{m-1}, C_m) @f$
     * The hash function is calculated as the multiplication in GF(2^128) of
     * the hash-key and the XOR of the previous hash and cipher block.
     */
    data_t ghash(const data_t& h, const data_t& a, const data_t& c);

    /**
     * @brief Multiplication in GF(2^128).
     * @param x The left operand of the multiplication
     * @param y The right  operand of the multiplication (hash key).
     * @return The GF multiplication value of X·Y.
     * @details
     * The multiplication in the Galois Field GF(2^128) with
     *     x^128 + x^7 + x^ 2 + x + 1
     * as irreducible polynomial.
     */
    data_t mult(const data_t& x, const data_t& y);

    /** The hash-key */
    data_t _H;

    /** The counter */
    counter_t _Y;

    /** The previously calculated hash value */
    data_t _X;

    /** The Encrypted Y0 used for the authentication tag */
    data_t _E_Y0;

    /** The hash index */
    ssize_t _X_i = -1;

    /** The counter index */
    ssize_t _Y_i = -1;

    /** The text block */
    data_t _text;

    /** The total size of cipher text in bytes */
    size_t _len_C = 0;

    /** The authentication tag */
    data_t _tag;
};

/**
 * @brief Stream buffer to apply block-cipher operations.
 */
class MUDLIB_CRYPTO_API block_cipher_streambuf: public std::streambuf
{
public:
    /**
     * @brief Create a streambuffer using a block-cipher.
     * @param chain The chained stream buffer.
     * @param algo The block cipher algorithm.
     * @param mode The block cipher mode.
     * @details
     * Using the @c chain stream-buffer for reading and writing encrypted data,
     * the @block_cipher_streambuf is a filtering steram buffer applying in-line
     * encryption adn decryption as specified by the @c algo block cipher
     * algorithm and @c mode of oepration.
     */
    block_cipher_streambuf(std::streambuf* chain,
            basic_algorithm& algo, basic_mode& mode);

    /**
     * @brief Destructors
     * Close and destruct this stream buffer, but not the chained stream buffer.
     */
    virtual ~block_cipher_streambuf();

    /**
     * @brief Explicitely close the stream buffer.
     * @details
     * When the stream buffer is closed it shall no longer be usable. For an
     * output stream, this will mark the end of the encrypted data stream and
     * any remaining data shall be padded (optional) and synchronised with the
     * chained stream buffer.
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

    /** The cipher algorithm */
    basic_algorithm& _algorithm;

    /** The blockcipher mode */
    basic_mode& _mode;

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
 * @brief An interface for a block cipher mode.
 * @details
 * The functions for a block cipher.
 */
class MUDLIB_CRYPTO_API basic_block_cipher
{
public:
    /**
     * @brief Destructor.
     */
    virtual ~basic_block_cipher() = default;

    /**
     * @brief Return the block cipher specifications.
     */
    virtual std::string specs() const = 0;

    /**
     * @brief Encrypt plain text into cipher text.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param key The algorithm key.
     */
    virtual void encrypt(const data_t& plain, data_t& cipher) = 0;

    /**
     * @brief Decrypt cipher text into plain text.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param key The algorithm key.
     */
    virtual void decrypt(const data_t& cipher, data_t& plain) = 0;

    /**
     * @brief Return the authentication tag.
     * @details
     * In case of authenticated encryption, return the authentication tag,
     * otherwise return a zero length buffer. The tag should only be valid after
     * the final block have been processed.
     */
    virtual data_t authentication_tag() const = 0;

    /**
     * @brief Create and return a filtering stream buffer for block cipher
     * cryptography.
     * @param chain The chained stream buffer.
     * @details
     * The stream buffer can be used in conjunction with any @c std::istream
     * to decrypt of @c std::ostream to encrypt data while performing the
     * streaming operators. The @c chain stream buffer is used to access the
     * encrypted data.
     *
     * If the stream buffer has already been created, then a pointer to that
     * stream-buffer will be returned.
     *
     * @note: on an @c std::ostream deleting the @c block_cipher will mark the
     * end of the encryption and the final data might be subject to padding.
     *
     * @note: on a @c std::istream, reading the end-of-file marks the end of the
     * decryption and the final data may be subject to unpadding.
     */
    virtual block_cipher_streambuf* sbuf(std::streambuf* chain) = 0;

    /**
     * @brief Return the associated stream buffer.
     * @return A pointer to the previously created stream-buffer or @c nullptr.
     */
    virtual block_cipher_streambuf* sbuf() const = 0;
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
 *
 * Encyrption and decryption of data using @c block_cipher can be accomplished
 * in two manners:
 *  - Using @c encrypt and @c decrypt on pre-determined plain and cipher texts
 *  - Using the @c sbuf stream buffer in association with an @c std::istream
 *    (for decryption) and @c std::ostream (for encryption) on plain and cipher
 *    texts as they are streamed.
 */
template<typename Algorithm, typename Mode>
class block_cipher : public basic_block_cipher
{
public:
    /** The type of block cipher algorithm */
    typedef Algorithm algorithm_t;

    /** The algorithm oeprating mode type */
    typedef Mode mode_t;

    /**
     * @brief Create a block cipher.
     * @param keying The keying material.
     */
    block_cipher(const material_t& keying);

    /**
     * @brief Destruct a block cipher.
     */
    virtual ~block_cipher();

    /**
     * @brief Return the block cipher specifications.
     */
    std::string specs() const override;

    /**
     * @brief Encrypt plain text into cipher text.
     * @param in The input block (ie plain text).
     * @param out The output block (ie cipher text).
     * @param key The algorithm key.
     */
    void encrypt(const data_t& plain, data_t& cipher) override;

    /**
     * @brief Decrypt cipher text into plain text.
     * @param in The input block (ie cipher text).
     * @param out The output block (ie plain text).
     * @param key The algorithm key.
     */
    void decrypt(const data_t& cipher, data_t& plain) override;

    /**
     * @brief Return the authentication tag.
     * @details
     * In case of authenticated encryption, return the authentication tag,
     * otherwise return a zero length buffer. The tag should only be valid after
     * the final block have been processed.
     */
    data_t authentication_tag() const override;

    /**
     * @brief Create and return a filtering stream buffer for block cipher
     * cryptography.
     * @param chain The chained stream buffer.
     * @details
     * The stream buffer can be used in conjunction with any @c std::istream
     * to decrypt of @c std::ostream to encrypt data while performing the
     * streaming operators. The @c chain stream buffer is used to access the
     * encrypted data.
     *
     * If the stream buffer has already been created, then a pointer to that
     * stream-buffer will be returned.
     *
     * @note: on an @c std::ostream deleting the @c block_cipher will mark the
     * end of the encryption and the final data might be subject to padding.
     *
     * @note: on a @c std::istream, reading the end-of-file marks the end of the
     * decryption and the final data may be subject to unpadding.
     */
    block_cipher_streambuf* sbuf(std::streambuf* chain) override;

    /**
     * @brief Return the associated stream buffer.
     * @return A pointer to the previously created stream-buffer or @c nullptr.
     */
    block_cipher_streambuf* sbuf() const override { return _sbuf; }

private:
    /** The cipher algorithm */
    algorithm_t _algorithm;

    /** The blockcipher mode */
    mode_t _mode;

    /** The stream buffer */
    block_cipher_streambuf* _sbuf = nullptr;

    /** Friend classes */
    friend class block_cipher_streambuf;
};

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::block_cipher(const material_t& keying)
    : _mode(keying, _algorithm)
{
}

template<typename Algorithm, typename Mode>
block_cipher<Algorithm, Mode>::~block_cipher()
{
    delete _sbuf;
}

template<typename Algorithm, typename Mode>
void
block_cipher<Algorithm, Mode>::encrypt(const data_t& plain, data_t& cipher)
{
    cipher.clear();
    auto* bptr = plain.data();
    auto* bend = plain.data() + plain.size();
    while (true) {
        /* Get the next block of data. */
        size_t sz = bend - bptr < _algorithm.block_size()
                  ? bend - bptr : _algorithm.block_size();
        data_t input = data_t(bptr, sz);

        /* Check if this is the final block */
        bool final = ((bend - bptr) <= _algorithm.block_size());

        /* Encrypt and append the output */
        data_t output;
        _mode.encrypt(input, output, final);
        cipher.append(output);
        if (output.size() == 0) {
            break;
        }

        /* Next */
        bptr += sz;
    }
}

template<typename Algorithm, typename Mode>
void
block_cipher<Algorithm, Mode>::decrypt(const data_t& cipher, data_t& plain)
{
    plain.clear();
    auto* bptr = cipher.data();
    auto* bend = cipher.data() + cipher.size();
    while (true) {
        /* Get the next block of data. */
        size_t sz = bend - bptr < _algorithm.block_size()
                  ? bend - bptr : _algorithm.block_size();
        data_t input = data_t(bptr, sz);

        /* Check if this is the final block */
        bool final = ((bend - bptr) <= _algorithm.block_size());

        /* Decrypt and append the output */
        data_t output;
        _mode.decrypt(input, output, final);
        plain.append(output);
        if (output.size() == 0) {
            break;
        }

        /* Next */
        bptr += sz;
    }
}

template<typename Algorithm, typename Mode>
std::string
block_cipher<Algorithm, Mode>::specs() const
{
    std::stringstream sstr;
    sstr << _algorithm.specs() << '-'
         << _algorithm.key_size() << '-'
         << _mode.specs();
    return sstr.str();
}

template<typename Algorithm, typename Mode>
data_t
block_cipher<Algorithm, Mode>::authentication_tag() const
{
    return _mode.authentication_tag();
}

template<typename Algorithm, typename Mode>
block_cipher_streambuf*
block_cipher<Algorithm, Mode>::sbuf(std::streambuf* chain)
{
    if (_sbuf == nullptr) {
        _sbuf = new block_cipher_streambuf(chain, _algorithm, _mode);
    }
    return _sbuf;
}

/**
 * @brief Factory for block-ciphers, idnexed by cipher specification.
 */
typedef ::mud::core::factory<std::string,
                             mud::crypto::basic_block_cipher,
                             mud::crypto::material_t>
        block_cipher_factory;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_BLOCK_CIPHER_H_ */
