/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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
 *        auto book = xml::dom::create_element("book");
 *        auto author = xml::dom::create_element("author");
 *        auto author_text = xml::dom::create_char_data("Homer");
 *        author->add(author_text);
 *        auto title = xml::dom::create_element("title");
 *        auto title_text = xml::dom::create_char_data("Illiad");
 *        author->add(title_text);
 *        book->add(author);
 *        book->add(title);
 *        catalog->add(book);
 *    }
 *    {
 *        auto book = xml::dom::create_element("book");
 *        auto author = xml::dom::create_element("author");
 *        auto author_text = xml::dom::create_char_data("Erasmus");
 *        author->add(author_text);
 *        auto title = xml::dom::create_element("title");
 *        auto title_text = xml::dom::create_char_data("In Praise of Folly");
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
     * @brief Create an XML attribute node with a qualified name.
     *
     * @details
     * A qualified name is a name of the form
     * @verbatim
     *    [ prefix ':' ]local-name
     * @endverbatim
     * If the @c prefix is omitted, the @c local-name is said to be part of the
     * default namespace. Otherwise, the @c prefix references the namespace
     * prefix that the attribute is defined in.
     *
     * A special namespace definition attribute can be defined through this
     * method by specifying the name as
     * @verbatim
     *    'xmlns' [ ':' prefix ]
     * @endverbatim
     * This defines a resolved namespace whose URI is provided by the attribute
     * value. If the prefix is omitted, then the attribute defines a default
     * namespace.
     *
     * @param qname The qualified name.
     */
    static xml::attribute::ptr create_attribute(const std::string& qname);

    /**
     * @brief Create an XML attribute node with a local-name that is part of
     * a defined namespace. The local-name should @c em not contain a prefix
     * specification as that is provided by the resolved namespace definition.
     *
     * @param local_name The local-name.
     * @param uri The namespace URI.
     */
    static xml::attribute::ptr create_attribute(const std::string& local_name,
            const mud::core::uri& uri);

    /**
     * @brief Create an XML CDATA section node with text data..
     * @param text The text to set.
     */
    static xml::cdata_section::ptr create_cdata_section(
            const std::string& text);

    /**
     * @brief Create an XML character data node with text data.
     * @param text The text to set.
     */
    static xml::char_data::ptr create_char_data(const std::string& text);

    /**
     * @brief Create an XML comment node with text data.
     * @param text The text to set.
     */
    static xml::comment::ptr create_comment(const std::string& text);

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
     * @brief Create a new @c element instance with a qualified name.
     *
     * @details
     * A qualified name is a name of the form
     * @verbatim
     *    [ prefix ':' ] local-name
     * @endverbatim
     * If the @c prefix is omitted, the @c local-name is said to be part of the
     * default namespace. Otherwise, the @c prefix references the namespace
     * prefix that the element is defined in.
     */
    static xml::element::ptr create_element(const std::string& qname);

    /**
     * @brief Create an XML element node with a local-name that is part of
     * a defined namespace. The local-name should @c em not contain a prefix
     * specification as that is provided by the resolved namespace definition.
     *
     * @param local_name The local-name.
     * @param uri The namespace URI.
     */
    static xml::element::ptr create_element(const std::string& local_name,
            const mud::core::uri& uri);

    /**
     * @brief Create an empty XML processing instruction node.
     * @param text The target to set.
     * @param text The data to set.
     */
    static xml::processing_instruction::ptr create_processing_instruction(
            const std::string& target, const std::string& data);
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_DOM_H_ */
