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

#include "mud/crypto/exception.h"
#include "mud/crypto/types.h"
#include <algorithm>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <cstring>
#include <cctype>

BEGIN_MUDLIB_CRYPTO_NS

counter_t::counter_t(size_t sz, uint64_t value)
    : mud::core::buffer(sz)
{
    uint64_t mask = 0x00FF;
    auto cptr = data() + size() - 1;
    auto eptr = data();
    for (size_t b = 0; b < 8 && cptr >= eptr; ++b, --cptr) {
        *cptr = (uint8_t)((value & mask) >> b*8);
        mask <<= 8;
    }
}

counter_t&
counter_t::operator++()
{
    uint8_t incr = 1;
    auto cptr = data() + size() - 1;
    auto eptr = data();
    for (size_t b = 0; b < 8 && cptr >= eptr; ++b, --cptr) {
        uint16_t value = ((uint16_t)*cptr) + incr;
        *cptr = (uint8_t)(value & 0x00FF);
        incr = value >> 8;
    }
    return *this;
}

random_t::random_t(size_t sz)
    : mud::core::buffer(sz)
{
    std::random_device rd;
    if (rd.max() >= UINT32_MAX) {
        /* Get random values as 32-bit values. */
        uint32_t value = rd();
        for (int i = 0; i < size() / 4; ++i) {
            *((uint32_t*)(data() + 4*i)) = value;
            value = rd();
        }
        for (int b = 0; b < size() % 4; ++b) {
            *(data() + (size() / 4)*4 + b) = *(((uint8_t*)&value) + b);
        }
    }
    else {
        /* Get random values as 8-bit values which is likely to be less
         * performant due to a call to the random device for each byte. */
        uint8_t value = rd();
        for (int b = 0; b < size(); ++b) {
            *(data() + b) = value;
            value = rd();
        }
    }
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
