#include "mud/xml/char_data.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ char_data::ptr
char_data::create(const std::string& text)
{
    return std::shared_ptr<xml::char_data>(new char_data(text));
}

char_data::char_data(const std::string& text)
    : node(node::type_t::CHAR_DATA)
    , _text(text)
{}

const std::string&
char_data::text() const
{
    return _text;
}

void
char_data::text(const std::string& value)
{
    _text = value;
}

void
char_data::text(std::string&& value)
{
    _text = std::move(value);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
