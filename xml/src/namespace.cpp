#include "mud/xml/namespace.h"

BEGIN_MUDLIB_XML_NS

/* static */ ns::ptr
ns::create()
{
    struct make_shared_enabler: public xml::ns
    {
        make_shared_enabler()
            : xml::ns()
        {}  
    };
    return std::make_shared<make_shared_enabler>();
}

ns::ns()
    : _resolved(false)
{
}

const mud::core::uri&
ns::uri() const
{
    return _uri;
}

void
ns::uri(const mud::core::uri& value)
{
    _uri = value;
}

void
ns::uri(mud::core::uri&& value)
{
    _uri = std::move(value);
}

const std::string&
ns::prefix() const
{
    return _prefix;
}

void
ns::prefix(const std::string& value)
{
    _prefix = value;
}

void
ns::prefix(std::string&& value)
{
    _prefix = std::move(value);
}

bool
ns::resolved() const
{
    return _resolved;
}

void
ns::resolved(bool value)
{
    _resolved = value;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
