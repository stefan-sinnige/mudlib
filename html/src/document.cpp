#include "mud/html/document.h"
#include "mud/html/exception.h"
#include <algorithm>
#include <utility>

BEGIN_MUDLIB_HTML_NS

document::document() {}

document::~document() {}

document::document(const document& rhs)
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

document::document(document&& rhs)
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
