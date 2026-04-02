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

#include "mud/crypto/padding.h"
#include "mud/crypto/exception.h"

#include <iostream>

BEGIN_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * None padding
 * ========================================================================== */

padding_factory::registrar<padding_t::none,
                           none_padding> _none_registrar;

/* ==========================================================================
 * PKCS#7 padding
 * ========================================================================== */

pkcs7_padding::pkcs7_padding(size_t block_size)
    : basic_padding(padding_t::pkcs7, block_size)
{
}

data_t
pkcs7_padding::pad(const data_t& data) const
{
    /* Determine the pad size */
    data_t result = data;
    uint8_t pad_size;
    if (data.size() % block_size() == 0) {
        pad_size = block_size();
    }
    else {
        pad_size = block_size() - (data.size() % block_size());
    }
    
    /* Add the padding */
    result.append(data_t(pad_size));
    uint8_t* dptr = result.data() + result.size() - pad_size;
    const uint8_t* eptr = result.data() + result.size() - 1;
    while (dptr <= eptr) {
        *dptr++ = pad_size;
    }
    return result;
}

data_t
pkcs7_padding::unpad(const data_t& data) const
{
    if (data.size() < block_size()) {
        throw padding_error("not a PKCS#7 padded block");
    }
    
    /* The last byte should tell how many bytes are padding */
    uint8_t pad_size = *(data.data() + data.size() -1);
    if (pad_size > block_size()) {
        throw padding_error("not a PKCS#7 padded block");
    }

    /* Verify the padding */
    const uint8_t* sptr = data.data() + data.size() - pad_size;
    const uint8_t* eptr = data.data() + data.size() - 1;
    while (sptr <= eptr) {
        if (*sptr++ != pad_size) {
            throw padding_error("not a PKCS#7 padded block");
        }
    }

    /* Remove the padding */
    return data_t(data.data(), data.size() - pad_size);
}

padding_factory::registrar<padding_t::pkcs7,
                           pkcs7_padding> _pkcs7_registrar;

END_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Padding Factory
 * ========================================================================== */

template<>
mud::crypto::padding_factory&
mud::crypto::padding_factory::instance()
{
    static mud::crypto::padding_factory _instance;
    return _instance;
}

/* vi: set ai ts=4 expandtab: */
