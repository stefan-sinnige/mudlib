#include "mud/html/cdata_section.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

cdata_section::cdata_section() : node(node::type_t::CDATA_SECTION) {}

cdata_section::cdata_section(const std::string& data)
  : node(node::type_t::CDATA_SECTION), _text(data)
{}

cdata_section::~cdata_section() {}

cdata_section::cdata_section(const cdata_section& rhs)
  : node(node::type_t::CDATA_SECTION)
{
    (void)operator=(rhs);
}

cdata_section::cdata_section(cdata_section&& rhs)
  : node(node::type_t::CDATA_SECTION)
{
    *this = std::move(rhs);
}

cdata_section&
cdata_section::operator=(const cdata_section& rhs)
{
    if (&rhs != this) {
        _text = rhs._text;
    }
    return *this;
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
END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
