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
#include "mud/xml/dom.h"
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

END_CONTEXT()

FEATURE("Document")
    DEFINE_GIVEN("An empty document",
      [](context& ctx) {
        ctx.doc = mud::xml::dom::create_document();
      })
    DEFINE_GIVEN("A well-formed document",
      [](context& ctx) {
        ctx.doc = mud::xml::dom::create_document();
        mud::xml::element::ptr root = mud::xml::dom::create_element("root");
        ctx.doc->child(root);
      })
    DEFINE_WHEN ("The document is examined",
      [](context& ctx) {
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Document type traits")
    GIVEN("A document type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::xml::document>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::xml::document>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::xml::document>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::xml::document>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::xml::document>::value);
        })

  /* Although this is not a valid XML document */
  SCENARIO("An empty document has no nodes")
    GIVEN("An empty document")
    WHEN ("The document is examined")
    THEN ("The document has no nodes",
        [](context& ctx) {
            ASSERT(0, ctx.doc->children().size());
        })

  SCENARIO("A document with an element node has a root element")
    GIVEN("A well-formed document")
    WHEN ("The document is examined")
    THEN ("There is a root element  node",
      [](context& ctx) {
        ASSERT("root", ctx.doc->root()->name());
      })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
