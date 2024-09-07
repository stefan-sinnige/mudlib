#include "mud/tls/message.h"
#include <stdexcept>

BEGIN_MUDLIB_TLS_NS

message::message(enum message::type type)
  : _type(type)
{
}

enum message::type
message::type() const
{
    return _type;
}

std::ostream&
operator<<(std::ostream& ostr, const message& msg)
{
    return ostr;
}

std::istream&
operator>>(std::istream& istr, message& msg)
{
    return istr;
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
