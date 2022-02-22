#ifndef _MUDLIB_XML_DOCUMENT_H_
#define _MUDLIB_XML_DOCUMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/element.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML document.
 */
class MUDLIB_XML_API document
{
public:
    /**
     * @brief Create an empty document.
     */
    document();

    /**
     * @brief Copy a document.
     * @param[in] rhs The document to copy from.
     */
    document(const document& rhs);

    /**
     * @brief Copy a document through assignment.
     * @param[in] rhs The document to copy from.
     * @return A reference to this document.
     */
    document& operator=(const document& rhs);

    /**
     * @brief Move a document.
     * @param[in] rhs The document to move from. After moving, it will
     * resemble an empty document.
     */
    document(document&& rhs);

    /**
     * @brief Move a document through assignment.
     * @param[in] rhs The document to move from. After moving, it will
     * resemble an empty document.
     * @return A reference to this document.
     */
    document& operator=(document&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~document();

    /**
     * @brief Return the root element.
     @ @throws mud::xml::not_found when there is no root element defined.
     */
    element& root();
    const element& root() const;

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
    /** The direct nodes (one of them should be the 'root' element. */
    mud::core::poly_vector<mud::xml::node> _nodes;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DOCUMENT_H_ */
