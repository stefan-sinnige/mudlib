#ifndef _MUDLIB_XML_CHARDATA_H_
#define _MUDLIB_XML_CHARDATA_H_

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
     * @brief Type definition of a @c char_data pointer.
     */
    typedef std::shared_ptr<mud::xml::char_data> ptr;

    /**
     * @brief Create a @c char_data instance with text.
     * @param text The text to set.
     */
    static ptr create(const std::string& text);

    /**
     * @brief Move a character data.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     */
    char_data(char_data&& rhs) = default;

    /**
     * @brief Move a character data through assignment.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     * @return A reference to this character data.
     */
    char_data& operator=(char_data&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~char_data() = default;

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
    /**
     * @brief Create a character data with text.
     */
    char_data(const std::string& text);

    /** The character data contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CHARDATA_H_ */
