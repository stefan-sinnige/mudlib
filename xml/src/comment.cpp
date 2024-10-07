#include "mud/xml/comment.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ comment::ptr
comment::create()
{
    return std::shared_ptr<xml::comment>(new comment());
}

comment::comment() : node(node::type_t::COMMENT) {}

comment::~comment() {}


comment::comment(comment&& rhs) : node(node::type_t::COMMENT)
{
    *this = std::move(rhs);
}

comment&
comment::operator=(comment&& rhs)
{
    _text = std::move(rhs._text);
    return *this;
}

const std::string&
comment::text() const
{
    return _text;
}

void
comment::text(const std::string& value)
{
    _text = value;
}

void
comment::text(std::string&& value)
{
    _text = std::move(value);
}
END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
