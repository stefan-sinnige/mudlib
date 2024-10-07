#ifndef _MUDLIB_XML_DOM_H_
#define _MUDLIB_XML_DOM_H_

#include <mud/xml/attribute.h>
#include <mud/xml/cdata_section.h>
#include <mud/xml/char_data.h>
#include <mud/xml/comment.h>
#include <mud/xml/declaration.h>
#include <mud/xml/document.h>
#include <mud/xml/element.h>
#include <mud/xml/node.h>
#include <mud/xml/processing_instruction.h>
#include <mud/xml/ns.h>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of the XML Document Object Model (DOM).
 *
 * @details
 * The XML Document Object Model (DOM) is an interface definition to construct
 * and interrogate XML documents. Any XML information item (element, attribute
 * and the document itself) is intended to be created through the DOM interface
 * to ensure that the integrity of the XML document node tree is preserved.
 *
 * Although the DOM interface is quite extensively specified, only the concept
 * of the model is implemented. The DOM does not hold any state and all the
 * functionality provided by the DOM class are therefore @em static.
 *
 * When creating an XML document programmatically, the @c dom class can be
 * used in the following way:
 * @code
 *    using namespace mud;
 *
 *    xml::document::ptr doc = xml::dom::create_document();
 *    xml::element::ptr catalog = xml::dom::create_element();
 *    {
 *        xml::element::ptr book = xml::dom::create_element();
 *        xml::element::ptr author = xml::dom::create_element();
 *        xml::char_data::ptr author_text = xml::dom::create_char_data();
 *        author_text->("Homer");
 *        author->add(author_text);
 *        xml::element::ptr title = xml::dom::create_element();
 *        xml::char_data::ptr title_text = xml::dom::create_char_data();
 *        title_text->("Illiad");
 *        author->add(title_text);
 *        book->add(author);
 *        book->add(title);
 *        catalog->add(book);
 *    }
 *    {
 *        xml::element::ptr book = xml::dom::create_element();
 *        xml::element::ptr author = xml::dom::create_element();
 *        xml::char_data::ptr author_text = xml::dom::create_char_data();
 *        author_text->("Erasmus");
 *        author->add(author_text);
 *        xml::element::ptr title = xml::dom::create_element();
 *        xml::char_data::ptr title_text = xml::dom::create_char_data();
 *        title_text->("On Free Will");
 *        author->add(title_text);
 *        book->add(author);
 *        book->add(title);
 *        catalog->add(book);
 *    }
 *    doc->add(catalog);
 * @endcode
 *
 * Reference: [DOM]: https://dom.spec.whatwg.org
 */
class MUDLIB_XML_API dom
{
public:
    /**
     * @brief Create an empty XML attribute node.
     */
    static xml::attribute::ptr create_attribute();

    /**
     * @brief Create an empty XML CDATA section node.
     */
    static xml::cdata_section::ptr create_cdata_section();

    /**
     * @brief Create an empty XML character data node.
     */
    static xml::char_data::ptr create_char_data();

    /**
     * @brief Create an empty XML comment node.
     */
    static xml::comment::ptr create_comment();

    /**
     * @brief Create an empty XML declaration node.
     */
    static xml::declaration::ptr create_declaration();

    /**
     * @brief Create an empty XML document node.
     *
     * @details
     * The XML document node is the start of the XML document tree
     * representation.
     */
    static xml::document::ptr create_document();

    /**
     * @brief Create an empty XML element node.
     */
    static xml::element::ptr create_element();

    /**
     * @brief Create an empty XML processing instruction node.
     */
    static xml::processing_instruction::ptr create_processing_instruction();
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DOM_H_ */
