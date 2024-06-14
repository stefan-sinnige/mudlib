#include "mud/http/request.h"

BEGIN_MUDLIB_HTTP_NS

request::request() : message(message::type::REQUEST) {}

void
request::uri(const mud::core::uri& value)
{
    // Ensure to create a URI with a relative path
    mud::core::uri relative_uri = value;
    relative_uri.path(std::filesystem::path("/") / value.path());
    _uri = http::uri(relative_uri);
}

void
request::uri(const std::string& value)
{
    uri(mud::core::uri(value));
}

void
request::normalise()
{
    // HTTP 1.1 normalisation:
    //    * Host is required, set to URI host if not found
    if (version() == mud::http::version_e::HTTP11) {
        if (!exists<mud::http::host>()) {
            field<mud::http::host>(_uri.value().host());
        }
    }
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
