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

#ifndef _MUDLIB_XML_ATTRIBUTE_H_
#define _MUDLIB_XML_ATTRIBUTE_H_

#include <mud/core/uri.h>
#include <mud/xml/ns.h>
#include <mud/xml/namespace.h>
#include <mud/xml/node.h>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_set>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML attribute.
 *
 * @details
 * As defined in the XML Information Set (Second Edition), an XML attribute has
 * the following properties:
 *  * A local name
 *  * A normalised value
 *  * A parent element that this attribute is a member of.
 */
class MUDLIB_XML_API attribute: public node
{
public:
    /**
     * @brief Type definition of an @c attribute pointer.
     */
    typedef std::shared_ptr<mud::xml::attribute> ptr;

    /**
     * @brief @brief Create an XML attribute node with a qualified name.
     *
     * @details
     * A qualified name is a name of the form
     * @verbatim
     *    [ prefix ':' ] local-name
     * @endverbatim
     * If the @c prefix is omitted, the @c local-name is said to be part of the
     * default namespace. Otherwise, the @c prefix references the namespace
     * prefix that the attribute is defined in.
     *
     * A special namespace definition attribute can be defined through this
     * method by specifying the name as
     * @verbatim
     *    'xmlns' [ ':' prefix ]
     * @endverbatim
     * This defines a resolved namespace whose URI is provided by the attribute
     * value. If the prefix is omitted, then the attribute defines a default
     * namespace.
     *
     * @param qname The qualified name.
     */
    static ptr create(const std::string& qname);

    /**
     * @brief Create an XML attribute node with a local-name that is part of
     * a defined namespace. The local-name should @c em not contain a prefix
     * specification as that is provided by the resolved namespace definition.
     *
     * @param local_name The local-name.
     * @param uri The namespace URI.
     */
    static ptr create(const std::string& local_name, const mud::core::uri& uri);

    /**
     * @brief Move a attribute.
     * @param[in] rhs The attribute to move from. After moving, it will
     * resemble an empty attribute.
     */
    attribute(attribute&& rhs) = default;

    /**
     * @brief Move a attribute through assignment.
     * @param[in] rhs The attribute to move from. After moving, it will
     * resemble an empty attribute.
     * @return A reference to this attribute.
     */
    attribute& operator=(attribute&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~attribute() = default;

    /**
     * @brief Return the qualified name of the attribute.
     *
     * @details
     * If the attribute has a non-default namespace, the qualified name is
     * returned consisting of the namespace prefix and the local-name. If the
     * attribute is part of the default namespace, only the local-name is
     * returned.
     *
     * If the non-default namespace cannot be resolved, the local-name is
     * returned.
     */
    const std::string& name() const;

    /**
     * @brief Return the local-name of the attribute.
     *
     * @details
     * The local-name is the part of the attribute name without any prefix.
     */
    const std::string& local_name() const;

    /**
     * @brief Return the prefix of the attribute.
     *
     * @details
     * The prefix of an attribute is the namespace prefix of the attribute's
     * non-default namespace. If the attribute is part of the default namespace,
     * an empty prefix is returned.
     */
    const std::string& prefix() const;

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

    /**
     * @brief Set the value of the attribute.
     * @param[in] value The value to set.
     *
     * @details
     * The value to be set must have a definition for an output streaming
     * operator.
     */
    template<typename T>
    void value(const T& value) {
        std::stringstream sstr;
        sstr << value;
        this->value(sstr.str());
    }

    /**
     * @brief Return the namespace definition associated to this attribute.
     *
     * @details
     * The namespace definition is a resolved one if the attribute's prefix
     * is not defined, or if the prefix matches a resolved namespace of its
     * containing @c element node or of its closest ancestor.
     */
    const mud::xml::ns::ptr& ns() const;

    /**
     * @brief Return the resolved namespace definition associated to this
     * attribute.
     *
     * @details
     * The resolved namespace is the namespace definition that maches the URI
     * of this attribute. If the attribute's namespace cannot be resolved by
     * containing @c element node or its ancestors, then a @c nullptr shall be
     * returned.
     */
    mud::xml::ns::ptr resolved_ns() const;

private:
    /**
     * @brief Create an XML attribute node with a qualified name.
     * @param qname The qualified name.
     */
    attribute(const std::string& qname);

    /**
     * @brief Create an XML attribute node with a local-name that is part of
     * a defined namespace. 
     * @param qname The qualified name.
     * @param uri The namespace URI.
     */
    attribute(const std::string& local_name, const mud::core::uri& uri);

    /** The qualified name */
    std::string _name;

    /** The local name */
    std::string _local_name;

    /** The value */
    std::string _value;

    /** The namespace definition if the attribute is a namespace. */
    mud::xml::ns::ptr _ns;
};

/**
 * Hash function to be used in the attribute set. This is not based on the
 * shared pointer, but rather on the name of the attribute it refers to.
 */
struct attribute_hash
{
    std::size_t
    operator()(const mud::xml::attribute::ptr& key) const;
};
 
/**
 * Equal function to be used in the attribute set. This is not based on the
 * shared pointer, but rather on the name of the attribute it refers to.
 */
struct attribute_equal_to
{
    std::size_t
    operator()(const mud::xml::attribute::ptr& key1,
               const mud::xml::attribute::ptr& key2) const;
};
 
/**
 * @brief The set of attribute instances.
 *
 * @details
 * The set of attribute instances is an unordered container of pointers to
 * attributes. This concept would allow us to easily and efficiently share
 * attributes (for example in an XML document and an XPath result query).
 */
class MUDLIB_XML_API attribute_set:
    public std::unordered_set<
        mud::xml::attribute::ptr,
        mud::xml::attribute_hash,
        mud::xml::attribute_equal_to>
{
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_ATTRIBUTE_H_ */
