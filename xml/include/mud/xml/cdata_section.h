#ifndef _MUDLIB_XML_CDATASECTION_H_
#define _MUDLIB_XML_CDATASECTION_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
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
     * @brief Create an empty CDATA section.
     */
    cdata_section();

    /**
     * @brief Create an CDATA section.
     * @param[in] data The character data.
     */
    cdata_section(const std::string& data);

    /**
     * @brief Copy a CDATA section.
     * @param[in] rhs The CDATA section to copy from.
     */
    cdata_section(const cdata_section& rhs);

    /**
     * @brief Copy a CDATA section through assignment.
     * @param[in] rhs The CDATA section to copy from.
     * @return A reference to this CDATA section.
     */
    cdata_section& operator=(const cdata_section& rhs);

    /**
     * @brief Move a CDATA section.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     */
    cdata_section(cdata_section&& rhs);

    /**
     * @brief Move a CDATA section through assignment.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     * @return A reference to this CDATA section.
     */
    cdata_section& operator=(cdata_section&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~cdata_section();

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
    /** The CDATA section contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CDATASECTION_H_ */
