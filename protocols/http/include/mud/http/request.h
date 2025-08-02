#ifndef _MUDLIB_HTTP_REQUEST_H_
#define _MUDLIB_HTTP_REQUEST_H_

#include <strings.h>
#include <mud/http/message.h>
#include <mud/http/ns.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of an HTTP request message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API request : public message
{
public:
    /**
     * Construct an empty HTTP request message.
     */
    request();

    /**
     *  Destructor.
     */
    virtual ~request() = default;

    /**
     * Set the method.
     * @param[in] value  The method value to set.
     */
    void method(http::method_e value) { _method = http::method(value); }
    void method(const std::string& value) { _method = http::method(value); }
    void method(const http::method& value) { _method = value; }

    /**
     * Get the method.
     */
    const http::method& method() const { return _method; }
    http::method& method() { return _method; }

    /**
     * Set the URI.
     * @param[in] value  The URI value to set.
     */
    void uri(const mud::core::uri& value);
    void uri(const std::string& value);

    /**
     * Get the URI.
     */
    const http::uri& uri() const { return _uri; }
    http::uri& uri() { return _uri; }

private:
    /**
     * The request line
     */
    http::method _method;
    http::uri _uri;
};

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_REQUEST_H_ */
