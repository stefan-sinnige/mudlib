#ifndef _MUDLIB_HTTP_RESPONSE_H_
#define _MUDLIB_HTTP_RESPONSE_H_

#include <mud/http/message.h>
#include <mud/http/ns.h>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of an HTTP response message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API response : public message
{
public:
    /**
     * Construct an empty HTTP response message.
     */
    response();

    /**
     *  Destructor.
     */
    virtual ~response() = default;

    /**
     * Set the status code.
     * @param[in] value  The status code value to set.
     */
    void status_code(const http::status_code& value) { _status_code = value; }

    /**
     * Get the status code.
     */
    const http::status_code& status_code() const { return _status_code; }
    http::status_code& status_code() { return _status_code; }

    /**
     * Set the reson phrase.
     * @param[in] value  The reason phrase value to set.
     */
    void reason_phrase(const http::reason_phrase& value)
    {
        _reason_phrase = value;
    }

    /**
     * Get the reason phrase.
     */
    const http::reason_phrase& reason_phrase() const { return _reason_phrase; }
    http::reason_phrase& reason_phrase() { return _reason_phrase; }

private:
    /**
     * The status line
     */
    http::status_code _status_code;
    http::reason_phrase _reason_phrase;
};

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_RESPONSE_H_ */
