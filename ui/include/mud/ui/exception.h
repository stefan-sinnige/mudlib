#ifndef _MUDLIB_UI_EXCEPTION_H_
#define _MUDLIB_UI_EXCEPTION_H_

#include <exception>
#include <mud/ui/ns.h>
#include <string>

BEGIN_MUDLIB_UI_NS

/**
 * @brief Reporting exceptions related to the underlying UI mechanism.
 */
class exception : public std::exception
{
public:
    /**
     * @brief Constructs an exception with the given message.
     *
     * @param what [in] Explanatory message.
     */
    exception(const std::string& what);

    /**
     * @brief Destructor.
     */
    virtual ~exception();

    /**
     * @brief The explanatory message.
     */
    const char* what() const noexcept override;

private:
    /** The explanatory message. */
    std::string _what;
};

inline exception::exception(const std::string& what) : _what(what) {}

inline exception::~exception() {}

inline const char*
exception::what() const noexcept
{
    return _what.c_str();
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_EXCEPTION_H_ */
