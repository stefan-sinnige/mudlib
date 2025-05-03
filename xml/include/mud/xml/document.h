#ifndef _MUDLIB_XML_DOCUMENT_H_
#define _MUDLIB_XML_DOCUMENT_H_

#include <mud/xml/element.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <iostream>
#include <vector>

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
    document(document&& rhs) = default;;

    /**
     * @brief Move a document through assignment.
     * @param[in] rhs The document to move from. After moving, it will
     * resemble an empty document.
     * @return A reference to this document.
     */
    document& operator=(document&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~document() = default;

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

    /**
     * @brief Resolve all namespaces.
     *
     * @details
     * When creating @c elements and @c attributes by qualified name only
     * (without defining the namespace at time of creation) renders the
     * namespace resolution incomplete as there is not associated URI defined.
     * The namespace URI can only be resolved in the conetxt of its ancestors.
     * The @c resolve method will aim to realign the namespace settings of all
     * elements and attributes to the namespace definitions (the @c xmlns 
     * attributes of their ancestors). This is accomplished by matching the
     * element and attribute prefixes to any namespace definition of its
     * ancestors that matches the prefix.
     *
     * If the namespace definition cannot be resolved, the namespace URI shall
     * remain as-is (empty).
     */
    void resolve();

private:
    /**
     * @brief Create an empty document.
     */
    document();

    /**
     * @brief Resolve all namespaces for the element and its children.
     *
     * @param element The element and its attributes to resolve.
     * @param parent_ctx The namespace context from its parent.
     */
    void resolve(
        const mud::xml::element::ptr& element,
        const std::vector<mud::xml::ns::ptr>& parent_ctx);

    /** The direct nodes (one of them should be the 'root' element. */
    mud::xml::node_seq _children;
};

/**
 * @brief Manipulator to output debug scanning information to standard error.
 * @note Output will only be generated if the lexical analyser has debugging
 * information enabled.
 */
std::ios_base&
scanner_debug(std::ios_base& istr);

/**
 * @brief Manipulator to output debug parsing information.
 * @note Output will only be generated if the parser generator has debugging
 * information enabled.
 */
std::ios_base&
parser_debug(std::ios_base& istr);

END_MUDLIB_XML_NS

/**
 * @brief Insertion operator to output an XML document to an output stream.
 * @param[in] ostr The stream to output the XML document to.
 * @param[in,out] doc The XML document to output.
 */
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::document::ptr& doc);

/**
 * @brief Extraction operator to read an XML document from an input stream.
 * @param[in] istr The stream to read the XML document from.
 * @param[in,out] doc The object to hold the XML document.
 */
std::istream&
operator>>(std::istream& istr, mud::xml::document::ptr& doc);

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DOCUMENT_H_ */
