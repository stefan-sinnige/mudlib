#ifndef _MUDLIB_IO_EXCEPTION_H_
#define _MUDLIB_IO_EXCEPTION_H_

#include <exception>
#include <mud/io/ns.h>
#include <string>

BEGIN_MUDLIB_IO_NS

/**
 * @brief Reporting I/O related exceptions. This is a generic exception that
 * more detailed exceptions are derived from.
 */
class MUDLIB_IO_API exception : public std::exception
{
public:
    /**
     * @brief Default constructor.
     * @param what [in] Explanatory information.
     */
    exception(const std::string& what);

    /**
     * @brief Copy constructor.
     */
    exception(const exception& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~exception();

    /**
     * @brief Assignment operator.
     */
    exception& operator=(const exception& rhs);

    /**
     * Return the detailed description.
     * @return A pointer to a null-terminated string with explanatory
     * information.
     */
    virtual const char* what() const throw() override;

private:
    std::string _what; /**< Explanatory string. */
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_PIPE_H_ */
