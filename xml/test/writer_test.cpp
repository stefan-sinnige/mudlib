#include "mud/test.h"
#include "mud/xml/cdata_section.h"
#include "mud/xml/char_data.h"
#include "mud/xml/comment.h"
#include "mud/xml/declaration.h"
#include "mud/xml/element.h"
#include "mud/xml/processing_instruction.h"
#include "mud/xml/writer.h"
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
    mud::xml::document doc;

END_CONTEXT()

FEATURE("Writer")
    DEFINE_WHEN("The text is written",
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
            mud::xml::declaration decl;
            ctx.doc.nodes().push_back(decl);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<?xml version="1.0"?>)XML", ctx.text.str());
        })

  SCENARIO("Writing empty root element")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root/>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element hierarchy")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.nodes().push_back(mud::xml::element("child-1"));
            root.nodes().push_back(mud::xml::element("child-2"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><child-1/><child-2/></root>)XML",
                   ctx.text.str());
        })

  SCENARIO("Writing XML element with attributes")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.attributes().push_back(
                    mud::xml::attribute("attr-1", "value-1"));
            root.attributes().push_back(
                    mud::xml::attribute("attr-2", "value-2"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root attr-1="value-1" attr-2="value-2"/>)XML",
                   ctx.text.str());
        })

  SCENARIO("Writing XML element with character data")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.nodes().push_back(
                    mud::xml::char_data("Lorus ipsum"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root>Lorus ipsum</root>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element with CDATA section")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.nodes().push_back(
                    mud::xml::cdata_section("Lorus ipsum"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><![CDATA[Lorus ipsum]]></root>)XML",
                   ctx.text.str());
        })

  SCENARIO("Writing XML element with comment")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.nodes().push_back(
                    mud::xml::comment("Lorus ipsum"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><!--Lorus ipsum--></root>)XML", ctx.text.str());
        })

  SCENARIO("Writing XML element with processing instruction")
    GIVEN("An XML document",
        [](context& ctx) {
            mud::xml::element root("root");
            root.nodes().push_back(
                    mud::xml::processing_instruction("php", "phpinfo();"));
            ctx.doc.nodes().push_back(root);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"XML(<root><?php phpinfo();?></root>)XML", ctx.text.str());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
