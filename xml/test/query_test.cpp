#include "mud/test.h"
#include "mud/xml/document.h"
#include "mud/xml/reader.h"
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
<catalog>
  <book id="1">
    <author>Homer</author>
    <title>Illiad</title>
  </book>
  <book id="2">
    <author>Erasmus</author>
    <title>On Free Will</title>
  </book>
</catalog>)XML";
        sstr >> ctx.doc;
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

  SCENARIO("An absolute query to element nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/author");
       })
    WHEN ("The query is evaluated on the document", [](context& ctx){
            ctx.result = ctx.query.evaluate(ctx.doc);
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

  SCENARIO("An absolute query to text nodes")
    GIVEN("A sample XML document")
    AND  ("A query", [](context& ctx){
            ctx.query = mud::xml::query("/catalog/book/author/text()");
       })
    WHEN ("The query is evaluated on the document", [](context& ctx){
            ctx.result = ctx.query.evaluate(ctx.doc);
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

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
