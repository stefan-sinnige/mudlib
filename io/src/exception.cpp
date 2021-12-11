#include "mud/io/exception.h"
#include "mud/io/ip.h"

BEGIN_MUDLIB_IO_NS

/* ==========================================================================
 * exception
 * ========================================================================== */

exception::exception(const std::string& what) : _what(what) {}

exception::exception(const exception& rhs) : _what(rhs._what) {}

exception::~exception() {}

exception&
exception::operator=(const exception& rhs)
{
    if (this != &rhs) {
        _what = rhs._what;
    }
    return *this;
}

const char*
exception::what() const throw()
{
    return _what.c_str();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
