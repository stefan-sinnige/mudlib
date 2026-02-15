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

#include "mud/test.h"
#include "mud/xml/document.h"
#include "mud/xml/query.h"
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The document */
    mud::xml::document::ptr doc;

    /* The query */
    mud::xml::query query;

    /* The query result */
    mud::xml::query_result result;
END_CONTEXT()

FEATURE("Query")
    DEFINE_GIVEN("A sample XML document", [](context& ctx) {
        std::stringstream sstr;
        sstr << R"XML(<?xml version="1.0" encoding="UTF-8"?>
<catalog xmlns="http://library.example.org/">
  <book isbn="978-0-14-027536-0">
    <author>Homer</author>
    <title>Illiad</title>
  </book>
  <book isbn="978-1-51-155896-9">
    <author>Erasmus</author>
    <title>In Praise of Folly</title>
  </book>
</catalog>)XML";
        sstr >> ctx.doc;
    })
  DEFINE_WHEN ("The query is evaluated on the document", [](context& ctx){
            ctx.result = ctx.query.evaluate(ctx.doc);
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Query type traits")
    GIVEN("A query type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::xml::query>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::xml::query>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::xml::query>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::xml::query>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::xml::query>::value);
        })

  SCENARIO("A query on an empty document returns an empty result")
    GIVEN("An empty XML document", [](context&){})
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/author");
       })
    WHEN ("The query is evaluated on the document")
    THEN ("The query returns an empty result",
        [](context& ctx) {
            ASSERT(0, ctx.result.nodes().size());
            ASSERT(0, ctx.result.strings().size());
        })

  SCENARIO("An absolute query to element nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/author");
       })
    WHEN ("The query is evaluated on the document")
    THEN ("The query returns the selected elements",
        [](context& ctx) {
            ASSERT(2, ctx.result.nodes().size());
            auto iter = ctx.result.nodes().begin();
            auto end_iter = ctx.result.nodes().end();
            ASSERT("author", std::dynamic_pointer_cast<mud::xml::element>(*iter)->name());
            ++iter;
            ASSERT("author", std::dynamic_pointer_cast<mud::xml::element>(*iter)->name());
            ++iter;
            ASSERT(true, end_iter == iter);
        })

  SCENARIO("An absolute query to text nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/author/text()");
       })
    WHEN ("The query is evaluated on the document")
    THEN ("The query returns the selected elements",
        [](context& ctx) {
            ASSERT(2, ctx.result.strings().size());
            auto iter = ctx.result.strings().begin();
            auto end_iter = ctx.result.strings().end();
            ASSERT("Homer", *iter);
            ++iter;
            ASSERT("Erasmus", *iter);
            ++iter;
            ASSERT(true, end_iter == iter);
        })

  SCENARIO("A relative query to element nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("book/author");
       })
    WHEN ("The query is evaluated on an element node", [](context& ctx){
            ctx.result = ctx.query.evaluate(ctx.doc->root());
        })
    THEN ("The query returns the selected elements",
        [](context& ctx) {
            ASSERT(2, ctx.result.nodes().size());
            auto iter = ctx.result.nodes().begin();
            auto end_iter = ctx.result.nodes().end();
            ASSERT("author", std::dynamic_pointer_cast<mud::xml::element>(*iter)->name());
            ++iter;
            ASSERT("author", std::dynamic_pointer_cast<mud::xml::element>(*iter)->name());
            ++iter;
            ASSERT(true, end_iter == iter);
        })

  SCENARIO("A relative query to text nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("book/author/text()");
       })
    WHEN ("The query is evaluated on an element node", [](context& ctx){
            ctx.result = ctx.query.evaluate(ctx.doc->root());
        })
    THEN ("The query returns the selected elements",
        [](context& ctx) {
            ASSERT(2, ctx.result.strings().size());
            auto iter = ctx.result.strings().begin();
            auto end_iter = ctx.result.strings().end();
            ASSERT("Homer", *iter);
            ++iter;
            ASSERT("Erasmus", *iter);
            ++iter;
            ASSERT(true, end_iter == iter);
        })

  SCENARIO("A query to attribute nodes")
    GIVEN("A sample XML document")
    AND  ("A query to select an attribute", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/@isbn");
       })
    WHEN ("The query is evaluated on the document")
    THEN ("The query returns the selected attributes",
        [](context& ctx) {
            ASSERT(2, ctx.result.nodes().size());
            auto iter = ctx.result.nodes().begin();
            auto end_iter = ctx.result.nodes().end();
            ASSERT("isbn", std::dynamic_pointer_cast<mud::xml::attribute>(*iter)->name());
            ASSERT("978-0-14-027536-0", std::dynamic_pointer_cast<mud::xml::attribute>(*iter)->value());
            ++iter;
            ASSERT("isbn", std::dynamic_pointer_cast<mud::xml::attribute>(*iter)->name());
            ASSERT("978-1-51-155896-9", std::dynamic_pointer_cast<mud::xml::attribute>(*iter)->value());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
