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

#include "mud/core/endian.h"
#include "mud/core/log.h"
#include "mud/crypto/block_cipher.h"
#include "mud/crypto/exception.h"
#include <cstring>
#include <iostream>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

/**
 * Internal logging is disabled for seurity reasons and performance. Only enable
 * it for development purposes.
 */
#define BLOCK_CIPHER_LOGGING false
#if BLOCK_CIPHER_LOGGING
    #warning "CAUTION: Block-cipher logging is enabled"
#else
    #undef LOG
    #undef TRACE
    #undef DEBUG
    #undef INFO
    #undef WARN
    #undef ERROR
    #undef FATAL
    #undef TYPEINFO
    #define LOG(obj)
    #define TRACE(obj)  if (true) {} else std::cout
    #define DEBUG(obj)  if (true) {} else std::cout
    #define INFO(obj)   if (true) {} else std::cout
    #define WARN(obj)   if (true) {} else std::cout
    #define ERROR(obj)  if (true) {} else std::cout
    #define FATAL(obj)  if (true) {} else std::cout
    #define TYPEINFO(t) ""
#endif

BEGIN_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Generic mode actions
 * ========================================================================== */

void
basic_mode::encrypt(const data_t& plain, data_t& cipher, bool final)
{
    _final = final;

    /* Ensure the cipher block is cleared. */
    cipher.clear();

    /* Retrieve the input block and preprocess it. If the processed result is
     * empty, then there is nothing to be done. */
    auto input = next(plain);
    input = prelude_encrypt(input);
    if (input.size() == 0) {
        return;
    }

    /* Perform the encryption */
    data_t output;
    if (encryption_direction() == basic_mode::direction_t::forward) {
        _algorithm.forward(input, output, _keying.key());
    }
    else {
        _algorithm.inverse(input, output, _keying.key());
    }

    /* Postprocess the result */
    output = postlude_encrypt(output);
    cipher.append(output);
}

void
basic_mode::decrypt(const data_t& cipher, data_t& plain, bool final)
{
    _final = final;

    /* Ensure the plain block is cleared */
    plain.clear();

    /* Retrieve the input block and preprocess it. If the processed result is
     * empty, then there is nothing to be done. */
    auto input = next(cipher);
    input = prelude_decrypt(input);
    if (input.size() == 0) {
        return;
    }

    /* Perform the decryption */
    data_t output;
    if (decryption_direction() == basic_mode::direction_t::forward) {
        _algorithm.forward(input, output, _keying.key());
    }
    else {
        _algorithm.inverse(input, output, _keying.key());
    }

    /* Postprocess the result */
    output = postlude_decrypt(output);
    plain.append(output);
}

data_t
basic_mode::next(const data_t& input)
{
    return input;
}

data_t
basic_mode::prelude_encrypt(const data_t& input)
{
    return input;
}

data_t
basic_mode::postlude_encrypt(const data_t& output)
{
    return output;
}

data_t
basic_mode::prelude_decrypt(const data_t& input)
{
    return input;
}

data_t
basic_mode::postlude_decrypt(const data_t& output)
{
    return output;
}

/* ==========================================================================
 * ECB mode
 *
 * Encryption:
 *   For each block in the PLAINTEXT, apply the cipher to create the block in
 *   the CIPHERTEXT
 *
 *         PLAINTEXT-n
 *             ↓
 *        input block
 *          CIPH(K)
 *        output block
 *             ↓
 *         CIPHERTEXT-n
 *
 * Decryption:
 *   For each block in the CIPHERTEXT, apply the inverse cipher to create the
 *   block in the CIPHERTEXT
 *
 *         CIPHERTEXT
 *             ↓
 *        input block
 *         CIPH-1(K)
 *        output block
 *             ↓
 *         PLAINTEXT
 *
 * ========================================================================== */

ecb::ecb(const material_t& keying, basic_algorithm& algorithm)
    : basic_mode(keying, algorithm)
{
    LOG(log);
    TRACE(log) << "ECB mode" << std::endl
               << "         K| " << keying.key() << std::endl;
}

data_t
ecb::prelude_encrypt(const data_t& input)
{
    data_t plain = input;
    if (final() && (input.size() < block_size())) {
        if (_padded) {
            return data_t(0);
        }
        plain = padder().pad(plain);
        _padded = true;
    }

    if (plain.size()) {
        LOG(log);
        TRACE(log) << "         P| " << plain << std::endl;
    }

    return plain;
}

data_t
ecb::postlude_encrypt(const data_t& output)
{
    if (output.size()) {
        LOG(log);
        TRACE(log) << "         C| " << output << std::endl;
    }

    return output;
}

data_t
ecb::prelude_decrypt(const data_t& input)
{
    if (input.size()) {
        LOG(log);
        TRACE(log) << "         C| " << input << std::endl;
    }
    return input;
}

data_t
ecb::postlude_decrypt(const data_t& output)
{
    data_t plain = output;
    if (final()) {
        plain = padder().unpad(output);
    }

    if (plain.size()) {
        LOG(log);
        TRACE(log) << "         P| " << plain << std::endl;
    }

    return plain;
}

/* ==========================================================================
 * CBC mode
 *
 * Encryption:
 *   For each block in the PLAINTEXT, XOR with the previous block output (or
 *   the inital vector for the first block) and apply the cipher to create the
 *   block in the CIPHERTEXT
 *
 *         PLAINTEXT-1      PLAINTEXT-2      PLAINTEXT-n
 *             ↓                ↓                ↓
 *        IV → ⨁        ┌─────→ ⨁        ┌─────→ ⨁
 *             ↓        │       ↓        │       ↓
 *        input block   │  input block   │  input block
 *          CIPH(K)     │    CIPH(K)     │    CIPH(K)
 *        output block  │  output block  │  output block
 *             ↓ ───────┘       ↓ ───────┘       ↓
 *         CIPHERTEXT-1     CIPHERTEXT-2     CIPHERTEXT-n
 *
 * Decryption:
 *   For each block in the CIPHERTEXT, apply the inverse cipher to create the
 *   output block which is XOR with the previous cipher block (or the initial
 *   vector for the first block) to create the block in the PLAINTEXT
 *
 *         CIPHERTEXT-1     CIPHERTEXT-2     CIPHERTEXT-n
 *             ↓ ───────┐       ↓ ───────┐       ↓
 *        input block   │  input block   │  input block
 *         CIPH-1(K)    │   CIPH-1(K)    │   CIPH-1(K)
 *        output block  │  output block  │  output block
 *             ↓        │       ↓        │       ↓
 *        IV → ⨁        └─────→ ⨁        └─────→ ⨁
 *             ↓                ↓                ↓
 *         PLAINTEXT-1      PLAINTEXT-2      PLAINTEXT-n
 *
 * ========================================================================== */

cbc::cbc(const material_t& keying, basic_algorithm& algorithm)
    : basic_mode(keying, algorithm)
    , _iv(keying.iv())
    , _previous(_iv)
{
    if (_iv.size() != block_size()) {
        throw size_error("CBC initialization vector not equal to block-size");
    }
    LOG(log);
    TRACE(log) << "CBC mode" << std::endl
               << "         K| " << keying.key() << std::endl
               << "        IV| " << keying.iv() << std::endl;
}

data_t
cbc::prelude_encrypt(const data_t& input)
{
    data_t plain = input;
    if (final() && (input.size() < block_size())) {
        if (_padded) {
            return data_t(0);
        }
        plain = padder().pad(plain);
        _padded = true;
    }

    if (plain.size()) {
        LOG(log);
        TRACE(log) << "         P| " << plain << std::endl;
    }

    return plain ^ _previous;
}

data_t
cbc::postlude_encrypt(const data_t& output)
{
    _previous = output;

    if (output.size()) {
        LOG(log);
        TRACE(log) << "         C| " << output << std::endl;
    }

    return output;
}

data_t
cbc::prelude_decrypt(const data_t& input)
{
    _tmp = input;

    if (input.size()) {
        LOG(log);
        TRACE(log) << "         C| " << input << std::endl;
    }

    return input;
}

data_t
cbc::postlude_decrypt(const data_t& output)
{
    auto plain = output ^ _previous;
    if (final()) {
        plain = padder().unpad(plain);
    }

    if (plain.size()) {
        LOG(log);
        TRACE(log) << "         P| " << plain << std::endl;
    }

    _previous = _tmp;
    return plain;
}

/* ==========================================================================
 * CFB mode (only CFB-128 is supported)
 *
 * Encryption:
 *    For each block in PLAINTEXT, XOR with the cipher'ed result of the
 *    previous CIPHERTEXT block (or the initial vector for the first block) to
 *    create the CIPHERTEXT block.
 *
 *            IV        ┌───────────┐        ┌───────────┐
 *             ↓        │           ↓        │           ↓
 *        input block   │      input block   │      input block
 *          CIPH(K)     │        CIPH(K)     │        CIPH(K)
 *        output block  │      output block  │      output block
 *             ↓        │           ↓        │           ↓      
 *   PLAIN-1 → ⨁        │ PLAIN-2 → ⨁        │ PLAIN-n → ⨁
 *             ↓ ───────┘           ↓ ───────┘           ↓
 *          CIPHER-1             CIPHER-2             CIPHER-n
 *
 * Decryption:
 *    For each block in CIPHERTEXT, XOR with the cipher'ed result of the
 *    previous CIPHERTEXT block (or the initial vector for the first block) to
 *    create the PLAINTEXT block.
 *
 *            IV        ┌───────────┐        ┌───────────┐
 *             ↓        │           ↓        │           ↓
 *        input block   │      input block   │      input block
 *          CIPH(K)     │        CIPH(K)     │        CIPH(K)
 *        output block  │      output block  │      output block
 *             ↓        │           ↓        │           ↓      
 *             ⨁  ← CIPHER-1        ⨁ ← CIPHER-2         ⨁ ← CIPHER-n
 *             ↓                    ↓                    ↓
 *          PLAIN-1              PLAIN-2-2             PLAIN-n-n
 *
 * Note: both encryption and decription use the CIPHER (not the inverse). The
 * exact same input blocks are used in each cipher step.
 *
 * CFB mode does not need padding.
 * ========================================================================== */

cfb::cfb(const material_t& keying, basic_algorithm& algorithm)
    : basic_mode(keying, algorithm)
    , _iv(keying.iv())
    , _previous(_iv)
{
    if (_iv.size() != block_size()) {
        throw size_error("CFB initialization vector not equal to block-size");
    }
    LOG(log);
    TRACE(log) << "CFB mode" << std::endl
               << "         K| " << keying.key() << std::endl
               << "        IV| " << keying.iv() << std::endl;
}

data_t
cfb::prelude_encrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;

    LOG(log);
    TRACE(log) << "         P| " << _text << std::endl;

    return _previous;
}

data_t
cfb::postlude_encrypt(const data_t& output)
{
    _previous = _text ^ output;

    LOG(log);
    TRACE(log) << "         C| " << _previous << std::endl;

    return _previous;
}

data_t
cfb::prelude_decrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;

    LOG(log);
    TRACE(log) << "         C| " << _text << std::endl;

    return _previous;
}

data_t
cfb::postlude_decrypt(const data_t& output)
{
    _previous = _text;
    data_t plain = _text ^ output;

    LOG(log);
    TRACE(log) << "         P| " << plain << std::endl;

    return plain;
}

/* ==========================================================================
 * CTR mode
 *
 * Encryption:
 *    For each block in PLAINTEXT, XOR with the cipher'ed result of the
 *    counter to create the CIPHERTEXT block.
 *
 *          Counter  ─────────→ Counter+1  ────────→ Counter+n
 *             ↓                    ↓                    ↓
 *        input block          input block          input block
 *          CIPH(K)              CIPH(K)              CIPH(K)
 *        output block         output block         output block
 *             ↓                    ↓                    ↓
 *   PLAIN-1 → ⨁          PLAIN-2 → ⨁          PLAIN-n → ⨁
 *             ↓                    ↓                    ↓
 *          CIPHER-1             CIPHER-2             CIPHER-n
 *
 * Decryption:
 *    For each block in CIPHERTEXT, XOR with the cipher'ed result of the
 *    counter to create the PLAINTEXT block.
 *
 *          Counter  ─────────→ Counter+1  ────────→ Counter+n
 *             ↓                    ↓                    ↓
 *        input block          input block          input block
 *          CIPH(K)              CIPH(K)              CIPH(K)
 *        output block         output block         output block
 *             ↓                    ↓                    ↓
 *  CIPHER-1 → ⨁         CIPHER-2 → ⨁         CIPHER-n → ⨁
 *             ↓                    ↓                    ↓
 *          PLAIN-1              PLAIN-2-2             PLAIN-n-n
 *
 * Note: both encryption and decription use the CIPHER (not the inverse). The
 * exact same input blocks (counters) are used in each cipher step.
 *
 * CTR mode does not need padding.
 * ========================================================================== */

ctr::ctr(const material_t& keying, basic_algorithm& algorithm)
    : basic_mode(keying, algorithm)
    , _counter(keying.counter())
{
    if (_counter.size() != block_size()) {
        throw size_error("CTR counter size not equal to block-size");
    }
    LOG(log);
    TRACE(log) << "CTR mode" << std::endl
               << "         K| " << keying.key() << std::endl;
}

data_t
ctr::prelude_encrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;

    LOG(log);
    TRACE(log) << "       CTR| " << _counter << std::endl
               << "         P| " << _text << std::endl;

    return _counter;
}

data_t
ctr::postlude_encrypt(const data_t& output)
{
    ++_counter;
    data_t cipher = _text ^ output;

    LOG(log);
    TRACE(log) << "         C| " << cipher << std::endl;

    return cipher;
}

data_t
ctr::prelude_decrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;

    LOG(log);
    TRACE(log) << "       CTR| " << _counter << std::endl
               << "         C| " << _text << std::endl;

    return _counter;
}

data_t
ctr::postlude_decrypt(const data_t& output)
{
    ++_counter;
    data_t plain = _text ^ output;

    LOG(log);
    TRACE(log) << "         P| " << plain << std::endl;

    return plain;
}

/* ==========================================================================
 * GCM mode
 *
 * Encryption:
 *    For each block in PLAINTEXT, XOR with the cipher'ed result of the
 *    counter to create the CIPHERTEXT block. The initial counter is created
 *    from an initialisation vector and cipher'ed. The result is XOR'd at the
 *    final stage to create the authentication tag. After each cipher'ed plain
 *    text, the cipher text is XOR'd with the result of the previous round
 *    GF multiplication. For the initial round, the authentication input data
 *    is used instead. The authentication tag is finalised with an XOR'd of the
 *    concatenated lengths of the authentication data and cipher text in bits,
 *    followed by a final GF multiplication.
 *
 *      IV
 *       ↓
 *    Counter  ────────→ Counter+1  ────────→ Counter+2  ────────→ Counter+n
 *       ↓                   ↓                    ↓                    ↓
 *  input block         input block          input block          input block
 *    CIPH(K)             CIPH(K)              CIPH(K)              CIPH(K)
 *  output block        output block         output block         output block
 *       │                   ↓                    ↓                    ↓
 *       │         PLAIN-1 ← ⨁          PLAIN-2 ← ⨁          PLAIN-n ← ⨁
 *       │                   ↑                    ↑                    ↑
 *       │            CIPHER-1             CIPHER-2             CIPHER-n
 *       │                   ↓                    ↓                    ↓
 *       │        ┌────────→ ⨁          ┌───────→ ⨁          ┌───────→ ⨁
 *       │        │          ↓          │         ↓          │         ↓
 *       │     MULT(H)    MULT(H) ──────┘      MULT(H) ──────┘      MULT(H)
 *       │        ↑                                                    ↓
 *       │       AAD                                 LEN(A)||LEN(C) ─→ ⨁
 *       │                                                             ↓
 *       │                                                          MULT(H)
 *       │                                                             ↓
 *       └───────────────────────────────────────────────────────────→ ⨁
 *                                                                     ↓
 *                                                                    TAG
 *
 * Decryption:
 *    Decryption follows the same path as encryption.
 *
 *      IV
 *       ↓
 *    Counter  ────────→ Counter+1  ────────→ Counter+2  ────────→ Counter+n
 *       ↓                   ↓                    ↓                    ↓
 *  input block         input block          input block          input block
 *    CIPH(K)             CIPH(K)              CIPH(K)              CIPH(K)
 *  output block        output block         output block         output block
 *       │                   ↓                    ↓                    ↓
 *       │        CIPHER-1 → ⨁         CIPHER-2 → ⨁         CIPHER-n → ⨁
 *       │                   ↓                    ↓                    ↓
 *       │             PLAIN-1              PLAIN-2              PLAIN-n
 *       │                   ↓                    ↓                    ↓
 *       │        ┌────────→ ⨁          ┌───────→ ⨁          ┌───────→ ⨁
 *       │        │          ↓          │         ↓          │         ↓
 *       │     MULT(H)    MULT(H) ──────┘      MULT(H) ──────┘      MULT(H)
 *       │        ↑                                                    ↓
 *       │       AAD                                 LEN(A)||LEN(C) ─→ ⨁
 *       │                                                             ↓
 *       │                                                          MULT(H)
 *       │                                                             ↓
 *       └───────────────────────────────────────────────────────────→ ⨁
 *                                                                     ↓
 *                                                                    TAG
 *
 * Note: both encryption and decription use the CIPHER (not the inverse). The
 * exact same input blocks (counters) are used in each cipher step.
 *
 * GCM mode does not need padding.
 * ========================================================================== */

gcm::gcm(const material_t& keying, basic_algorithm& algorithm)
    : basic_mode(keying, algorithm)
    , _X(data_t(block_size()))
{
    if (block_size()*8 != 128) {
        throw size_error("GCM mode only implemented for 128 bit blocks");
    }
    if (keying.iv().size() == 0) {
        throw size_error("GCM requires an initialization vector");
    }

    LOG(log);
    TRACE(log) << "GCM mode" << std::endl
               << "         K| " << keying.key() << std::endl
               << "         A| " << keying.aad() << std::endl
               << "        IV| " << keying.iv() << std::endl;

    /*
     * Hash key is a ciphered zero block.
     */
    data_t zero(16);
    algorithm.forward(zero, _H, keying.key());
    TRACE(log) << "         H| " << _H << std::endl;

    /*
     * Counter value derived from the initialization vector:
     *    - Length of IV 96-bits: Append 00000001
     *    - Else: GHASH(H, {}, IV)
     */
    if (keying.iv().size() == 12) {
        _Y = data_t(16);
        memcpy(_Y.data(), keying.iv().data(), keying.iv().size());
        ++_Y, ++_Y_i;
    }
    else {
        _Y = ghash(_H, data_t(0), keying.iv());
        TRACE(log) << "        N1| " << _Y << std::endl;
        data_t len(16);
        *(uint64_t*)(len.data()+8) = endian::convert(
                endian::native(), endian_t::big,
                (uint64_t)keying.iv().size()*8);
        TRACE(log) << "LEN({},IV)| " << len << std::endl;
        _Y = mult(_Y ^ len, _H), ++_Y_i;
    }


    /* The encrypted Y0 is used in the final tag construction */
    algorithm.forward(_Y, _E_Y0, keying.key());
    TRACE(log) << "        Y" << _Y_i << "| " << _Y << std::endl;
    TRACE(log) << "   E(K,Y" << _Y_i << ")| " << _E_Y0 << std::endl;

    /* The initial hash index is 0 */
    _X_i = 0;
    TRACE(log) << "        X" << _X_i << "| " << _X << std::endl;

    /*
     * The initial hash is over the AAD only and no cipher-text.
     */
    _X = ghash(_H, keying.aad(), data_t(0));
}

data_t
gcm::prelude_encrypt(const data_t& input)
{
    if (input.size() == 0) {
        if (final() && _len_C == 0) {
            /* Nothing to encrypt at all. Calculate the authentication tag. */
            authenticate();
        }
        return input;
    }
    _text = input;
    ++_Y, ++_Y_i;

    LOG(log);
    TRACE(log) << "        Y" << _Y_i << "| " << _Y << std::endl;

    return _Y;
}

data_t
gcm::postlude_encrypt(const data_t& output)
{
    /* The cipher output of the final block can be less than the block-size,
     * but the authenticated tag requires a zero-padded cipher input data. */
    data_t cipher = _text ^ output;
    data_t padded = cipher;
    padded.pad_end(16);
    _X = mult(padded ^ _X, _H), ++_X_i;
    _len_C += cipher.size();

    LOG(log);
    TRACE(log) << "         P| " << _text << std::endl
               << "   E(K,Y" << _Y_i << ")| " << output << std::endl
               << "         C| " << cipher << std::endl
               << "        X" << _X_i << "| " << _X << std::endl;

    /* If this is the final block, calculate the authentication tag */
    if (final()) {
        authenticate();
    }

    return cipher;
}

data_t
gcm::prelude_decrypt(const data_t& input)
{
    if (input.size() == 0) {
        if (final() && _len_C == 0) {
            /* Nothing to encrypt at all. Calculate the authentication tag. */
            authenticate();
        }
        return input;
    }
    _text = input;
    ++_Y, ++_Y_i;

    LOG(log);
    TRACE(log) << "        Y" << _Y_i << "| " << _Y << std::endl;

    return _Y;
}

data_t
gcm::postlude_decrypt(const data_t& output)
{
    /* The cipher output of the final block can be less than the block-size,
     * but the authenticated tag requires a zero-padded cipher input data. */
    data_t plain = _text ^ output;
    data_t padded = _text;
    padded.pad_end(16);
    _X = mult(padded ^ _X, _H), ++_X_i;
    _len_C += _text.size();

    LOG(log);
    TRACE(log) << "         C| " << _text << std::endl
               << "   E(K,Y" << _Y_i << ")| " << output << std::endl
               << "         P| " << plain << std::endl
               << "        X" << _X_i << "| " << _X << std::endl;

    /* If this is the final block, calculate the authentication tag */
    if (final()) {
        authenticate();
    }

    return plain;
}

data_t
gcm::authentication_tag() const
{
    if (!final()) {
        throw crypto_error("authentication tag on unfinalized cipher");
    }
    return _tag;
}

void
gcm::authenticate()
{
    data_t len(16);
    *(uint64_t*)(len.data()+0) = endian::convert(
            endian::native(), endian_t::big,
            (uint64_t)keying().aad().size()*8);
    *(uint64_t*)(len.data()+8) = endian::convert(
            endian::native(), endian_t::big,
            (uint64_t)_len_C*8);
    _tag = mult(_X ^ len, _H) ^ _E_Y0;

    LOG(log);
    TRACE(log) << "  LEN(A,C)| " << len << std::endl
               << "         T| " << _tag << std::endl;
}

data_t
gcm::ghash(const data_t& h, const data_t& a, const data_t& c)
{
    LOG(log);
    bool iv_hash = (_X_i < 0);

    /* Hash over all blocks of A, final partial block may need zero-padding. */
    data_t x = _X;
    size_t m;
    for (m = 0; m < a.size() / 16; ++m) {
        data_t b(a.data() + m * 16, 16);
        x = mult(x ^ b, _H);
        if (!iv_hash) {
            ++_X_i;
            TRACE(log) << "        X" << _X_i << "| " << x << std::endl;
        }
    }
    size_t v = a.size() % 16;
    if (v != 0) {
        data_t b(a.data() + m * 16, v);
        b.append(data_t(16 - v));
        x = mult(x ^ b, _H);
        if (!iv_hash) {
            ++_X_i;
            TRACE(log) << "        X" << _X_i << "| " << x << std::endl;
        }
    }

    /* Hash over all blocks of C, final partial block may need zero-padding. */
    size_t n;
    for (n = 0; n < c.size() / 16; ++n) {
        data_t b(c.data() + n * 16, 16);
        x = mult(x ^ b, _H);
        if (!iv_hash) {
            ++_X_i;
            TRACE(log) << "        X" << _X_i << "| " << x << std::endl;
        }
    }
    size_t u = c.size() % 16;
    if (u != 0) {
        data_t b(c.data() + n * 16, u);
        b.append(data_t(16 - u));
        x = mult(x ^ b, _H);
        if (!iv_hash) {
            ++_X_i;
            TRACE(log) << "        X" << _X_i << "| " << x << std::endl;
        }
    }
    return x;
}

data_t
gcm::mult(const data_t& x, const data_t& y)
{
    /* The default implementation from the AES GCM specification without any
     * optimization. Possible optimization is the use of calculated H-tables. */

    /* Define the reduced polynomial */
    data_t r(16);
    r[0]= 0xe1;

    data_t z(16);
    data_t v = x;
    for (int i = 0; i < 16*8; ++i) {
        /* If the bit at the i-th position is set add (XOR) v. */
        if (y[i/8] & (0x01 << (7 - (i % 8)))) {
            z ^= v;
        }

        /* Right-shift v and check for shifted out carry */
        bool carry = ((v[15] & 0x01) == 0x01);
        v >>= 1;

        /* If there was a carry, add (XOR) the reduced polynomial */
        if (carry) {
            v ^= r;
        }
    }
    return z;
}

/* ==========================================================================
 * The block cipher stream buffer.
 * ========================================================================== */

block_cipher_streambuf::block_cipher_streambuf(
        std::streambuf* chain,
        basic_algorithm& algo, basic_mode& mode)
    : _chain(chain)
    , _algorithm(algo)
    , _mode(mode)
    , _bufsize(mode.block_size())
    , _putbacksize(4)
    , _eof(false)
{
    /* Allocate the data structure */
    _buffer = new char[_bufsize + _putbacksize];

    /* Set the get area pointers for reading */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize);

    /* Set the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
}

block_cipher_streambuf::~block_cipher_streambuf()
{
    close();
    delete[] _buffer;
}

void
block_cipher_streambuf::close()
{
    _eof = true;
    sync();
}

int
block_cipher_streambuf::underflow()
{
    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Read new block using the buffer itself as temporary storage */
    int nread = 0;
    while (nread < _mode.block_size()) {
        int nr = _chain->sgetn(_buffer + _putbacksize + nread,
                               _mode.block_size() - nread);
        if (nr <= 0) {
            break;
        }
        nread += nr;
    }
    if (nread <= 0) {
        return traits_type::eof();
    }

    /* Check if there is more deta */
    _eof = _chain->sgetc() == traits_type::eof();

    /* Construct the input block */
    data_t cipher = data_t((const uint8_t*)_buffer + _putbacksize, nread);

    /* Decrypt the block */
    data_t plain;
    _mode.decrypt(cipher, plain, _eof);

    /* Move the decrypted block to the stream buffer */
    memcpy(_buffer + _putbacksize, plain.data(), plain.size());

    /* Reset the get buffer pointers with the number of characters read. */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize + plain.size());

    /* Check if there is no more data (a full block of padding was unpad) */
    if (gptr() == egptr()) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*gptr());
}

int
block_cipher_streambuf::overflow(int c)
{
    /* Insert the character into the buffer */
    if (c != traits_type::eof()) {
        *pptr() = c;
        pbump(1);
    }

    /* Encrypt the block */
    if (sync() != 0) {
        return traits_type::eof();
    }
    return c;
}

int
block_cipher_streambuf::sync()
{
    /* Construct the output block */
    data_t plain = data_t((const uint8_t*)pbase(), pptr() - pbase());

    /* Encrypt it */
    data_t cipher;
    _mode.encrypt(plain, cipher, _eof);

    /* Write the block */
    int nwritten = 0;
    while (nwritten < cipher.size()) {
        int nr = _chain->sputn((const char*)cipher.data() + nwritten,
                               cipher.size() - nwritten);
        if (nr <= 0) {
            break;
        }
        nwritten += nr;
    }
    if (nwritten <= 0) {
        return -1;
    }

    /* Reset the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
    return 0;
}

END_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Block cipher Factory
 * ========================================================================== */

template<>
mud::crypto::block_cipher_factory&
mud::crypto::block_cipher_factory::instance()
{
    static mud::crypto::block_cipher_factory _instance;
    return _instance;
}

/* vi: set ai ts=4 expandtab: */
