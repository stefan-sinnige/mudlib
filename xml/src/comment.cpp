#include "mud/xml/comment.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ comment::ptr
comment::create(const std::string& text)
{
    return std::shared_ptr<xml::comment>(new comment(text));
}

comment::comment(const std::string& text)
    : node(node::type_t::COMMENT)
    , _text(text)
{
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
