#ifndef _MUDLIB_XML_ELEMENT_H_
#define _MUDLIB_XML_ELEMENT_H_

#include <mud/xml/attribute.h>
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
     * @brief Create a new @c element instance.
     */
    static ptr create();

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
     * @brief Return the parent of the element.
     *
     * @details
     * The parent of the element is either another @c element, or a @c document
     * if the parent is the @em root element.
     */
    mud::xml::node::ptr parent() const;

private:
    /**
     * @brief Create an empty element without a name.
     */
    element();

    /** The name */
    std::string _name;

    /** The attributes */
    mud::xml::attribute_set _attributes;

    /** The direct nodes */
    mud::xml::node_seq _children;

    /** The parent. */
    mud::xml::node::ptr _parent;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_ELEMENT_H_ */
