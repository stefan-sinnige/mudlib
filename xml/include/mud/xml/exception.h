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

#ifndef _MUDLIB_XML_EXCEPTION_H_
#define _MUDLIB_XML_EXCEPTION_H_

#include <exception>
#include <mud/xml/ns.h>
#include <string>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Base class for any XML related exception.
 */
class MUDLIB_XML_API exception : public std::exception
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
 * @brief Exception raised when an XML entity is not found.
 */
class MUDLIB_XML_API not_found : public exception
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

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_EXCEPTION_H_ */
