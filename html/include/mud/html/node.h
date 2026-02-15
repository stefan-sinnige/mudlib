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

#ifndef _MUDLIB_HTML_NODE_H_
#define _MUDLIB_HTML_NODE_H_

#include <mud/html/attribute.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Abstract representation of any node of an HTML tree. This can be
 * an element, text, etc. In general, any HTML content is an HTML node.
 */
class MUDLIB_HTML_API node
{
public:
    /**
     * The type of nodes.
     */
    enum class type_t
    {
        DOCUMENT,
        CDATA_SECTION,
        CHAR_DATA,
        RAW_DATA,
        COMMENT,
        DECL,
        ELEMENT,
        PI
    };

    /**
     * @brief Copy a node.
     * @param[in] rhs The node to copy from.
     */
    node(const node& rhs) { _type = rhs._type; }

    /**
     * @brief Copy a node through assignment.
     * @param[in] rhs The node to copy from.
     * @return A reference to this node.
     */
    node& operator=(const node& rhs)
    {
        _type = rhs._type;
        return *this;
    }

    /**
     * @brief Move a node.
     * @param[in] rhs The node to move from.
     */
    node(node&& rhs) { _type = rhs._type; }

    /**
     * @brief Copy a node through assignment.
     * @param[in] rhs The node to copy from.
     * @return A reference to this node.
     */
    node& operator=(node&& rhs)
    {
        _type = rhs._type;
        return *this;
    }

    /**
     * @brief Destruct a node.
     */
    virtual ~node() = default;

    /**
     * Return the type of this node.
     */
    type_t type() const { return _type; }

protected:
    /**
     * @brief Construct a node of a certain type.
     * @param[in] type The type to assign to the node.
     */
    node(type_t type) : _type(type) {}

private:
    /** The node type. */
    type_t _type;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_NODE_H_ */
