#ifndef _MUDLIB_XML_ELEMENT_H_
#define _MUDLIB_XML_ELEMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML element.
 */
class MUDLIB_XML_API element : public node
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
    const std::vector<mud::xml::attribute>& attributes() const;
    std::vector<mud::xml::attribute>& attributes();

    /**
     * @brief Set the attributes of the element.
     * @param[in] value The attributes to set.
     */
    void attributes(const std::vector<mud::xml::attribute>& value);
    void attributes(std::vector<mud::xml::attribute>&& value);

    /**
     * @brief Return the direct nodes of the element.
     */
    const mud::core::poly_vector<mud::xml::node>& nodes() const;
    mud::core::poly_vector<mud::xml::node>& nodes();

    /**
     * @brief Set the direct nodes of the element.
     * @param[in] value The direct nodes to set.
     */
    void nodes(const mud::core::poly_vector<mud::xml::node>& value);
    void nodes(mud::core::poly_vector<mud::xml::node>&& value);

private:
    /** The name */
    std::string _name;

    /** The attributes */
    std::vector<mud::xml::attribute> _attributes;

    /** The direct nodes */
    mud::core::poly_vector<mud::xml::node> _nodes;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_ELEMENT_H_ */
