#include "mud/xml/attribute.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

attribute::attribute() {}

attribute::attribute(const std::string& name, const std::string& value)
  : _name(name), _value(value)
{}

attribute::~attribute() {}

attribute::attribute(const attribute& rhs)
{
    (void)operator=(rhs);
}

attribute&
attribute::operator=(const attribute& rhs)
{
    if (&rhs != this) {
        _name = rhs._name;
        _value = rhs._value;
    }
    return *this;
}

attribute::attribute(attribute&& rhs)
{
    *this = std::move(rhs);
}

attribute&
attribute::operator=(attribute&& rhs)
{
    _name = std::move(rhs._name);
    _value = std::move(rhs._value);
    return *this;
}

const std::string&
attribute::name() const
{
    return _name;
}

void
attribute::name(const std::string& value)
{
    _name = value;
}

void
attribute::name(std::string&& value)
{
    _name = std::move(value);
}

const std::string&
attribute::value() const
{
    return _value;
}

void
attribute::value(const std::string& value)
{
    _value = value;
}

void
attribute::value(std::string&& value)
{
    _value = std::move(value);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
