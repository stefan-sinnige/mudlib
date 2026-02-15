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
