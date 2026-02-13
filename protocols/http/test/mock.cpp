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

mud::http::response
server::request(const mud::http::request& req)
{
    return _resp;
}

/* ======================================================================
 * Client
 * ====================================================================== */

client::client()
{
}

} // namespace mock

