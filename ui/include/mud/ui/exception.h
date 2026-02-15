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
