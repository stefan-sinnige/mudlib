#include "mud/test.h"
#include "mud/xml/cdata_section.h"
#include "mud/xml/char_data.h"
#include "mud/xml/comment.h"
#include "mud/xml/declaration.h"
#include "mud/xml/document.h"
#include "mud/xml/element.h"
#include "mud/xml/processing_instruction.h"
#include <memory>
#include <sstream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The XML text stream */
    std::stringstream text;

    /* The document */
    mud::xml::document::ptr doc;

END_CONTEXT()

FEATURE("Reader")
    DEFINE_WHEN("The text is read",
        [](context& ctx) {
            ctx.text >> ctx.doc;
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Reading XML declaration")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(<?xml version="1.0" encoding="UTF-8"?><root/>)XML";
        })
    WHEN("The text is read")
    THEN("The first node is a declaration",
        [](context& ctx){
            ASSERT(2, ctx.doc->children().size());
            auto node = ctx.doc->children().begin();
            /*
            auto decl = std::dynamic_pointer_cast<mud::xml::declaration>(
                    *ctx.doc->children().begin());
            ASSERT("1.0", decl->version());
            ASSERT("UTF-8", decl->encoding());
            */
        })

  SCENARIO("Reading empty root element")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(<root/>)XML";
        })
    WHEN("The text is read")
    THEN("The root element is available",
        [](context& ctx){
            ASSERT("root", ctx.doc->root()->name());
        })

  SCENARIO("Reading root element without content")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(<root></root>)XML";
        })
    WHEN("The text is read")
    THEN("The root element is available",
        [](context& ctx){
            ASSERT("root", ctx.doc->root()->name());
        })

  SCENARIO("Reading XML document with enclosed whitespace")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << " \t\r\n<?xml version=\"1.0\"?><root></root> \t\r\n";
        })
    WHEN("The text is read")
    THEN("The root element is available",
        [](context& ctx){
            ASSERT("root", ctx.doc->root()->name());
        })

  SCENARIO("Reading XML element hierarchy")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
            <root><l1-1><l2-1/><l2-2/></l1-1><l1-2><l2-3/><l2-4/></l1-2></root>
            )XML";
        })
    WHEN("The text is read")
    THEN("The root element has 2 child elements",
        [](context& ctx){
            ASSERT(2, ctx.doc->root()->children().size());
            auto iter = ctx.doc->root()->children().begin();
            auto element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l1-1", element->name());
            ++iter;
            element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l1-2", element->name());
        })
    AND ("The first child element has 2 child elements",
        [](context& ctx){
            auto iter = ctx.doc->root()->children().begin();
            auto element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT(2, element->children().size());
            iter = element->children().begin();
            element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l2-1", element->name());
            ++iter;
            element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l2-2", element->name());
        })
    AND ("The second child element has 2 child elements",
        [](context& ctx){
            auto iter = ctx.doc->root()->children().begin();
            ++iter;
            auto element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT(2, element->children().size());
            iter = element->children().begin();
            element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l2-3", element->name());
            ++iter;
            element = std::dynamic_pointer_cast<mud::xml::element>(*iter);
            ASSERT("l2-4", element->name());
        })

  SCENARIO("Reading XML element with attributes")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root id="3"
                      class="toplevel">
                </root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has two attributes",
        [](context& ctx){
            // Attributes are unordered
            ASSERT(2, ctx.doc->root()->attributes().size());
            for (auto attr: ctx.doc->root()->attributes()) {
                if (attr->name().empty()) {
                    ASSERT("id", attr->prefix());
                    ASSERT("3", attr->value());
                }
                else
                if (attr->name() == "class") {
                    ASSERT("class", attr->name());
                    ASSERT("toplevel", attr->value());
                }
            }
        })

  SCENARIO("Reading XML child element with attributes")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root><child att1="true" class="toplevel"/></root>)XML";
        })
    WHEN("The text is read")
    THEN("The child element has two attributes",
        [](context& ctx){
            auto child = std::dynamic_pointer_cast<mud::xml::element>(
                *ctx.doc->root()->children().begin());
            for (auto attr: child->attributes()) {
                if (attr->name() == "att1") {
                    ASSERT("att1", attr->name());
                    ASSERT("true", attr->value());
                }
                else
                if (attr->name() == "class") {
                    ASSERT("class", attr->name());
                    ASSERT("toplevel", attr->value());
                }
            }
        })

  SCENARIO("Reading XML element with an empty attribute value")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root id="">
                </root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has one attributes",
        [](context& ctx){
            ASSERT(1, ctx.doc->root()->attributes().size());
            auto iter = ctx.doc->root()->attributes().begin();
            ASSERT("id", (*iter)->name());
            ASSERT("", (*iter)->value());
        })

  SCENARIO("Reading XML element with character data")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root>Hello World</root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has a character data node",
        [](context& ctx){
            auto node = ctx.doc->root()->children().at(0);
            ASSERT(mud::xml::node::type_t::CHAR_DATA, node->type());
            auto char_data = std::dynamic_pointer_cast<mud::xml::char_data>(node);
            ASSERT("Hello World", char_data->text());
        })

  SCENARIO("Reading XML element with CDATA section")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root><![CDATA[Hello <data> World]]></root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has a CDATA section node",
        [](context& ctx){
            auto node = ctx.doc->root()->children().at(0);
            ASSERT(mud::xml::node::type_t::CDATA_SECTION, node->type());
            auto cdata = std::dynamic_pointer_cast<mud::xml::cdata_section>(node);
            ASSERT("Hello <data> World", cdata->text());
        })

  SCENARIO("Reading XML element with comment")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root><!-- Comment --></root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has a comment node",
        [](context& ctx){
            auto node = ctx.doc->root()->children().at(0);
            ASSERT(mud::xml::node::type_t::COMMENT, node->type());
            auto comment = std::dynamic_pointer_cast<mud::xml::comment>(node);
            ASSERT(" Comment ", comment->text());
        })

  SCENARIO("Reading XML element with processing instruction")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root><?target The instruction data?></root>)XML";
        })
    WHEN("The text is read")
    THEN("The element has a processing instruction node",
        [](context& ctx){
            auto node = ctx.doc->root()->children().at(0);
            ASSERT(mud::xml::node::type_t::PI, node->type());
            auto pi = std::dynamic_pointer_cast<mud::xml::processing_instruction>(node);
            ASSERT("target", pi->target());
            ASSERT(" The instruction data", pi->data());
        })

  SCENARIO("Reading XML document with escaped characters")
    GIVEN("A document text",
        [](context& ctx){
            ctx.text << R"XML(
                <root att="Read &amp; weep">one &lt; three &gt; two</root>)XML";
        })
    WHEN("The text is read")
    THEN("The attribute is unescaped",
        [](context& ctx){
            auto root = ctx.doc->root();
            ASSERT(1, root->attributes().size());
            auto att = *(root->attributes().begin());
            ASSERT("Read & weep", att->value());
        })
    AND ("The text is unescaped",
        [](context& ctx){
            auto root = ctx.doc->root();
            ASSERT(1, root->children().size());
            auto text = root->children().at(0);
            ASSERT(mud::xml::node::type_t::CHAR_DATA, text->type());
            auto char_data = std::dynamic_pointer_cast<mud::xml::char_data>(text);
            ASSERT("one < three > two", char_data->text());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
