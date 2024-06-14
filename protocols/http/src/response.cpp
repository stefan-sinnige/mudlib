#include "mud/http/response.h"

BEGIN_MUDLIB_HTTP_NS

response::response() : message(message::type::RESPONSE) {}

void
response::normalise()
{
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
