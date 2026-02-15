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

#include <mud/core/utf.h>
#include <iomanip>

BEGIN_MUDLIB_CORE_NS

/* ==========================================================================
 * Unicode codepoint
 * ========================================================================== */

codepoint::codepoint()
    : _cp(0)
{
}

codepoint::codepoint(uint32_t cp)
    : _cp(cp)
{
    // Valid ranges are 0x0000 (plane 0) to 0x10FFFF (plane 16), excluding
    // 0xD800 - 0xDFFF (surrogates).
    if (_cp > 0x10FFFF || (_cp >= 0xD800 && _cp <= 0xDFFF)) {
        _cp = 0;
    }
}

codepoint::codepoint(const std::string& cp)
    : _cp(0)
{
    // If the string is not of the right size, bail out early.
    if (cp.size() < 6 || cp.size() > 8) {
        return;
    }

    // If it does not start with "U+", bail out.
    if ((cp[0] != 'U') || (cp[1] != '+')) {
        return;
    }

    // Treat the remainder as hexadecimal nibbles.
    for (int i = 2; i < cp.size(); ++i) {
        char nibble = cp[i];
        if ((nibble >= '0') && (nibble <= '9')) {
            _cp = (_cp << 4) | (nibble - '0');
        }
        else
        if ((nibble >= 'A') && (nibble <= 'F')) {
            _cp = (_cp << 4) | (nibble - 'A' + 10);
        }
        else
        if ((nibble >= 'a') && (nibble <= 'f')) {
            _cp = (_cp << 4) | (nibble - 'a' + 10);
        }
        else {
            // Not a valid hexadecimal nibble
            _cp = 0;
            return;
        }
    }

    // Valid ranges are 0x0000 (plane 0) to 0x10FFFF (plane 16)
    if (_cp > 0x10FFFF) {
        _cp = 0;
    }
}

bool
codepoint::operator==(const codepoint& rhs) const
{
    return _cp == rhs._cp;
}

bool
codepoint::operator!=(const codepoint& rhs) const
{
    return !operator==(rhs);
}

uint8_t
codepoint::plane() const
{
    return _cp >> 16;
}

codepoint::operator uint32_t() const
{
    return _cp;
}

/* ==========================================================================
 * UTF-8
 * ========================================================================== */

utf8::utf8()
{
    _ch = {0x00};
}

utf8::utf8(codepoint cp)
{
    if ((cp & ~0x0000007F) == 0) {
        // 1 Byte 0yyyzzzz
        _ch = {
            static_cast<uint8_t>(cp & 0x0000007F)
        };
    }
    else
    if ((cp & ~0x000007FF) == 0) {
        // 2 Bytes 110xxxyy 10yyzzzz
        _ch = {
            static_cast<uint8_t>(0xC0 | (cp & 0x000007C0) >> 6),
            static_cast<uint8_t>(0x80 | (cp & 0x0000003F))
        };
    }
    else
    if ((cp & ~0x0000FFFF) == 0) {
        // 3 Bytes 1110wwww 10xxxxyy 10yyzzzz
        _ch = {
            static_cast<uint8_t>(0xE0 | (cp & 0x0000F000) >> 12),
            static_cast<uint8_t>(0x80 | (cp & 0x00000FC0) >> 6),
            static_cast<uint8_t>(0x80 | (cp & 0x0000003F))
        };
    }
    else
    if ((cp & ~0x001FFFFF) == 0) {
        // 4 Bytes 11110uvv 10vvwwww 10xxxxyy 10yyzzzz
        _ch = {
            static_cast<uint8_t>(0xF0 | (cp & 0x001C0000) >> 18),
            static_cast<uint8_t>(0x80 | (cp & 0x0003F000) >> 12),
            static_cast<uint8_t>(0x80 | (cp & 0x00000FC0) >> 6),
            static_cast<uint8_t>(0x80 | (cp & 0x0000003F))
        };
    }
    else {
        _ch = {0x00};
    }
}

bool
utf8::operator==(const utf8& rhs) const
{
    return _ch == rhs._ch;
}

bool
utf8::operator!=(const utf8& rhs) const
{
    return !operator==(rhs);
}

size_t
utf8::size() const
{
    return _ch.size();
}

const utf8::data_type&
utf8::data() const
{
    return _ch;
}

utf8::operator codepoint() const {
    codepoint cp = 0;
    return cp;
}

/* ==========================================================================
 * UTF-16
 * ========================================================================== */

utf16::utf16()
{
    _ch = {0x0000};
}

utf16::utf16(codepoint cp)
{
    if ((cp & ~0x0000FFFF) == 0) {
        // Single UTF-16 character
        _ch = { static_cast<uint16_t>(cp & 0xFFFF) };
    }
    else
    if ((cp & ~0x001FFFFF) == 0) {
        // Double UT-16 character, using High and low surrgate pair
        //    U' = U - 0x10000
        //    W1 = 110110yy yyyyyyyy
        //    W2 = 110111xx xxxxxxxx
        uint32_t u = cp - 0x00010000;
        _ch = {
            static_cast<uint16_t>(0xD800 | (u & 0x000FFC00) >> 10),
            static_cast<uint16_t>(0xDC00 | (u & 0x000003FF))
        };
    }
    else {
        _ch = { 0x0000 };
    }
}

bool
utf16::operator==(const utf16& rhs) const
{
    return _ch == rhs._ch;
}

bool
utf16::operator!=(const utf16& rhs) const
{
    return !operator==(rhs);
}

size_t
utf16::size() const
{
    return _ch.size();
}

const utf16::data_type&
utf16::data() const
{
    return _ch;
}

utf16::operator codepoint() const {
    codepoint cp = 0;
    return cp;
}

/* ==========================================================================
 * UTF-32
 * ========================================================================== */

utf32::utf32()
{
    _ch = 0x00000000;
}

utf32::utf32(codepoint cp)
{
    if ((cp & ~0x001FFFFF) == 0) {
        _ch = cp;
    }
    else {
        _ch = { 0x00000000 };
    }
}

bool
utf32::operator==(const utf32& rhs) const
{
    return _ch == rhs._ch;
}

bool
utf32::operator!=(const utf32& rhs) const
{
    return !operator==(rhs);
}

utf32::data_type
utf32::data() const
{
    return _ch;
}

utf32::operator codepoint() const {
    codepoint cp = 0;
    return cp;
}

/* ==========================================================================
 * Streaming manipulators
 * ========================================================================== */

// Ending stream manipulator, defaults to big-endian (RFC-2781)
const int BE = 0;
const int LE = 1;
static int  __endian_index = std::ios_base::xalloc();

std::ios_base&
utf::be(std::ios_base& str)
{
    str.iword(__endian_index) = BE;
    return str;
}

std::ios_base&
utf::le(std::ios_base& str)
{
    str.iword(__endian_index) = LE;
    return str;
}

END_MUDLIB_CORE_NS

/* ==========================================================================
 * Streaming operators
 * ========================================================================== */

std::istream&
operator>>(std::istream& istr, mud::core::codepoint& cp)
{
    /* Read a code-point */
    cp = mud::core::codepoint();
    int ch = istr.get();
    if (ch == 'U') {
        ch = istr.get();
        if (ch == '+') {
            uint32_t value = 0;
            uint8_t nibbles = 0;
            while ((ch = istr.get()) != std::char_traits<char>::eof())  {
                /* Check if this is a hexedecimal character */
                uint8_t nibble = 0;
                if (ch >= '0' && ch <= '9') {
                    nibble = ch - '0';
                }
                else
                if (ch >= 'a' && ch <= 'f') {
                    nibble = ch - 'a' + 10;
                }
                else
                if (ch >= 'A' && ch <= 'F') {
                    nibble = ch - 'A' + 10;
                }
                else {
                    /* No hexadecimal character */
                    istr.unget();
                    break;
                }

                /* If we're adding a 6th nibble, we can only accept it if the
                 * value will be U+10xxxx */
                if (nibbles == 5) {
                    if ((value & 0xFF000) != 0x10000) {
                        istr.unget();
                        break;
                    }
                }

                /* Add it to the value */
                value <<= 4;
                value |= (nibble & 0x0F);
                if (++nibbles == 6) {
                    break;
                }
            }
            if (nibbles >= 4 && nibbles <= 6) {
                /* Valid value between U+0000 and U+10FFFF */
                cp = value;
                return istr;
            }
        }
    }

    /* Input failure */
    istr.setstate(std::ios_base::failbit);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::core::codepoint& cp)
{
    ostr << "U+";
    std::ios_base::fmtflags saved = ostr.flags();
    if (cp.plane() == 0) {
        ostr << std::hex << std::setfill('0') << std::uppercase << std::setw(4)
             << (uint32_t)cp;
    }
    else {
        ostr << std::hex << std::setfill('0') << std::uppercase << std::setw(5)
             << (uint32_t)cp;
    }
    ostr.flags(saved);
    return ostr;
}

/* vi: set ai ts=4 expandtab: */
