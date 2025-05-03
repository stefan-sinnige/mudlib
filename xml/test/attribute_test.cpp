#include "mud/test.h"
#include "mud/xml/attribute.h"
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

    /* An attribute */
    mud::xml::attribute::ptr attribute;

    /* A prefix */
    std::string prefix;

    /* An namespace uri */
    mud::core::uri uri;
END_CONTEXT()

FEATURE("Attribute")
      DEFINE_GIVEN("An empty attribute",
        [](context& ctx) {
        })
      DEFINE_WHEN("The attribute is examined",
        [](context& ctx) {
        })
      DEFINE_THEN("The prefix is set",
        [](context& ctx) {
            ASSERT(ctx.prefix, ctx.attribute->prefix());
        })
      DEFINE_THEN("The prefix is empty",
        [](context& ctx) {
            ASSERT("", ctx.attribute->prefix());
        })
      DEFINE_THEN ("The name is empty",
        [](context& ctx) {
            ASSERT("", ctx.attribute->name());
        })
      DEFINE_THEN ("The local-name is empty",
        [](context& ctx) {
            ASSERT("", ctx.attribute->local_name());
        })
      DEFINE_THEN("The local-name is the same as the qname",
        [](context& ctx) {
            ASSERT(ctx.attribute->local_name(), ctx.attribute->name());
        })
      DEFINE_THEN("The namespace is not set",
        [](context& ctx) {
            ASSERT(true, ctx.attribute->ns()->uri().empty());
        })
      DEFINE_THEN("The namespace is set",
        [](context& ctx) {
            ASSERT(ctx.uri, ctx.attribute->ns()->uri());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Attribute type traits")
    GIVEN("A attribute type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::xml::attribute>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::xml::attribute>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::xml::attribute>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::xml::attribute>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::xml::attribute>::value);
        })

  SCENARIO("Creating an attribute with no namespace")
    GIVEN("An attribute with no namespace",
        [](context& ctx) {
            ctx.attribute = mud::xml::dom::create_attribute("isbn");
            ctx.attribute->value("978-1-51-155896-9");
        })
    WHEN ("The attribute is examined")
    THEN ("The local-name is the same as the qname")
     AND ("The prefix is empty")
     AND ("The namespace is not set")

  SCENARIO("Creating an attribute with a default namespace")
    GIVEN("An attribute with a default namespace",
        [](context& ctx) {
            ctx.uri = mud::core::uri("http://library.example.org");
            ctx.attribute = mud::xml::dom::create_attribute("isbn", ctx.uri);
            ctx.attribute->value("978-1-51-155896-9");
        })
    WHEN ("The attribute is examined")
    THEN ("The local-name is the same as the qname")
     AND ("The prefix is empty")
     AND ("The namespace is set")

  SCENARIO("Creating an attribute with a non-default namespace")
    GIVEN("An attribute with a non-default namespace",
        [](context& ctx) {
            ctx.uri = mud::core::uri("http://library.example.org");
            ctx.attribute = mud::xml::dom::create_attribute("isbn", ctx.uri);
            ctx.attribute->value("978-1-51-155896-9");
        })
    WHEN ("The attribute is examined")
    THEN ("The local-name is the same as the qname")
     AND ("The prefix is set")
     AND ("The namespace is set")

  SCENARIO("Creating a default namespace attribute")
    GIVEN("A default namespace",
        [](context& ctx) {
            ctx.uri = mud::core::uri("http://library.example.org");
            ctx.attribute = mud::xml::dom::create_attribute("xmlns");
            ctx.attribute->value(ctx.uri);
        })
    WHEN ("The attribute is examined")
    THEN ("The name is empty")
     AND ("The local-name is empty")
     AND ("The prefix is empty")
     AND ("The namespace is set")

  SCENARIO("Creating a non-default namespace attribute")
    GIVEN("A default namespace",
        [](context& ctx) {
            ctx.prefix = "lib";
            ctx.uri = mud::core::uri("http://library.example.org");
            ctx.attribute = mud::xml::dom::create_attribute("xmlns:lib");
            ctx.attribute->value(ctx.uri);
        })
    WHEN ("The attribute is examined")
    THEN ("The name is empty")
     AND ("The local-name is empty")
     AND ("The prefix is set")
     AND ("The namespace is set")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
