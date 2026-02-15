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

#ifndef _MUDLIB_XML_QUERY_H_
#define _MUDLIB_XML_QUERY_H_

#include <mud/xml/ns.h>
#include <mud/xml/attribute.h>
#include <mud/xml/cdata_section.h>
#include <mud/xml/char_data.h>
#include <mud/xml/comment.h>
#include <mud/xml/declaration.h>
#include <mud/xml/document.h>
#include <mud/xml/element.h>
#include <mud/xml/exception.h>
#include <mud/xml/node.h>
#include <mud/xml/processing_instruction.h>
#include <string>
#include <variant>
#include <vector>

#include <iostream>

BEGIN_MUDLIB_XML_NS

/**
 * @brief The result of an XPath query.
 *
 * @details
 * Depending on the grammar of the @c query being executed, the result type can
 * be different. For example, the query may return a list of element object,
 * attribute objects or just a list of strings. The result class is able to
 * represent any kind of a query result.
 */
class MUDLIB_XML_API query_result
{
private:
public:
    /**
     * @brief Default constructor.
     */
    query_result();

    /**
     * @brief Return the list of nodes.
     */
    const node_seq& nodes() const;

    /**
     * @brief Return the list of string.
     */
    const std::vector<std::string>& strings() const;

    /**
     * @brief Append an item to the result.
     *
     * @details
     * Append the item to the result. If the result's underlying type has not
     * yet been set, it will be set accordingly. If the underlying type is
     * set to a different type than the item inserted, and exception will be
     * thrown.
     *
     * @param item The item to append
     */
    void append(const mud::xml::node::ptr& item);
    void append(const std::string& item);

private:
    /** The underlying type that is used. */
    enum class underlying_t {
        UNSET,
        NODE,
        STRING
    };
    underlying_t _underlying;

    /**
     * @brief Assert the underlying type.
     *
     * @details
     * Assert that the underlying type if of a specified value. Raise a @c
     * mud::xml::exception if it doesn't.
     */
    void assert_underlying(underlying_t expected) const {
         if (_underlying != expected) {
            throw mud::xml::exception("XPath result type is not as expected");
         }
    }

    /** The type of the structure holding the result. The actual result exposed
     * in the public interface is an iterator to one of the result vectors. */
    mud::xml::node_seq _nodes;
    std::vector<std::string> _strings;
};

/**
 * @brief Perform an XPath query on an XML node (usually the document).
 *
 * @details
 * XPath is an expression language that is designed to query for information
 * from an XML document. The @c query class is able to perform such an XPath
 * query on a document and return the information queried for.
 *
 * @note
 * The implementation only able to perform a limited subset of the XPath 3.0
 * syntax. The queries are limited to:
 *    * Non-Abbreviated Absolute Location Path
 *    * Only Literal Primary Expressions
 *
 * For example, consider the following XML document to be loaded in an XML
 * @c document instance.
 * @code
 *    <?xml version="1.0" encoding="utf-8"?>
 *    <catalog>
 *        <book id="1">
 *            <author>Homer</author>
 *            <title>Illiad</title>
 *        </book>
 *        <book id="2">
 *            <author>Erasmus</author>
 *            <title>On Free Will</title>
 *        </book>
 *    </catalog>
 * @endcode
 * To retrieve all the book titles:
 * @code
 *    xml::query query(doc);
 *    auto title_iter = q.evaluate("/catalog/book/title");
 * @endcode
 * The iterator will navigate to the next match when it is advanced, or it
 * shall return @c end() when there are no (more) matches.
 */
class MUDLIB_XML_API query
{
public:
    /**
     * @brief Create an empty (invalid) XPath query.
     *
     * @details
     * An empty query cannot be evaluated. An object created in this manner
     * would need to be initialised through the copy or move constructs with a
     * non-empty @c query object before being used.
     */
    query();

    /**
     * @brief Create an XPath query.
     *
     * @details
     * The format of the query is a subset of XPath and the supported format
     * can be extended in future releases. If the query encounters an error,
     * a @c mud::xml::exception shall be thrown.
     *
     * @param xpath_query The XPath query to evaluate.
     */
    query(const std::string& xpath_query);

    /**
     * @brief Copy constructor.
     * @param other The query to copy from.
     */
    query(const query& other);

    /**
     * @brief Move constructor
     * @param other The query to move from.
     */
    query(query&& other);

    /**
     * @brief Copy assignment.
     * @param other The query to copy from.
     */
    query& operator=(const query& other);

    /**
     * @brief Move assigment
     * @param other The query to move from.
     */
    query& operator=(query&& other);

    /**
     * @brief Evaluate an XPath query.
     *
     * @details
     * Evaluate the XPath query against an XML document and return the result.
     * The same query object can be used repeatedly, with different @c nodes.
     *
     * @param xml_doc The XML document to operate on.
     * @return The result of the query.
     */
    query_result evaluate(const mud::xml::document::ptr& xml_doc);

    /**
     * @brief Evaluate an XPath query.
     *
     * @details
     * Evaluate the XPath query against an XML document and return the result.
     * The same query object can be used repeatedly, with different @c nodes.
     *
     * @param xml_node The XML node to operate on.
     * @return The result of the query.
     */
    query_result evaluate(const mud::xml::node::ptr& xml_node);

private:
    /** The implementation */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_XML_QUERY_H_ */
