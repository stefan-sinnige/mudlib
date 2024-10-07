#include "mud/xml/document.h"
#include "mud/xml/exception.h"
#include <algorithm>
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ document::ptr
document::create()
{
    return std::shared_ptr<xml::document>(new document());
}
document::document() : node(node::type_t::DOCUMENT)
{}

document::~document() {}

document::document(document&& rhs) : node(node::type_t::DOCUMENT)
{
    *this = std::move(rhs);
}

document&
document::operator=(document&& rhs)
{
    _children = std::move(rhs._children);
    return *this;
}

std::shared_ptr<mud::xml::element>
document::root() const
{
    auto found = std::find_if(_children.cbegin(), _children.cend(),
        [](const std::shared_ptr<mud::xml::node>& node) {
            return node->type() == node::type_t::ELEMENT;
        });
    if (found != _children.cend()) {
        return std::static_pointer_cast<mud::xml::element>(*found);
    } else {
        throw not_found("document has no root element");
    }
}

const mud::xml::node_seq&
document::children() const
{
    return _children;
}

void
document::children(const mud::xml::node_seq& value)
{
    _children = value;
}

void
document::children(mud::xml::node_seq&& value)
{
    _children = std::move(value);
}

void
document::child(const mud::xml::node::ptr& node)
{
    _children.push_back(node);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
