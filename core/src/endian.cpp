#include "mud/core/endian.h"

BEGIN_MUDLIB_CORE_NS

/* static */ endian::endian_t
endian::native()
{
    // Runtime determination of system endian
    static endian::endian_t g_native = endian::endian_t::unresolved;
    if (g_native == endian::endian_t::unresolved) {
        uint32_t value = 0x01234567;
        uint8_t* s = (uint8_t*)&value;
        if (s[0] == 0x67 && s[1] == 0x45 && s[2] == 0x23 && s[3] == 0x01) {
            g_native = endian::endian_t::little;
        } else {
            g_native = endian::endian_t::big;
        }
    }
    return g_native;
}

/* ======================================================================
 * Little to Big Endian (and reverse)
 * ====================================================================== */

uint16_t
le_be(uint16_t value)
{
    uint8_t* s = (uint8_t*)&value;
    return (uint16_t)(((uint16_t)s[0]) << 8 | ((uint16_t)s[1]));
}

uint32_t
le_be(uint32_t value)
{
    uint8_t* s = (uint8_t*)&value;
    return (uint32_t)(((uint32_t)s[0]) << 24 | ((uint32_t)s[1]) << 16 |
                      ((uint32_t)s[2]) << 8 | ((uint32_t)s[3]));
}

uint64_t
le_be(uint64_t value)
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
endian::convert(endian::endian_t from, endian::endian_t to, uint16_t value)
{
    if (from == to) {
        return value;
    }
    return le_be(value);
}

/* static */ uint32_t
endian::convert(endian::endian_t from, endian::endian_t to, uint32_t value)
{
    if (from == to) {
        return value;
    }
    return le_be(value);
}

/* static */ uint64_t
endian::convert(endian::endian_t from, endian::endian_t to, uint64_t value)
{
    if (from == to) {
        return value;
    }
    return le_be(value);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
