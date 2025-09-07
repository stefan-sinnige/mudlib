#include <map>
#include "remediator.h"

BEGIN_MUDLIB_HTTP_NS

/* =========================================================================
 * HTTP/1.0
 * ========================================================================= */

void
http_10_remediator::remediate(mud::http::request& req) const
{
}

void
http_10_remediator::remediate(
        const mud::http::request& req,
        mud::http::response& resp) const
{
};

/* =========================================================================
 * HTTP/1.1
 * ========================================================================= */

void
http_11_remediator::remediate(mud::http::request& req) const
{
    // The 'Host' header field is required. If not defined, extract it from
    // the URI.
    if (req.version() == mud::http::version_e::HTTP11) {
        if (!req.exists<mud::http::host>()) {
            req.field<mud::http::host>(req.uri().value().host());
        }
    }
}

void
http_11_remediator::remediate(
        const mud::http::request& req,
        mud::http::response& resp) const
{
    // Set the 'Connection' field:
    //   * Use the response setting if it has been defined
    //   * Use the request setting if it has been defined
    //   * Use keep-alive if none is defined in the response or request
    auto conn = mud::http::connection_e::KeepAlive;
    if (resp.exists<mud::http::connection>()) {
        conn = resp.field<mud::http::connection>();
    }
    else {
        if (req.exists<mud::http::connection>()) {
            conn = req.field<mud::http::connection>();
            resp.field<mud::http::connection>(conn);
        }
        else {
            resp.field<mud::http::connection>(conn);
        }
    }

    // Set the 'Content-Length' if none supplied
    if (!resp.exists<mud::http::content_length>()) {
        resp.field<mud::http::content_length>(
            resp.entity_body().value().size());
    }
    std::cout << resp << std::endl;
};

/* =========================================================================
 * Remediator selector
 * ========================================================================= */

const base_http_remediator&
remediator(mud::http::version_e version)
{
    static no_remediator s_no_remediator;
    static http_10_remediator s_http10_remediator;
    static http_11_remediator s_http11_remediator;

    switch (version) {
        case mud::http::version_e::HTTP10:
            return s_http10_remediator;
        case mud::http::version_e::HTTP11:
            return s_http11_remediator;
        default:
            return s_no_remediator;
    };
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
