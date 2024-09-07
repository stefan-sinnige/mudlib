#include "mock.h"

namespace mock {

/* ======================================================================
 * Server
 * ====================================================================== */

server::server()
{
}

void
server::response(const mud::http::response& resp)
{
    _resp = resp;
}

void
server::on_request( const mud::http::request& req, mud::http::response& resp)
{
    resp = _resp;
}

/* ======================================================================
 * Client
 * ====================================================================== */

client::client()
{
}

} // namespace mock

