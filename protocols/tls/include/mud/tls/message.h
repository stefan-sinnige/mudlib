#ifndef _MUDLIB_TLS_MESSAGE_H_
#define _MUDLIB_TLS_MESSAGE_H_

#include <iostream>
#include <mud/tls/ns.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The definition of an TLS message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_TLS_API message
{
public:
    /**
     * The TLS message type
     */
    enum class type
    {
    };

    /**
     * Copy constructor.
     */
    message(const message& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~message() = default;

    /**
     * Assign an TLS mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    message& operator=(const message& rhs) = default;

    /**
     * The message type.
     */
    message::type type() const;

protected:
    /**
     * Construction of a message of a certain type.
     */
    message(enum message::type);

private:
    /**
     * Data members
     */
    enum message::type _type;
};

/** Read an TLS message from an input stream. */
std::istream&
operator>>(std::istream&, message&);

/** Write an TLS message to an output stream. */
std::ostream&
operator<<(std::ostream&, const message&);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_MESSAGE_H_ */
