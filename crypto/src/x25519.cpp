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
#include "mud/crypto/exception.h"
#include "mud/crypto/x25519.h"
#include <cstring>
#include <iomanip>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

/**
 * Internal logging is disabled for seurity reasons and performance. Only enable
 * it for development purposes.
 */
#define X25519_LOGGING true
#if X25519_LOGGING
    #warning "CAUTION: X25519 logging is enabled"
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

x25519::field_element::field_element()
{
    memset(_value, 0, sizeof(_value));
}

x25519::field_element::field_element(const data_t& data)
{
    if (data.size() != 32) {
        throw size_error("x25519 element is not 32 bytes");
    }

    /* Unpacking a little-endian binary buffer into an array of 16 64-bit signed
     * integers in native endian. Each two bytes of the data are assigned to a
     * 64-bit integer value. The final value clears the MSB bit (255) such that
     * the value is in the range of [0 - 2^255-1]. */
    for (int i = 0; i < 16; ++i) {
        _value[i] = endian::convert(
                endian_t::little, endian::native(),
                *(uint16_t*)(data.data() + 2*i));
    }
    _value[15] &= 0x7FFF;
}

x25519::field_element::operator data_t() const
{
    /* Packing the array of 16 64-bit signed integers to a binary buffer. Reduce
     * the value. We have three options after the initial reduction:
     *   0 <= t < p      : Already reduced
     *   p <= t < 2p     : Subtract with p
     *   2p <= t < 2^256 : Subtract with 2p
     * To ensure we are constant-time we calculate both t-p and t-2p, then use
     * the 'swap' to determing which we should be using. */
    field_element t = *this;
    carry(t); carry(t); carry(t);
    field_element m;
    bool c;
    for (int j = 0; j < 2; ++j) {
        m[0] = t[0] - 0xFFED;
        for (int i = 1; i < 15; ++i) {
            m[i] = t[i] - 0xFFFF - ((m[i-1] >> 16) & 1);
            m[i-1] &= 0xFFFF;
        }
        m[15] = t[15] - 0x7FFF - ((m[14] >> 16) & 1);
        c = 1 - (m[15] >> 16) & 1;
        m[14] &= 0xFFFF;
        swap(t, m, c);
    }

    /* Move all lower 16-bits of each field-element value to the binary buffer
     * in little endian. */
    data_t result(32);
    for (int i = 0; i < 16; ++i) {
        *((uint16_t*)(result.data() + 2*i)) = endian::convert(
                endian::native(),
                endian_t::little,
                (uint16_t)(t[i] & 0xFFFF));
    }

    return result;
}

x25519::field_element&
x25519::field_element::operator+=(const x25519::field_element& rhs)
{
    *this = *this + rhs;
    return *this;
}

x25519::field_element&
x25519::field_element::operator-=(const x25519::field_element& rhs)
{
    *this = *this - rhs;
    return *this;
}

x25519::field_element&
x25519::field_element::operator*=(const x25519::field_element& rhs)
{
    *this = *this * rhs;
    return *this;
}

void
x25519::field_element::swap(x25519::field_element& p, x25519::field_element& q, bool c)
{
    int64_t mask = ~(c - 1);
    int64_t t;
    for (int i = 0; i < 16; ++i) {
        t = mask & (p._value[i] ^ q._value[i]);
        p._value[i] ^= t;
        q._value[i] ^= t;
    }
}

void
x25519::field_element::carry(x25519::field_element& e)
{
    /* From the least significant value (first in the array), grab the carry of
     * the upper 48 bits and add them to the next alue.  On the last element,
     * perform modulo 2p reduction (multiply by 38). */
    int64_t carry;
    for (int i  = 0; i < 16; ++i) {
        carry = e[i] >> 16;
        e[i] -= carry << 16;
        if (i < 15) {
            e[i+1] += carry;   
        }
        else {
            e[0] += 38 * carry;
        }
    }
}

std::ostream&
operator<<(std::ostream& ostr, const x25519::field_element& e)
{
    auto saved_flags = ostr.flags();
    ostr << std::hex << std::setfill('0');
    for (int i = 0; i < 4; ++i) {
        ostr  << std::setw(2) << 4*i << "|"
              << std::setw(16) << e[4*i] << " "
              << std::setw(2) << 4*i+1 << "|"
              << std::setw(16) << e[4*i+1] << " "
              << std::setw(2) << 4*i+2 << "|"
              << std::setw(16) << e[4*i+2] << " "
              << std::setw(2) << 4*i+3 << "|"
              << std::setw(16) << e[4*i+3];
        if (i < 3) {
            ostr << std::endl;
        }
    }
    ostr.flags(saved_flags);
    return ostr;
}

x25519::x25519()
    : x25519(random_t(32))
{
}

x25519::x25519(const data_t& p)
    : _basepoint(32)
    , _private(p)
{
    /** The public base-point is always 9 */
    _basepoint[0] = 9;

    /** The public key is the private key multiplied by the base point */
    _public = mult(_private, _basepoint);

    LOG(log);
    TRACE(log) << "   Public Key| " << _public << std::endl
               << "  Private Key| " << _private << std::endl;
}

void
x25519::peer(const data_t& key)
{
    _shared = mult(_private, key);
}

x25519::field_element
x25519::mult(const x25519::field_element& s, const x25519::field_element& point)
{
    /**
     * The scalar value is 'clamped' (some bits are overridden) for security
     * reasons:
     *   - bits 0,1,2 set to 0 to protect against small-subgroup attacks
     *   - bit 254 set to 1 to prevent skipping a multiplication round and
     *     protect against a timing leak
     *   - bit 255 set to 0 to prevent reaching infinity for x=9
     */
    x25519::field_element scalar = s;
    scalar[0] &= 0xFFF8;
    scalar[15] = (scalar[15] & 0x7FFF) | 0x4000;

    x25519::field_element s121665;
    s121665[0] = 0xDB41;
    s121665[1] = 0x0001;
    x25519::field_element a, b, c, d, e, f, x;
    a[0] = d[0] = 1;
    b = x = point;
    int printi = 254;
    for(int i = 254; i >= 0; --i) {
        int bit = (scalar[i >> 4] >> (i % 16)) & 1;
        x25519::field_element::swap(a, b, bit);
        x25519::field_element::swap(c, d, bit);
        e = a + c;
        a = a - c;
        c = b + d;
        b = b - d;
        d = e * e;
        f = a * a;
        a = c * a;
        c = b * e;
        e = a + c;
        a = a - c;
        b = a * a;
        c = d - f;
        a = c * s121665;
        a = a + d;
        c = c * a;
        a = d * f;
        d = b * x;
        b = e * e;
        x25519::field_element::swap(a, b, bit);
        x25519::field_element::swap(c, d, bit);
    }
    c = inverse(c);
    a = a * c;
    return a;
}

x25519::field_element
operator+(const x25519::field_element& lhs, const x25519::field_element& rhs)
{
    x25519::field_element result;
    for (int i = 0; i < 16; ++i) {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

x25519::field_element
operator-(const x25519::field_element& lhs, const x25519::field_element& rhs)
{
    x25519::field_element result;
    for (int i = 0; i < 16; ++i) {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

x25519::field_element
operator*(const x25519::field_element& lhs, const x25519::field_element& rhs)
{
    int64_t product[31];
    for (int i = 0; i < 31; ++i) {
        product[i] = 0;
    }
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            product[i+j] += lhs[i] * rhs[j];
        }
    }
    for (int i = 0; i < 15; ++i) {
        product[i] += 38 * product[i + 16];
    }

    x25519::field_element result;
    for (int i = 0; i < 16; ++i) {
        result[i] = product[i];
    }
    x25519::field_element::carry(result);
    x25519::field_element::carry(result);
    return result;
}

x25519::field_element
inverse(const x25519::field_element& rhs)
{
    x25519::field_element result = rhs;
    for (int i = 253; i >= 0; --i) {
        result = result * result;
        if (i != 2 && i != 4) {
            result = result * rhs;
        }
    }
    return result;
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
