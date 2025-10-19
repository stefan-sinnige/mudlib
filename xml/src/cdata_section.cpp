#include "mud/xml/cdata_section.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ cdata_section::ptr
cdata_section::create(const std::string& text)
{
    struct make_shared_enabler: public xml::cdata_section
    {
        make_shared_enabler(const std::string& text)
            : xml::cdata_section(text)
        {}  
    };
    return std::make_shared<make_shared_enabler>(text);
}

cdata_section::cdata_section(const std::string& text)
    : node(node::type_t::CDATA_SECTION)
    , _text(text)
{
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
