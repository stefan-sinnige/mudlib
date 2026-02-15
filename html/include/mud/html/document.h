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

#ifndef _MUDLIB_HTML_DOCUMENT_H_
#define _MUDLIB_HTML_DOCUMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/html/element.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of an HTML document.
 */
class MUDLIB_HTML_API document: public node
{
public:
    /**
     * @brief Create an empty document.
     */
    document();

    /**
     * @brief Copy a document.
     * @param[in] rhs The document to copy from.
     */
    document(const document& rhs);

    /**
     * @brief Copy a document through assignment.
     * @param[in] rhs The document to copy from.
     * @return A reference to this document.
     */
    document& operator=(const document& rhs);

    /**
     * @brief Move a document.
     * @param[in] rhs The document to move from. After moving, it will
     * resemble an empty document.
     */
    document(document&& rhs);

    /**
     * @brief Move a document through assignment.
     * @param[in] rhs The document to move from. After moving, it will
     * resemble an empty document.
     * @return A reference to this document.
     */
    document& operator=(document&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~document();

    /**
     * @brief Return the root element.
     @ @throws mud::html::not_found when there is no root element defined.
     */
    element& root();
    const element& root() const;

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
    /** The direct nodes (one of them should be the 'root' element. */
    mud::core::poly_vector<mud::html::node> _nodes;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_DOCUMENT_H_ */
