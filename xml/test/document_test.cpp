#include "mud/test.h"
#include "mud/xml/document.h"
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
    mud::xml::document doc;

END_CONTEXT()

FEATURE("Document")
    DEFINE_GIVEN("An empty document",
      [](context& ctx) {
      })
    DEFINE_GIVEN("A well-formed document",
      [](context& ctx) {
        mud::xml::element root;
        root.name("root");
        ctx.doc.nodes().push_back(std::move(root));
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
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::xml::document>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::xml::document>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
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
            ASSERT(0, ctx.doc.nodes().size());
        })

  SCENARIO("A document with an element node has a root element")
    GIVEN("A well-formed document")
    WHEN ("The document is examined")
    THEN ("There is a root element  node",
      [](context& ctx) {
        ASSERT("root", ctx.doc.root().name());
      })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
