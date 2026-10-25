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

#include "mud/core/hexdump.h"
#include "mud/core/buffer.h"
#include <cctype>
#include <iomanip>

BEGIN_MUDLIB_CORE_NS

hexdump::hexdump(const char* data, size_t size)
    : _data((const uint8_t*)data)
    , _size(size)
{
}

hexdump::hexdump(const uint8_t* data, size_t size)
    : _data(data)
    , _size(size)
{
}

hexdump::hexdump(const buffer& buf)
    : _data(buf.data())
    , _size(buf.size())
{
}

hexdump::hexdump(const buffer_view& buf)
    : _data(buf.data())
    , _size(buf.size())
{
}

std::ostream&
operator<<(std::ostream& ostr, const hexdump& value)
{
    static const char* hex = "0123456789abcdef";
    const uint8_t* ptr = value._data;
    size_t size = value._size;
    size_t off = 0;
    size_t off_width = (size > 0xFFFF ? 8 : 4);
    auto saved_flags = ostr.flags();
    ostr << std::right << std::hex << std::nouppercase << std::setfill('0');
    do {
        /* Insert a new line, unless we're at the first offset */
        if (off != 0) {
            ostr << std::endl;
        }

        /* Output the offset */
        ostr << std::setw(off_width) << off << " ";

        /* Output the hexadecimal values */
        for (int i = 0; i < 16; ++i) {
            if ((off + i) < size) {
                ostr << " "
                     << hex[(*(ptr + off + i) >> 4) & 0x0F]
                     << hex[*(ptr + off + i) & 0x0F];
            }
            else {
                ostr << "   ";
            }
            if (i == 7) {
                ostr << " ";
            }
        }
        ostr << "  ";

        /* Output the character values */
        for (int i = 0; i < 16; ++i) {
            char c = ' ';
            if ((off + i) < size) {
                c = (isprint((int)(*(ptr + off + i)))) 
                    ? (char)(*(ptr + off + i)) : '.';
            }
            ostr << c;
        }

        /* Next block */
        off += 16;
    }
    while(off < size);
    ostr.flags(saved_flags);
    return ostr;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
