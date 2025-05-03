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

    /* A list of nodes */
    mud::xml::node_seq nodes;

    /* An attribute */
    mud::xml::attribute::ptr attribute;

    /* An namespace uri */
    mud::core::uri uri;
END_CONTEXT()

FEATURE("Element")
      DEFINE_WHEN("The element is examined",
        [](context& ctx) {
        })
      DEFINE_THEN("The prefix is empty",
        [](context& ctx) {
            ASSERT("", ctx.element->prefix());
        })
      DEFINE_THEN("The local-name is the same as the qname",
        [](context& ctx) {
            ASSERT(ctx.element->local_name(), ctx.element->name());
        })
      DEFINE_THEN("The namespace is not set",
        [](context& ctx) {
            ASSERT(true, ctx.element->ns()->uri().empty());
        })
      DEFINE_THEN("The namespace is set",
        [](context& ctx) {
            ASSERT(ctx.uri, ctx.element->ns()->uri());
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
            ctx.element = mud::xml::dom::create_element("book");
            {
                auto attr = mud::xml::dom::create_attribute("isbn");
                attr->value("1-56619-909-3");
                ctx.element->attribute(attr);
            }
            {
                auto child = mud::xml::dom::create_element("title");
                {
                    auto attr = mud::xml::dom::create_attribute("name");
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

  SCENARIO("Querying an element's child element")
    GIVEN("An element with various children",
        [](context& ctx) {
            ctx.element = mud::xml::dom::create_element("catalog");
            for (int i = 0; i < 10; ++i) {
                if (i % 2 == 0) {
                    auto book = mud::xml::dom::create_element("book");
                    ctx.element->child(book);
                }
                else {
                    auto cd = mud::xml::dom::create_element("cd");
                    ctx.element->child(cd);
                }
            }
        })
    WHEN ("The a list of elements is queried by name",
        [](context& ctx) {
            ctx.nodes = ctx.element->elements("book");
        })
    THEN ("The correct number of elements is returned",
        [](context& ctx) {
            ASSERT(5, ctx.nodes.size());
        })
     AND ("The elements have the signature",
        [](context& ctx) {
            for (auto& child: ctx.nodes) {
                ASSERT(mud::xml::node::type_t::ELEMENT, child->type());
                auto elem = std::static_pointer_cast<mud::xml::element>(child);
                ASSERT("book", elem->local_name());
            }
        })

  SCENARIO("Querying an element's attribute")
    GIVEN("An element with attributes",
        [](context& ctx) {
            ctx.element = mud::xml::dom::create_element("catalog");
            auto attr_1 = mud::xml::dom::create_attribute("attr-1");
            ctx.element->attribute(attr_1);
            auto attr_2 = mud::xml::dom::create_attribute("attr-2");
            ctx.element->attribute(attr_2);
            auto attr_3 = mud::xml::dom::create_attribute("attr-3");
            ctx.element->attribute(attr_3);
        })
    WHEN ("The an attribute is queried by name",
        [](context& ctx) {
            ctx.attribute = ctx.element->attribute("attr-2");
        })
    THEN ("The correct attribute is returned",
        [](context& ctx) {
            ASSERT(false, ctx.attribute == nullptr);
            ASSERT("attr-2", ctx.attribute->local_name());
        })

  SCENARIO("Creating an element with no namespace")
    GIVEN("An element with no namespace",
        [](context& ctx) {
            ctx.element = mud::xml::dom::create_element("book");
        })
    WHEN ("The element is examined")
    THEN ("The local-name is the same as the qname")
     AND ("The prefix is empty")
     AND ("The namespace is not set")

  SCENARIO("Creating an element with a default namespace")
    GIVEN("An element with a default namespace",
        [](context& ctx) {
            ctx.uri = mud::core::uri("http://library.example.org");
            ctx.element = mud::xml::dom::create_element("book", ctx.uri);
        })
    WHEN ("The element is examined")
    THEN ("The local-name is the same as the qname")
     AND ("The prefix is empty")
     AND ("The namespace is set")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
