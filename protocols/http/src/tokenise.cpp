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

#include "tokenise.h"

BEGIN_MUDLIB_HTTP_NS

std::string
tokenise(std::istream& istr, const token_manip& manip)
{
    /* The standard table of allowable characters. Additional characters
     * that an be governed by the token manipulators are *not* included. */
    static uint8_t table[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // NUL .. SI
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // DLE .. US
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, //  SP .. /
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, //   0 .. ?
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   @ .. O
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   P .. _
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   ` .. o
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  //   p .. DEL
    };

    /* Read all characters that are part of a token */
    int ch;
    char buf[128];
    int n = 0;
    std::string str;
    while ((ch = istr.get()) != std::char_traits<char>::eof()) {
        if ((ch > 0 && ch < 127 && table[ch] == 1) ||
            (manip.space && ch == SP) || (manip.colon && ch == CL) ||
            (manip.comma && ch == CM)) 
        {
            buf[n++] = ch;
            if (n >= sizeof(buf)) {
                str.append(buf, n);
                n = 0;
            }
        } else {
            break;
        }
    }
    buf[n] = '\0';
    str.append(buf, n);
    istr.unget();
    return str;
}

/*
 * Expect a specific character to be read next from an input stream. Throws
 * a 'malformed HTTP message' error if the next character does not match.
 */
std::istream&
expect(std::istream& istr, int expected)
{
    int ch = istr.get();
    if (ch != expected) {
        throw std::runtime_error("Malformed HTTP message");
    }
    return istr;
}

/*
 * Skip whitespace according to its type.
 */
std::istream&
whitespace(std::istream& istr, ws_t type)
{
    bool found = false; 
    char ch = istr.peek();
    while (ch == ' ' || ch == '\t') {
        found = true;
        istr.get();
        ch = istr.peek();
    }
    if (type == RWS && !found) {
        throw std::runtime_error("Malformed HTTP message");
    }
    return istr;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
