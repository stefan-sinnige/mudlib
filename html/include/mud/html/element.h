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

#ifndef _MUDLIB_HTML_ELEMENT_H_
#define _MUDLIB_HTML_ELEMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of an HTML element.
 */
class MUDLIB_HTML_API element : public node
{
public:
    /**
     * @brief Create an empty element without a name.
     */
    element();

    /**
     * @brief Create an empty element.
     * @param[in] name The tag name of the element.
     */
    element(const std::string& name);

    /**
     * @brief Copy a element.
     * @param[in] rhs The element to copy from.
     */
    element(const element& rhs);

    /**
     * @brief Copy a element through assignment.
     * @param[in] rhs The element to copy from.
     * @return A reference to this element.
     */
    element& operator=(const element& rhs);

    /**
     * @brief Move a element.
     * @param[in] rhs The element to move from. After moving, it will
     * resemble an empty element.
     */
    element(element&& rhs);

    /**
     * @brief Move a element through assignment.
     * @param[in] rhs The element to move from. After moving, it will
     * resemble an empty element.
     * @return A reference to this element.
     */
    element& operator=(element&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~element();

    /**
     * @brief Return the name of the element.
     */
    const std::string& name() const;

    /**
     * @brief Set the name of the element.
     * @param[in] value The value to set.
     */
    void name(const std::string& value);
    void name(std::string&& value);

    /**
     * @brief Return the attributes of the element.
     */
    const std::vector<mud::html::attribute>& attributes() const;
    std::vector<mud::html::attribute>& attributes();

    /**
     * @brief Set the attributes of the element.
     * @param[in] value The attributes to set.
     */
    void attributes(const std::vector<mud::html::attribute>& value);
    void attributes(std::vector<mud::html::attribute>&& value);

    /**
     * @brief Return the direct nodes of the element.
     */
    const mud::core::poly_vector<mud::html::node>& nodes() const;
    mud::core::poly_vector<mud::html::node>& nodes();

    /**
     * @brief Set the direct nodes of the element.
     * @param[in] value The direct nodes to set.
     */
    void nodes(const mud::core::poly_vector<mud::html::node>& value);
    void nodes(mud::core::poly_vector<mud::html::node>&& value);

private:
    /** The name */
    std::string _name;

    /** The attributes */
    std::vector<mud::html::attribute> _attributes;

    /** The direct nodes */
    mud::core::poly_vector<mud::html::node> _nodes;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_ELEMENT_H_ */
