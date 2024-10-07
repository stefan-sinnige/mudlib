#include "mud/test.h"
#include "mud/xml/element.h"
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

    /* An element */
    mud::xml::element::ptr element;

    /* Another element */
    mud::xml::element::ptr other;
END_CONTEXT()

FEATURE("Element")
      DEFINE_GIVEN("An empty element",
        [](context& ctx) {
            ctx.element = mud::xml::dom::create_element();
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Element type traits")
    GIVEN("A element type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::xml::element>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::xml::element>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
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
            ctx.element = mud::xml::dom::create_element();
            ctx.element->name("book");
            {
                auto attr = mud::xml::dom::create_attribute();
                attr->name("isbn");
                attr->value("1-56619-909-3");
                ctx.element->attribute(attr);
            }
            {
                auto child = mud::xml::dom::create_element();
                child->name("title");
                {
                    auto attr = mud::xml::dom::create_attribute();
                    attr->name("name");
                    attr->value("The Encyclopedia of Ships");
                    child->attribute(attr);
                }
                ctx.element->child(child);
            }
        })
    WHEN ("The element is moved to another one",
        [](context& ctx) {
            ctx.other = std::move(ctx.element);
        })
    THEN ("The contents have been moved",
        [](context& ctx) {
            ASSERT("book", ctx.other->name());
            ASSERT(1, ctx.other->attributes().size());
            {
                auto attr = *ctx.other->attributes().begin();
                ASSERT("isbn", attr->name());
                ASSERT("1-56619-909-3", attr->value());
            }
            ASSERT(1, ctx.other->children().size());
            {
                auto child = std::dynamic_pointer_cast<mud::xml::element>(
                        *ctx.other->children().begin());
                {
                    auto attr = *child->attributes().begin();
                    ASSERT("name", attr->name());
                    ASSERT("The Encyclopedia of Ships", attr->value());
                }
            }
        })
    AND  ("The original element is empty",
        [](context& ctx) {
            ASSERT(nullptr, ctx.element.get());
        })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
