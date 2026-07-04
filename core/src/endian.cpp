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

BEGIN_MUDLIB_CORE_NS

/* static */ endian::endian_t
endian::native()
{
    static endian::endian_t g_native = endian::endian_t::native;
    switch (g_native) {
        case endian::endian_t::native: {
            /* Runtime determination of system endian */
            uint32_t value = 0x01234567;
            uint8_t* s = (uint8_t*)&value;
            if (s[0] == 0x67 && s[1] == 0x45 && s[2] == 0x23 && s[3] == 0x01) {
                g_native = endian::endian_t::little;
            } else {
                g_native = endian::endian_t::big;
            }
            break; }
        default:
            break;
    }
    return g_native;
}

bool
operator==(endian::endian_t lhs, endian::endian_t rhs)
{
    auto nat_ut = std::underlying_type_t<endian::endian_t>(
            endian::endian_t::native);
    auto lhs_ut = std::underlying_type_t<endian::endian_t>(lhs);
    auto rhs_ut = std::underlying_type_t<endian::endian_t>(rhs);
    if (lhs_ut == nat_ut) {
        lhs_ut = std::underlying_type_t<endian::endian_t>(endian::native());
    }
    if (rhs_ut == nat_ut) {
        rhs_ut = std::underlying_type_t<endian::endian_t>(endian::native());
    }
    return lhs_ut == rhs_ut;
}

bool
operator!=(endian::endian_t lhs, endian::endian_t rhs)
{
    auto nat_ut = std::underlying_type_t<endian::endian_t>(
            endian::endian_t::native);
    auto lhs_ut = std::underlying_type_t<endian::endian_t>(lhs);
    auto rhs_ut = std::underlying_type_t<endian::endian_t>(rhs);
    if (lhs_ut == nat_ut) {
        lhs_ut = std::underlying_type_t<endian::endian_t>(endian::native());
    }
    if (rhs_ut == nat_ut) {
        rhs_ut = std::underlying_type_t<endian::endian_t>(endian::native());
    }
    return lhs_ut != rhs_ut;
}

/* ======================================================================
 * Swap endian systems
 * ====================================================================== */

uint16_t
swap(uint16_t value)
{
    uint8_t* s = (uint8_t*)&value;
    return (uint16_t)(((uint16_t)s[0]) << 8 | ((uint16_t)s[1]));
}

uint32_t
swap(uint32_t value)
{
    uint8_t* s = (uint8_t*)&value;
    return (uint32_t)(((uint32_t)s[0]) << 24 | ((uint32_t)s[1]) << 16 |
                      ((uint32_t)s[2]) << 8 | ((uint32_t)s[3]));
}

uint64_t
swap(uint64_t value)
{
    uint8_t* s = (uint8_t*)&value;
    return (uint64_t)(((uint64_t)s[0]) << 56 | ((uint64_t)s[1]) << 48 |
                      ((uint64_t)s[2]) << 40 | ((uint64_t)s[3]) << 32 |
                      ((uint64_t)s[4]) << 24 | ((uint64_t)s[5]) << 16 |
                      ((uint64_t)s[6]) << 8 | ((uint64_t)s[7]));
}

/* ======================================================================
 * Conversion specialisations
 * ====================================================================== */

/* static */ uint16_t
endian::_convert(endian::endian_t from, endian::endian_t to, uint16_t value)
{
    if (from == to) {
        return value;
    }
    return swap(value);
}

/* static */ uint32_t
endian::_convert(endian::endian_t from, endian::endian_t to, uint32_t value)
{
    if (from == to) {
        return value;
    }
    return swap(value);
}

/* static */ uint64_t
endian::_convert(endian::endian_t from, endian::endian_t to, uint64_t value)
{
    if (from == to) {
        return value;
    }
    return swap(value);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
