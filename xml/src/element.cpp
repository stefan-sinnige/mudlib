#include "mud/xml/element.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ element::ptr
element::create()
{
    return std::shared_ptr<xml::element>(new element());
}

element::element() : node(node::type_t::ELEMENT) {}

element::~element() {}

element::element(element&& rhs) : node(node::type_t::ELEMENT)
{
    *this = std::move(rhs);
}

element&
element::operator=(element&& rhs)
{
    _name = std::move(rhs._name);
    _attributes = std::move(rhs._attributes);
    _children = std::move(rhs._children);
    _parent = std::move(rhs._parent);
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

const mud::xml::attribute_set&
element::attributes() const
{
    return _attributes;
}

void
element::attributes(const mud::xml::attribute_set& value)
{
    _attributes = value;
}

void
element::attributes(mud::xml::attribute_set&& value)
{
    _attributes = std::move(value);
}

void
element::attribute(const mud::xml::attribute::ptr& attr)
{
    _attributes.insert(attr);
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
}

void
element::children(mud::xml::node_seq&& value)
{
    _children = std::move(value);
}

void
element::child(const mud::xml::node::ptr& child)
{
    _children.push_back(child);
}

std::shared_ptr<mud::xml::node>
element::parent() const
{
    return _parent;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
