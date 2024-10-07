#ifndef _MUDLIB_XML_ATTRIBUTE_H_
#define _MUDLIB_XML_ATTRIBUTE_H_

#include <mud/xml/ns.h>
#include <mud/xml/node.h>
#include <memory>
#include <string>
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
 *  * An owner element that this attribute is a member of.
 */
class MUDLIB_XML_API attribute: public node
{
public:
    /**
     * @brief Type definition of an @c attribute pointer.
     */
    typedef std::shared_ptr<mud::xml::attribute> ptr;

    /**
     * @brief Create a new @c attribute instance.
     */
    static ptr create();

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
    /**
     * @brief Create an empty attribute.
     */
    attribute();

    /** The name */
    std::string _name;

    /** The value */
    std::string _value;
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
