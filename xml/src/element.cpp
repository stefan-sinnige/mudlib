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

#include "mud/xml/element.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ element::ptr
element::create(const std::string& qname)
{
    struct make_shared_enabler: public xml::element
    {
        make_shared_enabler(const std::string& qname) 
            : xml::element(qname)
        {}  
    };
    return std::make_shared<make_shared_enabler>(qname);
}

/* static */ element::ptr
element::create(const std::string& local_name, const mud::core::uri& uri)
{
    struct make_shared_enabler: public xml::element
    {
        make_shared_enabler(const std::string& local_name,
                            const mud::core::uri& uri)
            : xml::element(local_name, uri)
        {}  
    };
    return std::make_shared<make_shared_enabler>(local_name, uri);
}

element::element(const std::string& qname)
    : node(node::type_t::ELEMENT)
    , _name(qname)
    , _ns(mud::xml::ns::create())
{
    // Split the qualified name into a prefix and local-name
    auto pos = _name.find_first_of(':');
    if (pos == std::string::npos) {
        _local_name = qname;
    }
    else {
        _ns->prefix(qname.substr(0, pos));
        _local_name = qname.substr(pos+1);
    }

}

element::element(const std::string& local_name, const mud::core::uri& uri)
    : node(node::type_t::ELEMENT)
    , _local_name(local_name)
    , _name(local_name)
    , _ns(mud::xml::ns::create())
{
    _ns->uri(uri);
}

const std::string&
element::name() const
{
    return _name;
}

const std::string&
element::prefix() const
{
    return _ns->prefix();
}

const std::string&
element::local_name() const
{
    return _local_name;
}

const mud::xml::attribute_set&
element::attributes() const
{
    return _attributes;
}

void
element::attributes(const mud::xml::attribute_set& value)
{
    _attributes = value;
    for (auto& attr: _attributes) {
        attr->parent(get_shared());
    }
}

void
element::attributes(mud::xml::attribute_set&& value)
{
    _attributes = std::move(value);
    for (auto& attr: _attributes) {
        attr->parent(get_shared());
    }
}

void
element::attribute(const mud::xml::attribute::ptr& attr)
{
    attr->parent(get_shared());
    _attributes.insert(attr);
}

mud::xml::attribute::ptr
element::attribute(const std::string& local_name) const
{
    for (auto& attr: _attributes) {
        if (attr->local_name() == local_name) {
            return attr;
        }
    }
    return nullptr;
}

const mud::xml::node_seq&
element::children() const
{
    return _children;
}

void
element::children(const mud::xml::node_seq& value)
{
    _children = value;
    for (auto& child: _children) {
        child->parent(get_shared());
    }
}

void
element::children(mud::xml::node_seq&& value)
{
    _children = std::move(value);
    for (auto& child: _children) {
        child->parent(get_shared());
    }
}

mud::xml::node_seq
element::elements(const std::string& local_name) const
{
    mud::xml::node_seq seq;
    for (auto& child: _children) {
        if (child->type() == mud::xml::node::type_t::ELEMENT) {
            auto element = std::static_pointer_cast<mud::xml::element>(child);
            if (element->local_name() == local_name) {
                seq.push_back(child);
            }

        }
    }
    return seq;
}

void
element::child(const mud::xml::node::ptr& child)
{
    child->parent(get_shared());
    _children.push_back(child);
}

const mud::xml::ns::ptr&
element::ns() const
{
    return _ns;
}

mud::xml::ns::ptr
element::resolved_ns() const
{
    return resolved_ns(_ns->uri());
}

mud::xml::ns::ptr
element::resolved_ns(const mud::core::uri& uri) const
{
    // Check the attribute of this node for a matching namespace definition with
    // the same URI.
    for (auto& attr: _attributes) {
        if (attr->ns()->resolved() && attr->ns()->uri() == uri) {
            return attr->ns();
        }
    }

    // If there is no parent (ie we're at the document element, we have
    // exhausted all ancestors and no match is found.
    auto ancestor = _parent.lock();
    if (!ancestor ) {
        return nullptr;
    }

    // Check the ancestor until we find the closest match.
    return ancestor->get_shared<mud::xml::element>()->resolved_ns(uri);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
