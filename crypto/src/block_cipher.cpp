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
#include <cstring>

#include <iostream>

BEGIN_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Generic mode actions
 * ========================================================================== */

data_t
basic_mode::next(const uint8_t* bptr, const uint8_t* bend, bool eof)
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

ecb::ecb(size_t block_size, const material_t& keying)
    : basic_mode(block_size, keying)
    , _pad(0)
{
}

data_t
ecb::next(const uint8_t* bptr, const uint8_t* bend, bool eof)
{
    if (eof) {
        ++_pad;
    }
    return basic_mode::next(bptr, bend, eof);
}

data_t
ecb::prelude_encrypt(const data_t& input)
{
    if (input.size() < block_size()) {
        if (_pad == 1) {
            return padder().pad(input);
        }
        else {
            return data_t(0);
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
    if (_pad) {
        data_t tmp = padder().unpad(output);
        return tmp;
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

cbc::cbc(size_t block_size, const material_t& keying)
    : basic_mode(block_size, keying)
    , _iv(keying.iv())
    , _previous(_iv)
    , _pad(0)
{
    if (_iv.size() != block_size) {
        throw size_error("CBC initial vector not equal to block-size");
    }
}

data_t
cbc::next(const uint8_t* bptr, const uint8_t* bend, bool eof)
{
    if (eof) {
        ++_pad;
    }
    return basic_mode::next(bptr, bend, eof);
}

data_t
cbc::prelude_encrypt(const data_t& input)
{
    if (input.size() < block_size()) {
        if (_pad == 1) {
            return data_t(padder().pad(input)) ^ _previous;
        }
        else {
            return data_t(0);
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
    if (_pad) {
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

cfb::cfb(size_t block_size, const material_t& keying)
    : basic_mode(block_size, keying)
    , _iv(keying.iv())
    , _previous(_iv)
{
    if (_iv.size() != block_size) {
        throw size_error("CFB initial vector not equal to block-size");
    }
}

data_t
cfb::next(const uint8_t* bptr, const uint8_t* bend, bool eof)
{
    return basic_mode::next(bptr, bend, eof);
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

ctr::ctr(size_t block_size, const material_t& keying)
    : basic_mode(block_size, keying)
    , _counter(keying.counter())
{
    if (_counter.size() != block_size) {
        throw size_error("CTR counter size not equal to block-size");
    }
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

    /* Decrypt the block */
    data_t output;
    auto input = _mode.next((const uint8_t*)_buffer + _putbacksize,
                            (const uint8_t*)_buffer + _putbacksize + nread,
                            _eof);
    input = _mode.prelude_decrypt(input);
    if (input.size() == 0) {
        return traits_type::eof();
    }
    if (_mode.decryption_direction() == basic_mode::direction_t::forward) {
        _algorithm.encrypt(input, output, _mode.keying().key());
    }
    else {
        _algorithm.decrypt(input, output, _mode.keying().key());
    }
    output = _mode.postlude_decrypt(output);

    /* Move the decrypted block to the stream buffer */
    memcpy(_buffer + _putbacksize, output.data(), output.size());

    /* Reset the get buffer pointers with the number of characters read. */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize + output.size());

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
    /* Encrypt the block */
    data_t output;
    auto input = _mode.next((const uint8_t*)pbase(), 
                            (const uint8_t*)pptr(),
                            _eof);
    input = _mode.prelude_encrypt(input);
    if (input.size() == 0) {
        return -1;
    }
    if (_mode.encryption_direction() == basic_mode::direction_t::forward) {
        _algorithm.encrypt(input, output, _mode.keying().key());
    }
    else {
        _algorithm.decrypt(input, output, _mode.keying().key());
    }
    output = _mode.postlude_encrypt(output);

    /* Write the block */
    int nwritten = 0;
    while (nwritten < output.size()) {
        int nr = _chain->sputn((const char*)output.data() + nwritten,
                               output.size() - nwritten);
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
