#ifndef _MUDLIB_TEST_EXCEPTION_H_
#define _MUDLIB_TEST_EXCEPTION_H_

#include <exception>
#include <mud/test/ns.h>
#include <string>

BEGIN_MUDLIB_TEST_NS

/**
 * @brief Reporting a test that failed an asserting.
 */
class assertion_failed : public std::exception
{
public:
    /**
     * @brief Default constructor.
     * @param what [in] Detailed description of the failed assertion.
     */
    assertion_failed(const std::string& what);

    /**
     * @brief Destructor.
     */
    virtual ~assertion_failed();

    /**
     * @brief Detailed description of the failed assertion.
     * @return The description.
     */
    virtual const char* what() const noexcept;

private:
    std::string _what;
};

inline assertion_failed::assertion_failed(const std::string& what) : _what(what)
{}

inline assertion_failed::~assertion_failed() {}

inline const char*
assertion_failed::what() const noexcept
{
    return _what.c_str();
}

/**
 * @brief Reporting a test that references an unknown gherkin element.
 */
class not_specified : public std::exception
{
public:
    /**
     * @brief Default constructor.
     * @param what [in] Detailed description of the failed assertion.
     */
    not_specified(const std::string& what);

    /**
     * @brief Destructor.
     */
    virtual ~not_specified();

    /**
     * @brief Detailed description of the failed assertion.
     * @return The description.
     */
    virtual const char* what() const noexcept;

private:
    std::string _what;
};

inline not_specified::not_specified(const std::string& what) : _what(what) {}

inline not_specified::~not_specified() {}

inline const char*
not_specified::what() const noexcept
{
    return _what.c_str();
}

END_MUDLIB_TEST_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TEST_EXCEPTION_H_ */
