#include "mud/test.h"
#include "mud/xml/cdata_section.h"
#include "mud/xml/char_data.h"
#include "mud/xml/comment.h"
#include "mud/xml/declaration.h"
#include "mud/xml/document.h"
#include "mud/xml/element.h"
#include "mud/xml/processing_instruction.h"
#include "mud/xml/dom.h"
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

FEATURE("Writer")
    DEFINE_WHEN("The document is written",
        [](context& ctx) {
            ctx.text << ctx.doc;
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Writing XML declaration")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto decl = mud::xml::dom::create_declaration();
            ctx.doc->child(decl);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<?xml version="1.0"?>)XML", ctx.text.str());
        })

  SCENARIO("Writing empty root element")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root/>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element hierarchy")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto child1 = mud::xml::dom::create_element("child-1");
            auto child2 = mud::xml::dom::create_element("child-2");
            root->child(child1);
            root->child(child2);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><child-1/><child-2/></root>)XML",
                   ctx.text.str());
        })

  SCENARIO("Writing XML element with attributes")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto attr1 = mud::xml::dom::create_attribute("attr-1");
            attr1->value("value-1");
            auto attr2 = mud::xml::dom::create_attribute("attr-2");
            attr2->value("value-2");
            root->attribute(attr1);
            root->attribute(attr2);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            // Attributes are unordered
            const std::string option1 = 
                R"XML(<root attr-1="value-1" attr-2="value-2"/>)XML";
            const std::string option2 = 
                R"XML(<root attr-2="value-2" attr-1="value-1"/>)XML";
            std::string result = ctx.text.str();
            ASSERT(true, result == option1 || result == option2);
        })

  SCENARIO("Writing XML element with character data")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto char_data = mud::xml::dom::create_char_data("Lorus ipsum");
            root->child(char_data);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root>Lorus ipsum</root>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element with CDATA section")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto cdata_section = mud::xml::dom::create_cdata_section(
                    "Lorus ipsum");
            root->child(cdata_section);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><![CDATA[Lorus ipsum]]></root>)XML",
                   ctx.text.str());
        })

  SCENARIO("Writing XML element with comment")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto comment = mud::xml::dom::create_comment("Lorus ipsum");
            root->child(comment);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><!--Lorus ipsum--></root>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element with processing instruction")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto pi = mud::xml::dom::create_processing_instruction(
                    "php", "phpinfo();");
            root->child(pi);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><?php phpinfo();?></root>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML document with escaped characters")
    GIVEN("An XML document",
        [](context& ctx) {
            ctx.doc = mud::xml::dom::create_document();
            auto root = mud::xml::dom::create_element("root");
            auto attr = mud::xml::dom::create_attribute("att");
            attr->value("Read & weep");
            root->attribute(attr);
            auto char_data = mud::xml::dom::create_char_data(
                    "one < three > two");
            root->child(char_data);
            ctx.doc->child(root);
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root att="Read &amp; weep">one &lt; three &gt; two</root>)XML", ctx.text.str());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
