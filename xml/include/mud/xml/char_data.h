#ifndef _MUDLIB_XML_CHARDATA_H_
#define _MUDLIB_XML_CHARDATA_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML character data
 */
class MUDLIB_XML_API char_data : public node
{
public:
    /**
     * @brief Create an empty character data..
     */
    char_data();

    /**
     * @brief Create character data..
     * @param[in] data The character data.
     */
    char_data(const std::string& data);

    /**
     * @brief Copy a character data.
     * @param[in] rhs The character data to copy from.
     */
    char_data(const char_data& rhs);

    /**
     * @brief Copy a character data through assignment.
     * @param[in] rhs The character data to copy from.
     * @return A reference to this character data.
     */
    char_data& operator=(const char_data& rhs);

    /**
     * @brief Move a character data.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     */
    char_data(char_data&& rhs);

    /**
     * @brief Move a character data through assignment.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     * @return A reference to this character data.
     */
    char_data& operator=(char_data&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~char_data();

    /**
     * @brief Return the contents of the character data.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the character data.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /** The character data contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CHARDATA_H_ */
