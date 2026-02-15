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

#include "mud/html/document.h"
#include "mud/html/exception.h"
#include <algorithm>
#include <utility>

BEGIN_MUDLIB_HTML_NS

document::document() : node(node::type_t::DOCUMENT) {}

document::~document() {}

document::document(const document& rhs) : node(node::type_t::DOCUMENT)
{
    (void)operator=(rhs);
}

document&
document::operator=(const document& rhs)
{
    if (&rhs != this) {
        _nodes = rhs._nodes;
    }
    return *this;
}

document::document(document&& rhs) : node(node::type_t::DOCUMENT)
{
    *this = std::move(rhs);
}

document&
document::operator=(document&& rhs)
{
    _nodes = std::move(rhs._nodes);
    return *this;
}

element&
document::root()
{
    auto found =
        std::find_if(_nodes.begin(), _nodes.end(), [](const node& node) {
            return node.type() == node::type_t::ELEMENT;
        });
    if (found != _nodes.end()) {
        return dynamic_cast<element&>(*found);
    } else {
        throw not_found("document has no root element");
    }
}

const element&
document::root() const
{
    auto found =
        std::find_if(_nodes.cbegin(), _nodes.cend(), [](const node& node) {
            return node.type() == node::type_t::ELEMENT;
        });
    if (found != _nodes.cend()) {
        return dynamic_cast<const element&>(*found);
    } else {
        throw not_found("document has no root element");
    }
}

const mud::core::poly_vector<mud::html::node>&
document::nodes() const
{
    return _nodes;
}

mud::core::poly_vector<mud::html::node>&
document::nodes()
{
    return _nodes;
}

void
document::nodes(const mud::core::poly_vector<mud::html::node>& value)
{
    _nodes = value;
}

void
document::nodes(mud::core::poly_vector<mud::html::node>&& value)
{
    _nodes = std::move(value);
}

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
