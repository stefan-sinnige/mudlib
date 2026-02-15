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

#include "mud/html/element.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

element::element() : node(node::type_t::ELEMENT) {}

element::element(const std::string& name)
  : node(node::type_t::ELEMENT), _name(name)
{}

element::~element() {}

element::element(const element& rhs) : node(node::type_t::ELEMENT)
{
    (void)operator=(rhs);
}

element::element(element&& rhs) : node(node::type_t::ELEMENT)
{
    *this = std::move(rhs);
}

element&
element::operator=(const element& rhs)
{
    if (&rhs != this) {
        _name = rhs._name;
        _attributes = rhs._attributes;
        _nodes = rhs._nodes;
    }
    return *this;
}

element&
element::operator=(element&& rhs)
{
    _name = std::move(rhs._name);
    _attributes = std::move(rhs._attributes);
    _nodes = std::move(rhs._nodes);
    return *this;
}

const std::string&
element::name() const
{
    return _name;
}

void
element::name(const std::string& value)
{
    _name = value;
}

void
element::name(std::string&& value)
{
    _name = std::move(value);
}

const std::vector<mud::html::attribute>&
element::attributes() const
{
    return _attributes;
}

std::vector<mud::html::attribute>&
element::attributes()
{
    return _attributes;
}

void
element::attributes(const std::vector<mud::html::attribute>& value)
{
    _attributes = value;
}

void
element::attributes(std::vector<mud::html::attribute>&& value)
{
    _attributes = std::move(value);
}

const mud::core::poly_vector<mud::html::node>&
element::nodes() const
{
    return _nodes;
}

mud::core::poly_vector<mud::html::node>&
element::nodes()
{
    return _nodes;
}

void
element::nodes(const mud::core::poly_vector<mud::html::node>& value)
{
    _nodes = value;
}

void
element::nodes(mud::core::poly_vector<mud::html::node>&& value)
{
    _nodes = std::move(value);
}

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
