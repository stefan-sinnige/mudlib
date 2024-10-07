#ifndef _MUDLIB_XML_DOCUMENT_H_
#define _MUDLIB_XML_DOCUMENT_H_

#include <mud/xml/element.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML document.
 *
 * @details
 * As defined in the XML Information Set (Second Edition), an XML document has
 * the following properties:
 *   * A single child element, the @em root element.
 *   * An ordered sequence of children. This can be different XML node types,
 *     however, it contains only a single element type (the @em root element).
 */
class MUDLIB_XML_API document : public node
{
public:
    /**
     * @brief Type definition of a @c document pointer.
     */
    typedef std::shared_ptr<mud::xml::document> ptr;

    /**
     * @brief Create a new @c document instance.
     */
    static ptr create();

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
     *
     * @details
     * Return the root element of the document. This is the first (and only)
     * element child node of the document at the top level.
     *
     * @throws mud::xml::not_found when there is no root element defined.
     */
    mud::xml::element::ptr root() const;

    /**
     * @brief Return the children of the document.
     */
    virtual const mud::xml::node_seq& children() const override;

    /**
     * @brief Set the children of the document.
     * @param[in] value The children of the document. There should only be
     * a singe node of the element type (the @em root element).
     */
    void children(const mud::xml::node_seq& value);
    void children(mud::xml::node_seq&& value);

    /**
     * @brief Add a child to the element.
     * @param[in] value The node to add.
     */
    void child(const mud::xml::node::ptr& value);

private:
    /**
     * @brief Create an empty document.
     */
    document();

    /** The direct nodes (one of them should be the 'root' element. */
    mud::xml::node_seq _children;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DOCUMENT_H_ */
