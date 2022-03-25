#include "mud/html/raw_data.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

raw_data::raw_data() : node(node::type_t::RAW_DATA) {}

raw_data::raw_data(const std::string& data)
  : node(node::type_t::RAW_DATA), _text(data)
{}

raw_data::~raw_data() {}

raw_data::raw_data(const raw_data& rhs) : node(node::type_t::RAW_DATA)
{
    (void)operator=(rhs);
}

raw_data::raw_data(raw_data&& rhs) : node(node::type_t::RAW_DATA)
{
    *this = std::move(rhs);
}

raw_data&
raw_data::operator=(const raw_data& rhs)
{
    if (&rhs != this) {
        _text = rhs._text;
    }
    return *this;
}

raw_data&
raw_data::operator=(raw_data&& rhs)
{
    _text = std::move(rhs._text);
    return *this;
}

const std::string&
raw_data::text() const
{
    return _text;
}

void
raw_data::text(const std::string& value)
{
    _text = value;
}

void
raw_data::text(std::string&& value)
{
    _text = std::move(value);
}
END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
