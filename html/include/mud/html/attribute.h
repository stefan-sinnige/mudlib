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

#ifndef _MUDLIB_HTML_ATTRIBUTE_H_
#define _MUDLIB_HTML_ATTRIBUTE_H_

#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of an HTML attribute.
 */
class MUDLIB_HTML_API attribute
{
public:
    /**
     * @brief Create an empty attribute.
     */
    attribute();

    /**
     * @brief Create an attribute.
     * @param[in] name The atribute name.
     * @param[in] value The atribute value.
     */
    attribute(const std::string& name, const std::string& value);

    /**
     * @brief Copy a attribute.
     * @param[in] rhs The attribute to copy from.
     */
    attribute(const attribute& rhs);

    /**
     * @brief Copy a attribute through assignment.
     * @param[in] rhs The attribute to copy from.
     * @return A reference to this attribute.
     */
    attribute& operator=(const attribute& rhs);

    /**
     * @brief Move a attribute.
     * @param[in] rhs The attribute to move from. After moving, it will
     * resemble an empty attribute.
     */
    attribute(attribute&& rhs);

    /**
     * @brief Move a attribute through assignment.
     * @param[in] rhs The attribute to move from. After moving, it will
     * resemble an empty attribute.
     * @return A reference to this attribute.
     */
    attribute& operator=(attribute&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~attribute();

    /**
     * @brief Return the name of the attribute.
     */
    const std::string& name() const;

    /**
     * @brief Set the name of the attribute.
     * @param[in] value The value to set.
     */
    void name(const std::string& value);
    void name(std::string&& value);

    /**
     * @brief Return the value of the attribute.
     */
    const std::string& value() const;

    /**
     * @brief Set the value of the attribute.
     * @param[in] value The value to set.
     */
    void value(const std::string& value);
    void value(std::string&& value);

private:
    /** The name */
    std::string _name;

    /** The value */
    std::string _value;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_ATTRIBUTE_H_ */
