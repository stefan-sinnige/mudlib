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
