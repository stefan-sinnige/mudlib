/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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
