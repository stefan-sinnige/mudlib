#include "mud/xml/declaration.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ declaration::ptr
declaration::create()
{
    struct make_shared_enabler: public xml::declaration
    {
        make_shared_enabler()
            : xml::declaration()
        {}  
    };
    return std::make_shared<make_shared_enabler>();
}

declaration::declaration()
  : node(node::type_t::DECL), _version("1.0"), _standalone(false)
{}

declaration::~declaration() {}

declaration::declaration(declaration&& rhs) : node(node::type_t::DECL)
{
    *this = std::move(rhs);
}

declaration&
declaration::operator=(declaration&& rhs)
{
    _version = std::move(rhs._version);
    _encoding = std::move(rhs._encoding);
    _standalone = std::move(rhs._standalone);
    return *this;
}

const std::string&
declaration::version() const
{
    return _version;
}

void
declaration::version(const std::string& value)
{
    _version = value;
}

void
declaration::version(std::string&& value)
{
    _version = std::move(value);
}

const std::string&
declaration::encoding() const
{
    return _encoding;
}

void
declaration::encoding(const std::string& value)
{
    _encoding = value;
}

void
declaration::encoding(std::string&& value)
{
    _encoding = std::move(value);
}

bool
declaration::standalone() const
{
    return _standalone;
}

void
declaration::standalone(bool value)
{
    _standalone = value;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
