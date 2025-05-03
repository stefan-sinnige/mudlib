#ifndef _MUDLIB_XML_CDATASECTION_H_
#define _MUDLIB_XML_CDATASECTION_H_

#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML CDATA section
 */
class MUDLIB_XML_API cdata_section : public node
{
public:
    /**
     * @brief Type definition of a @c cdata_section pointer.
     */
    typedef std::shared_ptr<mud::xml::cdata_section> ptr;

    /**
     * @brief Create a new @c cdata_section instance.
     * @param text The text to set.
     */
    static ptr create(const std::string& text);

    /**
     * @brief Move a CDATA section.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     */
    cdata_section(cdata_section&& rhs) = default;

    /**
     * @brief Move a CDATA section through assignment.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     * @return A reference to this CDATA section.
     */
    cdata_section& operator=(cdata_section&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~cdata_section() = default;

    /**
     * @brief Return the contents of the CDATA section.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the CDATA section.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /**
     * @brief Create an CDATA section with text.
     */
    cdata_section(const std::string& text);

    /** The CDATA section contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CDATASECTION_H_ */
