#include "mud/http/request.h"

BEGIN_MUDLIB_HTTP_NS

request::request() : message(message::type::REQUEST) {}

void
request::uri(const mud::core::uri& value)
{
    _uri = http::uri(value);
}

void
request::uri(const std::string& value)
{
    uri(mud::core::uri(value));
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
