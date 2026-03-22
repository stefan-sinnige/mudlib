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
#include <iomanip>
#include <cstring>
#include <cctype>

BEGIN_MUDLIB_CRYPTO_NS

data_t::data_t(size_t sz)
    : std::vector<uint8_t>(sz)
{
}

data_t::data_t(std::initializer_list<uint8_t> init)
    : std::vector<uint8_t>(init)
{
}

data_t::data_t(const uint8_t* d, size_t sz)
    : std::vector<uint8_t>(sz)
{
    memcpy(std::vector<uint8_t>::data(), d, sz);
}

void
data_t::clear()
{
    std::vector<uint8_t>::clear();
}

void
data_t::append(const data_t& data)
{
    reserve(size() + data.size());
    insert(end(), data.begin(), data.end());
}

std::ostream&
operator<<(std::ostream& ostr, const data_t& d)
{
    auto cptr = d.data();
    auto eptr = d.data() + d.size();
    auto saved_flags = ostr.flags();
    ostr << std::hex << std::setfill('0');
    while (cptr < eptr) {
        ostr << std::setw(2) << (int)*cptr;
        ++cptr;
    }
    ostr.flags(saved_flags);
    return ostr;
}


std::istream&
operator>>(std::istream& istr, data_t& d)
{
    /* Define a simple hex-digit to a nibble */
    auto hex2nibble = [](char d) -> int {
        if (d >= '0' && d <= '9') {
            return d - '0';
        }
        else 
        if (d >= 'a' && d <= 'f') {
            return d - 'a' + 10;
        }
        else
        if (d >= 'A' && d <= 'F') {
            return d - 'A' + 10;
        }
        return -1;
    };

    /* Work on 16-bytes temporary storage, only add a byte if two nibbles have
     * been read and processed correctly. */
    std::vector<uint8_t> tmp;
    tmp.reserve(16);
    while (true) {
        /* Read next two characters from the stream */
        int dh = istr.get();
        if (dh == std::istream::traits_type::eof()) {
            break;
        }
        int dl = istr.get();
        if (dl == std::istream::traits_type::eof()) {
            istr.putback(dh);
            break;
        }

        /* Check if they are hexadecimal digits */
        int nh = hex2nibble(dh);
        int nl = hex2nibble(dl);
        if (nh == -1 || nl == -1) {
            istr.putback(dl);
            istr.putback(dh);
            break;
        }

        /* Append it */
        tmp.push_back((uint8_t)(nh << 4 | nl));
        if (tmp.size() == 16) {
            d.append(tmp);
            tmp.clear();
        }
    }
    d.append(tmp);
    return istr;
}

data_t
operator^(const data_t& lhs, const data_t& rhs)
{
    if (lhs.size() > rhs.size()) {
        throw size_error("second operand in XOR is smaller than first");
    }
    data_t result = lhs;
    auto sptr = rhs.data();
    auto dptr = result.data();
    auto eptr = result.data() + result.size();
    while (dptr < eptr) {
        *dptr++ ^= *sptr++;
    }
    return result;
}

counter_t::counter_t(size_t sz, uint64_t value)
    : data_t(sz)
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

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
