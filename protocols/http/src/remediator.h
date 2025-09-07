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
