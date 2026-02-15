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

#include "mud/html/cdata_section.h"
#include "mud/html/char_data.h"
#include "mud/html/comment.h"
#include "mud/html/declaration.h"
#include "mud/html/element.h"
#include "mud/html/processing_instruction.h"
#include "mud/html/writer.h"
#include "mud/test.h"
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

    /* The HTML text stream */
    std::stringstream text;

    /* The document */
    mud::html::document doc;

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

  SCENARIO("Writing HTML declaration")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::declaration decl;
            ctx.doc.nodes().push_back(decl);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<?html version="1.0"?>)HTML", ctx.text.str());
        })

  SCENARIO("Writing empty html element")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html/>)HTML", ctx.text.str());
        })

  SCENARIO("Writing HTML element hierarchy")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            html.nodes().push_back(mud::html::element("child-1"));
            html.nodes().push_back(mud::html::element("child-2"));
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html><child-1/><child-2/></html>)HTML",
                   ctx.text.str());
        })

  SCENARIO("Writing HTML element with attributes")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            html.attributes().push_back(
                    mud::html::attribute("attr-1", "value-1"));
            html.attributes().push_back(
                    mud::html::attribute("attr-2", "value-2"));
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html attr-1="value-1" attr-2="value-2"/>)HTML",
                   ctx.text.str());
        })

  SCENARIO("Writing HTML element with character data")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            html.nodes().push_back(
                    mud::html::char_data("Lorus ipsum"));
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html>Lorus ipsum</html>)HTML", ctx.text.str());
        })

  SCENARIO("Writing HTML element with CDATA section")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            html.nodes().push_back(
                    mud::html::cdata_section("Lorus ipsum"));
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html><![CDATA[Lorus ipsum]]></html>)HTML",
                   ctx.text.str());
        })

  SCENARIO("Writing HTML element with comment")
    GIVEN("An HTML document",
        [](context& ctx) {
            mud::html::element html("html");
            html.nodes().push_back(
                    mud::html::comment("Lorus ipsum"));
            ctx.doc.nodes().push_back(html);
        })
    WHEN ("The text is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(R"HTML(<html><!--Lorus ipsum--></html>)HTML", ctx.text.str());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
