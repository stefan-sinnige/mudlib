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

#include "mud/html/char_data.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

char_data::char_data() : node(node::type_t::CHAR_DATA) {}

char_data::char_data(const std::string& data)
  : node(node::type_t::CHAR_DATA), _text(data)
{}

char_data::~char_data() {}

char_data::char_data(const char_data& rhs) : node(node::type_t::CHAR_DATA)
{
    (void)operator=(rhs);
}

char_data::char_data(char_data&& rhs) : node(node::type_t::CHAR_DATA)
{
    *this = std::move(rhs);
}

char_data&
char_data::operator=(const char_data& rhs)
{
    if (&rhs != this) {
        _text = rhs._text;
    }
    return *this;
}

char_data&
char_data::operator=(char_data&& rhs)
{
    _text = std::move(rhs._text);
    return *this;
}

const std::string&
char_data::text() const
{
    return _text;
}

void
char_data::text(const std::string& value)
{
    _text = value;
}

void
char_data::text(std::string&& value)
{
    _text = std::move(value);
}
END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
