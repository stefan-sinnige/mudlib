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

#include "mud/crypto/block_cipher.h"
#include "mud/crypto/exception.h"

#include <iostream>

BEGIN_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Generic mode actions
 * ========================================================================== */

data_t
basic_mode::next(const uint8_t* bptr, const uint8_t* bend)
{
    if (bend > bptr) {
        if (bend - bptr >= block_size()) {
            return data_t(bptr, block_size());
        }
        return data_t(bptr, bend - bptr);
    }
    return data_t(0);
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

void
basic_mode::padding(basic_padding::type_t type)
{
    _padder = padding_factory::instance().create(type, _block_size);
}

basic_padding::type_t
basic_mode::padding() const
{
    return _padder->type();
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

ecb::ecb(size_t block_size, const key_t& key)
    : basic_mode(block_size, key)
    , _final(false)
    , _eos(false)
{
    padding(basic_padding::type_t::pkcs7);
}

data_t
ecb::next(const uint8_t* bptr, const uint8_t* bend)
{
    if (bptr > bend) {
        _eos = true;
        return data_t(0);
    }
    if (bend - bptr <= block_size()) {
        _final = true;
    }
    return basic_mode::next(bptr, bend);
}

data_t
ecb::prelude_encrypt(const data_t& input)
{
    if (_eos) {
        return data_t(0);
    }
    if (input.size() < block_size()) {
        if (_final) {
            return padder().pad(input);
        }
    }
    return input;
}

data_t
ecb::postlude_encrypt(const data_t& output)
{
    return output;
}

data_t
ecb::prelude_decrypt(const data_t& input)
{
    return input;
}

data_t
ecb::postlude_decrypt(const data_t& output)
{
    if (_final) {
        return padder().unpad(output);
    }
    return output;
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

cbc::cbc(size_t block_size, const key_t& key, const iv_t& iv)
    : basic_mode(block_size, key)
    , _iv(iv)
    , _previous(_iv)
    , _final(false)
    , _eos(false)
{
    padding(basic_padding::type_t::pkcs7);
}

data_t
cbc::next(const uint8_t* bptr, const uint8_t* bend)
{
    if (_iv.size() != block_size()) {
        throw size_error("CBC initial vector not equal to block-size");
    }
    if (bptr > bend) {
        _eos = true;
        return data_t(0);
    }
    if (bend - bptr <= block_size()) {
        _final = true;
    }
    return basic_mode::next(bptr, bend);
}

data_t
cbc::prelude_encrypt(const data_t& input)
{
    if (_eos) {
        return data_t(0);
    }
    if (input.size() < block_size()) {
        if (_final) {
            return data_t(padder().pad(input)) ^ _previous;
        }
    }
    return input ^ _previous;
}

data_t
cbc::postlude_encrypt(const data_t& output)
{
    _previous = output;
    return output;
}

data_t
cbc::prelude_decrypt(const data_t& input)
{
    _tmp = input;
    return input;
}

data_t
cbc::postlude_decrypt(const data_t& output)
{
    auto result = output ^ _previous;
    if (_final) {
        return padder().unpad(result);
    }
    _previous = _tmp;
    return result;
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

cfb::cfb(size_t block_size, const key_t& key, const iv_t& iv)
    : basic_mode(block_size, key)
    , _iv(iv)
    , _previous(_iv)
{
    padding(basic_padding::type_t::none);
}

data_t
cfb::next(const uint8_t* bptr, const uint8_t* bend)
{
    if (_iv.size() != block_size()) {
        throw size_error("CFB initial vector not equal to block-size");
    }
    return basic_mode::next(bptr, bend);
}

data_t
cfb::prelude_encrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;
    return _previous;
}

data_t
cfb::postlude_encrypt(const data_t& output)
{
    _previous = _text ^ output;
    return _previous;
}

data_t
cfb::prelude_decrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;
    return _previous;
}

data_t
cfb::postlude_decrypt(const data_t& output)
{
    _previous = _text;
    return _text ^ output;
}

/* ==========================================================================
 * CTR mode
 *
 * Encryption:
 *    For each block in PLAINTEXT, XOR with the cipher'ed result of the
 *    counter to create the CIPHERTEXT block.
 *
 *          Counter             Counter+1            Counter+n
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
 *          Counter             Counter+1            Counter+n
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

ctr::ctr(size_t block_size, const key_t& key, const counter_t& counter)
    : basic_mode(block_size, key)
    , _counter(counter)
{
    padding(basic_padding::type_t::none);
}

data_t
ctr::prelude_encrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;
    return _counter;
}

data_t
ctr::postlude_encrypt(const data_t& output)
{
    ++_counter;
    return _text ^ output;
}

data_t
ctr::prelude_decrypt(const data_t& input)
{
    if (input.size() == 0) {
        return input;
    }
    _text = input;
    return _counter;
}

data_t
ctr::postlude_decrypt(const data_t& output)
{
    ++_counter;
    return _text ^ output;
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
