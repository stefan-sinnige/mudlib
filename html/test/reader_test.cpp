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
#include "mud/html/raw_data.h"
#include "mud/html/reader.h"
#include "mud/test.h"
#include <memory>
#include <sstream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() : exception_hash(0) {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The HTML text stream */
    std::stringstream text;

    /* The document */
    mud::html::document doc;

    /* Any exception being thrown */
    size_t exception_hash;
    std::string exception_what;

END_CONTEXT()

FEATURE("Reader")
    DEFINE_WHEN("The text is read",
        [](context& ctx) {
            try {
                ctx.text >> ctx.doc;
            }
            catch (std::exception& ex) {
                ctx.exception_hash = typeid(ex).hash_code();
                ctx.exception_what = ex.what();
            }
        })
    DEFINE_THEN("An exception is thrown",
        [](context& ctx) {
            ASSERT(true, ctx.exception_hash != 0);
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Reading full doctype preamble")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html system "about:legacy-compat"><html/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("Only the HTML node is recorded",
        [](context& ctx) {
            ASSERT(1, ctx.doc.nodes().size());
            ASSERT("html", ctx.doc.root().name());
        })

  SCENARIO("Reading case-insensitive minimal doctype preamble")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!DoCtYpE HtMl><html/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("Only the HTML node is recorded",
        [](context& ctx) {
            ASSERT(1, ctx.doc.nodes().size());
            ASSERT("html", ctx.doc.root().name());
        })

  SCENARIO("Reading empty HTML tag")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag is empty",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(0, ctx.doc.root().attributes().size());
            ASSERT(0, ctx.doc.root().nodes().size());
        })

  SCENARIO("Reading case-insensitive HTML tag")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <HtMl/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag is empty",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(0, ctx.doc.root().attributes().size());
            ASSERT(0, ctx.doc.root().nodes().size());
        })

  SCENARIO("Reading HTML tag with double-quoted attribute value")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html att="Attribute Value"/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag is empty",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("att", iter->name());
            ASSERT("Attribute Value", iter->value());
        })

  SCENARIO("Reading HTML tag with single-quoted attribute value")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html att='Attribute Value'/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag is empty",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("att", iter->name());
            ASSERT("Attribute Value", iter->value());
        })

  SCENARIO("Reading HTML tag with unquoted attribute value")
    GIVEN("An HTML document",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html att=value />
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag contains the attribute value",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("att", iter->name());
            ASSERT("value", iter->value());
        })

  SCENARIO("Reading HTML tag with attribute value containing char-references")
    GIVEN("An HTML document with an attribute value containing char-references",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html src="This is an '&amp;' (ampersand)"/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag contains the unescaped attribute value",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("src", iter->name());
            ASSERT("This is an '&' (ampersand)", iter->value());
        })

  SCENARIO("Reading HTML tag with attribute value containing encoded URL")
    GIVEN("An HTML document with an attribute value containing encoded URL",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html src="http://www.example.com/index.php?c=q&t=w"/>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag contains the unescaped attribute value",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("src", iter->name());
            ASSERT("http://www.example.com/index.php?c=q&t=w", iter->value());
        })

  SCENARIO("Reading HTML tag with attribute without a value")
    GIVEN("An HTML document with an attribute containing no value",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html att />
            )HTML";
        })
    WHEN("The text is read")
    THEN("The HTML tag contains the attribute value with empty value",
        [](context& ctx) {
            ASSERT("html", ctx.doc.root().name());
            ASSERT(1, ctx.doc.root().attributes().size());
            auto iter = ctx.doc.root().attributes().begin();
            ASSERT("att", iter->name());
            ASSERT("", iter->value());
        })

  SCENARIO("Reading TITLE tag with escapable raw text")
    GIVEN("An HTML document with a TITLE tag",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html><title>This is an '&amp;' (ampersand)</title></html>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The TITLE tag contains the text",
        [](context& ctx) {
            auto title = dynamic_cast<mud::html::element&>(
                *ctx.doc.root().nodes().begin());
            ASSERT("title", title.name());
            auto& node = dynamic_cast<mud::html::char_data&>(
                title.nodes().at(0));
            ASSERT("This is an '&' (ampersand)", node.text());
        })

  SCENARIO("Reading TEXTAREA tag with escapable raw text")
    GIVEN("An HTML document with a TEXTAREA tag",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html><textarea>[ -z "$PS1" ] &amp;&amp; return</textarea></html>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The TEXTAREA tag contains the text",
        [](context& ctx) {
            auto textarea = dynamic_cast<mud::html::element&>(
                *ctx.doc.root().nodes().begin());
            ASSERT("textarea", textarea.name());
            auto& node = dynamic_cast<mud::html::char_data&>(
                textarea.nodes().at(0));
            ASSERT("[ -z \"$PS1\" ] && return", node.text());
        })

  SCENARIO("Reading AREA void tag")
    GIVEN("An HTML document with an AREA tag",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html><area shape="cirle" coords="10,25,5"></html>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The AREA tag contains no text",
        [](context& ctx) {
            auto area = dynamic_cast<mud::html::element&>(
                *ctx.doc.root().nodes().begin());
            ASSERT("area", area.name());
            ASSERT(0, area.nodes().size());
        })

  SCENARIO("Reading AREA void tag with erroneous end tag")
    GIVEN("An HTML document with an AREA ended with an empty close tag",
        [](context& ctx) {
            ctx.text << R"HTML(
                <!doctype html>
                <html><area shape="cirle" coords="10,25,5"/></html>
            )HTML";
        })
    WHEN("The text is read")
    THEN("The AREA tag contains no text",
        [](context& ctx) {
            auto area = dynamic_cast<mud::html::element&>(
                *ctx.doc.root().nodes().begin());
            ASSERT("area", area.name());
            ASSERT(0, area.nodes().size());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
