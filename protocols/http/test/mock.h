#ifndef _MUDLIB_HTTP_MOCK_H_
#define _MUDLIB_HTTP_MOCK_H_

#include "mud/http/client.h"
#include "mud/http/server.h"
#include "mud/io/tcp.h"
#include "mud/test.h"

namespace mock {

/**
 * Mock HTTP server
 */
class server: public mud::http::server
{
public:
    /*
     * Constructor.
     */
    server();

    /*
     * Specify the response to reply with.
     */
    void response(const mud::http::response& resp);

protected:
    /*
     * Handle incoming request.
     */
    void on_request(
            const mud::http::request& req,
            mud::http::response& resp) override;

private:
    /* The response to reply with */
    mud::http::response _resp;
};

/**
 * Mock HTTP client
 */
class client: public mud::http::client
{
public:
    /*
     * Constructor.
     */
    client();
};

}; // namespace mock

/* clang-format on */

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_MOCK_H_ */
