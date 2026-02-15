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

#include "mud/html/attribute.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

attribute::attribute() {}

attribute::attribute(const std::string& name, const std::string& value)
  : _name(name), _value(value)
{}

attribute::~attribute() {}

attribute::attribute(const attribute& rhs)
{
    (void)operator=(rhs);
}

attribute&
attribute::operator=(const attribute& rhs)
{
    if (&rhs != this) {
        _name = rhs._name;
        _value = rhs._value;
    }
    return *this;
}

attribute::attribute(attribute&& rhs)
{
    *this = std::move(rhs);
}

attribute&
attribute::operator=(attribute&& rhs)
{
    _name = std::move(rhs._name);
    _value = std::move(rhs._value);
    return *this;
}

const std::string&
attribute::name() const
{
    return _name;
}

void
attribute::name(const std::string& value)
{
    _name = value;
}

void
attribute::name(std::string&& value)
{
    _name = std::move(value);
}

const std::string&
attribute::value() const
{
    return _value;
}

void
attribute::value(const std::string& value)
{
    _value = value;
}

void
attribute::value(std::string&& value)
{
    _value = std::move(value);
}

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
