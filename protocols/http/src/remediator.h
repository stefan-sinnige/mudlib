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

#ifndef _HTTP_REMEDIATOR_H_
#define _HTTP_REMEDIATOR_H_

#include "mud/http/request.h"
#include "mud/http/response.h"

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The base class for an HTTP protocol remdiator.
 *
 * @details
 * The remediator is the tool to ensure that the HTTP protocol is compliant
 * with the specification. This may mean adding, removing or altering header
 * fields.
 */
class base_http_remediator
{
public:
    /**
     * @brief Constructor.
     */
    base_http_remediator() = default;

    /**
     * @brief Destructor.
     */
    virtual ~base_http_remediator() = default;

    /**
     * @brief Remediate a request message.
     * @param req The request to remediate.
     */
    virtual void remediate(mud::http::request& req) const = 0;

    /**
     * @brief Remediate a response message.
     * @param req The request associated to the response.
     * @param resp The response to remediate.
     */
    virtual void remediate(
            const mud::http::request& req,
            mud::http::response& resp) const = 0;
};

/**
 * @brief The remediator that does not remediate anything.
 */
class no_remediator: public base_http_remediator
{
public:
    /**
     * @brief Remediate a request message.
     * @param req The request to remediate.
     */
    virtual void remediate(mud::http::request& req) const {}

    /**
     * @brief Remediate a response message.
     * @param req The request associated to the response.
     * @param resp The response to remediate.
     */
    virtual void remediate(
            const mud::http::request& req,
            mud::http::response& resp) const {}
};

/**
 * @brief The HTTP/1.0 protocol remdiator.
 *
 * @details
 * The remediator is the tool to ensure that the HTTP protocol is compliant
 * with the specification. This may mean adding, removing or altering header
 * fields.
 */
class http_10_remediator: public base_http_remediator
{
public:
    /**
     * @brief Remediate a request message.
     * @param req The request to remediate.
     */
    virtual void remediate(mud::http::request& req) const;

    /**
     * @brief Remediate a response message.
     * @param req The request associated to the response.
     * @param resp The response to remediate.
     */
    virtual void remediate(
            const mud::http::request& req,
            mud::http::response& resp) const;
};

/**
 * @brief The HTTP/1.0 protocol remdiator.
 *
 * @details
 * The remediator is the tool to ensure that the HTTP protocol is compliant
 * with the specification. This may mean adding, removing or altering header
 * fields.
 */
class http_11_remediator: public base_http_remediator
{
public:
    /**
     * @brief Remediate a request message.
     * @param req The request to remediate.
     */
    virtual void remediate(mud::http::request& req) const;

    /**
     * @brief Remediate a response message.
     * @param req The request associated to the response.
     * @param resp The response to remediate.
     */
    virtual void remediate(
            const mud::http::request& req,
            mud::http::response& resp) const;
};

/**
 * @brief Get the remediator for a specific HTTP version.
 *
 * @param version The HTTP version.
 * @return The remediator associated to the HTTP version.
 */
const base_http_remediator& remediator(mud::http::version_e version);

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _HTTP_REMEDIATOR_H_ */
