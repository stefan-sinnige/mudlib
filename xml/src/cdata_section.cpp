#include "mud/xml/cdata_section.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ cdata_section::ptr
cdata_section::create()
{
    return std::shared_ptr<xml::cdata_section>(new cdata_section());
}

cdata_section::cdata_section() : node(node::type_t::CDATA_SECTION) {}

cdata_section::~cdata_section() {}

cdata_section::cdata_section(cdata_section&& rhs)
  : node(node::type_t::CDATA_SECTION)
{
    *this = std::move(rhs);
}

cdata_section&
cdata_section::operator=(cdata_section&& rhs)
{
    _text = std::move(rhs._text);
    return *this;
}

const std::string&
cdata_section::text() const
{
    return _text;
}

void
cdata_section::text(const std::string& value)
{
    _text = value;
}

void
cdata_section::text(std::string&& value)
{
    _text = std::move(value);
}
END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
