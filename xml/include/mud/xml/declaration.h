#ifndef _MUDLIB_XML_DECLARATION_H_
#define _MUDLIB_XML_DECLARATION_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML declaration
 */
class MUDLIB_XML_API declaration : public node
{
public:
    /**
     * @brief Create an empty declaration..
     */
    declaration();

    /**
     * @brief Copy a declaration.
     * @param[in] rhs The declaration to copy from.
     */
    declaration(const declaration& rhs);

    /**
     * @brief Copy a declaration through assignment.
     * @param[in] rhs The declaration to copy from.
     * @return A reference to this declaration.
     */
    declaration& operator=(const declaration& rhs);

    /**
     * @brief Move a declaration.
     * @param[in] rhs The declaration to move from. After moving,
     * it will resemble an empty declaration.
     */
    declaration(declaration&& rhs);

    /**
     * @brief Move a declaration through assignment.
     * @param[in] rhs The declaration to move from. After moving,
     * it will resemble an empty declaration.
     * @return A reference to this declaration.
     */
    declaration& operator=(declaration&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~declaration();

    /**
     * @brief Return the version of the declaration.
     */
    const std::string& version() const;

    /**
     * @brief Set the version of the declaration
     * @param[in] value The value to set.
     */
    void version(const std::string& value);
    void version(std::string&& value);

    /**
     * @brief Return the encoding of the XML document.
     */
    const std::string& encoding() const;

    /**
     * @brief Set the encoding of the XML document
     * @param[in] value The value to set.
     */
    void encoding(const std::string& value);
    void encoding(std::string&& value);

    /**
     * @brief Return the standalone status.
     */
    bool standalone() const;

    /**
     * @brief Set the standalone status.
     * @param[in] value The value to set.
     */
    void standalone(bool value);

private:
    /** The declaration version */
    std::string _version;

    /** The encoding */
    std::string _encoding;

    /** The standalone declaration */
    bool _standalone;

    /** The declaration data */
    std::string _data;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DECLARATION_H_ */
