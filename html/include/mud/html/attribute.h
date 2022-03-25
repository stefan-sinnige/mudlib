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
