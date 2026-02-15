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

#ifndef _MUDLIB_XML_ELEMENT_H_
#define _MUDLIB_XML_ELEMENT_H_

#include <mud/xml/attribute.h>
#include <mud/xml/namespace.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML element.
 *
 * @details
 * As defined in the XML Information Set (Second Edition), an XML element has
 * the following properties:
 *  * An unordered set of attributes
 *  * An ordered sequence of children (an XML node other than an attribute)
 *  * A parent. If the parent is an XML document node, then the element is
 *    also known as the @em root element.
 */
class MUDLIB_XML_API element : public node
{
public:
    /**
     * @brief Type definition of an @c element pointer.
     */
    typedef std::shared_ptr<mud::xml::element> ptr;

    /**
     * @brief Create a new @c element instance with a qualified name.
     *
     * @details
     * A qualified name is a name of the form
     * @verbatim
     *    [ prefix ':' ] local-name
     * @endverbatim
     * If the @c prefix is omitted, the @c local-name is said to be part of the
     * default namespace. Otherwise, the @c prefix references the namespace
     * prefix that the element is defined in.
     */
    static ptr create(const std::string& qname);

    /**
     * @brief Create an XML element node with a local-name that is part of
     * a defined namespace. The local-name should @c em not contain a prefix
     * specification as that is provided by the resolved namespace definition.
     *
     * @param local_name The local-name.
     * @param uri The namespace URI.
     */
    static ptr create(const std::string& local_name, const mud::core::uri& uri);

    /**
     * @brief Move a element.
     * @param[in] rhs The element to move from. After moving, it will
     * resemble an empty element.
     */
    element(element&& rhs) = default;

    /**
     * @brief Move a element through assignment.
     * @param[in] rhs The element to move from. After moving, it will
     * resemble an empty element.
     * @return A reference to this element.
     */
    element& operator=(element&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~element() = default;

    /**
     * @brief Return the qualified name of the element.
     *
     * @details
     * If the element has a non-default namespace, the qualified name is
     * returned consisting of the namespace prefix and the local-name. If the
     * element is part of the default namespace, only the local-name is
     * returned.
     *
     * If the non-default namespace cannot be resolved, the local-name is
     * returned.
     */
    const std::string& name() const;

    /**
     * @brief Return the local-name of the element.
     *
     * @details
     * The local-name is the part of the element name without any prefix.
     */
    const std::string& local_name() const;

    /**
     * @brief Return the prefix of the element.
     *
     * @details
     * The prefix of an element is the namespace prefix of the element's
     * non-default namespace. If the element is part of the default namespace,
     * an empty prefix is returned.
     */
    const std::string& prefix() const;

    /**
     * @brief Return the attributes of the element.
     */
    const mud::xml::attribute_set& attributes() const;

    /**
     * @brief Set the attributes of the element.
     * @param[in] value The attributes to set.
     */
    void attributes(const mud::xml::attribute_set& value);
    void attributes(mud::xml::attribute_set&& value);

    /**
     * @brief Add an attribute to the element.
     * @param[in] value The attribute to add.
     */
    void attribute(const mud::xml::attribute::ptr& value);

    /**
     * @brief Return the attribute that matches the @c local_name.
     * @param local_name The attribute's local name to query for.
     * @return The attribute with matching local name, or @c nullptr if there is
     * no matching attribute.
     */
    mud::xml::attribute::ptr attribute(const std::string& local_name) const;

    /**
     * @brief Return the children of the element.
     */
    virtual const mud::xml::node_seq& children() const override;

    /**
     * @brief Set the children of the element.
     * @param[in] value The children to set.
     */
    void children(const mud::xml::node_seq& value);
    void children(mud::xml::node_seq&& value);

    /**
     * @brief Add a child to the element.
     * @param[in] value The node to add.
     */
    void child(const mud::xml::node::ptr& value);

    /**
     * @brief Return all the child elements that match the @c local_name.
     * @param local_name The child element's local name to query for.
     * @return The list of all direct children with matching local name.
     */
    mud::xml::node_seq elements(const std::string& local_name) const;

    /**
     * @brief Return the namespace definition associated to this element.
     *
     * @details
     * The namespace definition is a resolved one if the element's prefix
     * is not defined, or if the prefix matches a resolved namespace of this
     * node or of its closest ancestor.
     */
    const mud::xml::ns::ptr& ns() const;

    /**
     * @brief Return the resolved namespace definition associated to this
     * element.
     *
     * @details
     * The resolved namespace is the namespace definition that maches the URI
     * of this element. If the element's namespace cannot be resolved by the
     * node itself or its ancestors, then the unresolved element's namespace
     * definition is returned.
     */
    mud::xml::ns::ptr resolved_ns() const;

    /**
     * @brief Return the resolved namespace definition associated to the
     * specified namespace URI.
     *
     * @details
     * The resolved namespace is the namespace definition that maches the
     * specified URI. If the namespace cannot be resolved by the node itself or
     * its ancestors, then a @c nullptr shall be returned.
     *
     * @param uri The namespace URI to resolve.
     */
    mud::xml::ns::ptr resolved_ns(const mud::core::uri& uri) const;

private:
    /**
     * @brief Create an XML element node with a qualified name.
     * @param qname The qualified name.
     */
    element(const std::string& qname);

    /**
     * @brief Create an XML element node with a local-name that is part of
     * a defined namespace. 
     * @param qname The qualified name.
     * @param uri The namespace URI.
     */
    element(const std::string& local_name, const mud::core::uri& uri);

    /** The qualified name */
    std::string _name;

    /** The prefix */
    std::string _prefix;

    /** The local name */
    std::string _local_name;

    /** The attributes */
    mud::xml::attribute_set _attributes;

    /** The direct nodes */
    mud::xml::node_seq _children;

    /** The resolved namespace. */
    mud::xml::ns::ptr _ns;

    /** The list of namespaces defined at this element. */
    mud::xml::ns_list _namespaces;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_ELEMENT_H_ */
