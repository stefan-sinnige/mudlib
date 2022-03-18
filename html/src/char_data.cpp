#include "mud/html/char_data.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

char_data::char_data() : node(node::type_t::CHAR_DATA) {}

char_data::char_data(const std::string& data)
  : node(node::type_t::CHAR_DATA), _text(data)
{}

char_data::~char_data() {}

char_data::char_data(const char_data& rhs) : node(node::type_t::CHAR_DATA)
{
    (void)operator=(rhs);
}

char_data::char_data(char_data&& rhs) : node(node::type_t::CHAR_DATA)
{
    *this = std::move(rhs);
}

char_data&
char_data::operator=(const char_data& rhs)
{
    if (&rhs != this) {
        _text = rhs._text;
    }
    return *this;
}

char_data&
char_data::operator=(char_data&& rhs)
{
    _text = std::move(rhs._text);
    return *this;
}

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
END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
