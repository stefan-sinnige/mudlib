#ifndef _MUDLIB_HTML_EXCEPTION_H_
#define _MUDLIB_HTML_EXCEPTION_H_

#include <exception>
#include <mud/html/ns.h>
#include <string>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Base class for any HTML related exception.
 */
class MUDLIB_HTML_API exception : public std::exception
{
public:
    /**
     * @brief Default constructor.
     */
    exception() = default;

    /**
     * @brief Construct an exception with explanatory information.
     */
    exception(const std::string& what) : _what(what){};

    /**
     * @brief Copy constructor.
     */
    exception(const exception& rhs) : _what(rhs._what){};

    /**
     * @brief Assignment an exception from another exception.
     * @param[in] The exception to assign from.
     * @return Reference to this object.
     */
    exception& operator=(const exception& rhs)
    {
        if (&rhs != this) {
            _what = rhs._what;
        }
        return *this;
    }

    /**
     * @brief Destructor.
     */
    virtual ~exception() = default;

    /**
     * Return the detailed explanatory information.
     * @return A pointer to a null-terminated string with explanatory
     * information.
     */
    virtual const char* what() const throw() override { return _what.c_str(); }

private:
    std::string _what; /**< Explanatory string */
};

/**
 * @brief Exception raised when an HTML entity is not found.
 */
class MUDLIB_HTML_API not_found : public exception
{
public:
    /**
     * @brief Default constructor.
     */
    not_found() = default;

    /**
     * @brief Construct an exception with explanatory information.
     */
    not_found(const std::string& what) : exception(what){};
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_EXCEPTION_H_ */
