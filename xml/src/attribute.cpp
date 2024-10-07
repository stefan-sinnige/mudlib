#include "mud/xml/attribute.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ attribute::ptr
attribute::create()
{
    return std::shared_ptr<xml::attribute>(new attribute());
}

attribute::attribute(): node(node::type_t::ATTRIBUTE) {}

attribute::~attribute() {}

attribute::attribute(attribute&& rhs)
  : node(node::type_t::ATTRIBUTE)
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

std::size_t
attribute_hash::operator()(
    const mud::xml::attribute::ptr& key) const
{
    if (!key) {
        // Non-existent attribute.
        return 0;
    }
    else {
        // Hash to the attribute name.
        return std::hash<std::string>()(key->name());
    }
}
 
std::size_t
attribute_equal_to::operator()(
    const mud::xml::attribute::ptr& key1,
    const mud::xml::attribute::ptr& key2) const
{
    if (key1 == key2) {
        // Same underlying attribute object.
        return true;
    }
    else {
        if (!key2 || !key2) {
            // One of the object is non-existent.
            return false;
        }
        // Equality on the attribute name
        return std::equal_to<std::string>()(key1->name(), key2->name());
    }
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
