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

#include <iostream>
#include <string>
#include "mud/http/request.h"
#include "mud/http/response.h"

BEGIN_MUDLIB_HTTP_NS

/*
 * Special characters with specific meaning in the HTTP protocol.
 */
const char SP = 32;
const char CR = 13;
const char LF = 10;
const char CL = 58;
const char HT =  9;
const char DQ = 34;
const char CM = 44;

/*
 * Token manipulation options. Extends the allowable characters when
 * tokenising the input stream.
 */
struct token_manip
{
    bool space; /* Include a space to be part of the token. */
    bool colon; /* Include a colon to be part of the token. */
    bool comma; /* Include a comma to be part of the token. */
};
constexpr token_manip include_none = { 0, 0, 0 };
constexpr token_manip include_space = { 1, 0, 0 };
constexpr token_manip include_colon = { 0, 1, 0 };
constexpr token_manip include_all = { 1, 1, 1 };

/*
 * Return the next HTTP message token. Additional token manipulation
 * options can be provided.
 */
std::string
tokenise(std::istream& istr, const token_manip& manip);

/*
 * Expect a specific character to be read next from an input stream. Throws
 * a 'malformed HTTP message' error if the next character does not match.
 */
std::istream&
expect(std::istream& istr, int expected);

/*
 * The witespace options.
 */
typedef char ws_t;
constexpr ws_t OWS = 0;   /* Optional whitespace. */
constexpr ws_t RWS = 1;   /* Required whitespace. */
constexpr ws_t BWS = OWS; /* Bad whitespace (allowed for historical reasons). */

/*
 * Skip any whitespace. Throws a 'malformed HTTP message' error if the white
 * space is not conform the specifications. The whitespace will not be part of
 * any token or value.
 */
std::istream&
whitespace(std::istream& istr, ws_t type);

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
