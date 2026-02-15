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

#include "mud/http/chunk.h"
#include "tokenise.h"
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

std::ostream&
operator<<(std::ostream& ostr, const chunk& body)
{
    ostr << std::hex << body.size() << std::dec;
    ostr << CR << LF;
    ostr << body.data();
    ostr << CR << LF;
    return ostr;
}

std::istream&
operator>>(std::istream& istr, chunk& body)
{
    // Read the hexadecimal size. Support up to 32-bits (which is ridiculous).
    uint32_t size;
    istr >> std::hex >> size >> std::dec;
    expect(istr, CR);
    expect(istr, LF);

    // Read the data
    if (size > 0) {
        std::string data;
        data.resize(size);
        istr.read(&data[0], size);
        body = chunk(data);
    } else {
        body = chunk();
    }
    expect(istr, CR);
    expect(istr, LF);
    return istr;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
