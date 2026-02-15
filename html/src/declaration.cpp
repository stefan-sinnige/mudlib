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

#include "mud/html/declaration.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

declaration::declaration()
  : node(node::type_t::DECL), _version("1.0"), _standalone(false)
{}

declaration::~declaration() {}

declaration::declaration(const declaration& rhs) : node(node::type_t::DECL)
{
    (void)operator=(rhs);
}

declaration::declaration(declaration&& rhs) : node(node::type_t::DECL)
{
    *this = std::move(rhs);
}

declaration&
declaration::operator=(const declaration& rhs)
{
    if (&rhs != this) {
        _version = rhs._version;
        _encoding = rhs._encoding;
        _standalone = rhs._standalone;
    }
    return *this;
}

declaration&
declaration::operator=(declaration&& rhs)
{
    _version = std::move(rhs._version);
    _encoding = std::move(rhs._encoding);
    _standalone = std::move(rhs._standalone);
    return *this;
}

const std::string&
declaration::version() const
{
    return _version;
}

void
declaration::version(const std::string& value)
{
    _version = value;
}

void
declaration::version(std::string&& value)
{
    _version = std::move(value);
}

const std::string&
declaration::encoding() const
{
    return _encoding;
}

void
declaration::encoding(const std::string& value)
{
    _encoding = value;
}

void
declaration::encoding(std::string&& value)
{
    _encoding = std::move(value);
}

bool
declaration::standalone() const
{
    return _standalone;
}

void
declaration::standalone(bool value)
{
    _standalone = value;
}

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
