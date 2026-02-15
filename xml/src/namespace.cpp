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

#include "mud/xml/namespace.h"

BEGIN_MUDLIB_XML_NS

/* static */ ns::ptr
ns::create()
{
    struct make_shared_enabler: public xml::ns
    {
        make_shared_enabler()
            : xml::ns()
        {}  
    };
    return std::make_shared<make_shared_enabler>();
}

ns::ns()
    : _resolved(false)
{
}

const mud::core::uri&
ns::uri() const
{
    return _uri;
}

void
ns::uri(const mud::core::uri& value)
{
    _uri = value;
}

void
ns::uri(mud::core::uri&& value)
{
    _uri = std::move(value);
}

const std::string&
ns::prefix() const
{
    return _prefix;
}

void
ns::prefix(const std::string& value)
{
    _prefix = value;
}

void
ns::prefix(std::string&& value)
{
    _prefix = std::move(value);
}

bool
ns::resolved() const
{
    return _resolved;
}

void
ns::resolved(bool value)
{
    _resolved = value;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
