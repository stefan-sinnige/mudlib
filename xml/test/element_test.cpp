#include "mud/test.h"
#include "mud/xml/element.h"
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

    /* An element */
    mud::xml::element element;

    /* Another element */
    mud::xml::element other;
END_CONTEXT()

FEATURE("Element")
      DEFINE_GIVEN("An empty element",
        [](context& ctx) {
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Element type traits")
    GIVEN("A element type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::xml::element>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::xml::element>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::xml::element>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::xml::element>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::xml::element>::value);
        })

  SCENARIO("Moving a complex element")
    GIVEN("A complex element",
        [](context& ctx) {
            ctx.element.name("book");
            {
                mud::xml::attribute attr;
                attr.name("isbn");
                attr.value("1-56619-909-3");
                ctx.element.attributes().push_back(attr);
            }
            {
                mud::xml::element child;
                child.name("title");
                {
                    mud::xml::attribute attr;
                    attr.name("name");
                    attr.value("The Encyclopedia of Ships");
                    child.attributes().push_back(attr);
                }
                ctx.element.nodes().push_back(child);
            }
        })
    WHEN ("The element is moved to another one",
        [](context& ctx) {
            ctx.other = std::move(ctx.element);
        })
    THEN ("The contents have been moved",
        [](context& ctx) {
            ASSERT("book", ctx.other.name());
            ASSERT(1, ctx.other.attributes().size());
            {
                auto& attr = *ctx.other.attributes().begin();
                ASSERT("isbn", attr.name());
                ASSERT("1-56619-909-3", attr.value());
            }
            ASSERT(1, ctx.other.nodes().size());
            {
                auto& child = dynamic_cast<mud::xml::element&>(
                        *ctx.other.nodes().begin());
                {
                    auto& attr = *child.attributes().begin();
                    ASSERT("name", attr.name());
                    ASSERT("The Encyclopedia of Ships", attr.value());
                }
            }
        })
    AND  ("The original element is empty",
        [](context& ctx) {
            ASSERT("", ctx.element.name());
            ASSERT(0, ctx.element.attributes().size());
            ASSERT(0, ctx.element.nodes().size());
        })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
