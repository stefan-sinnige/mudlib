#include "mud/html/char_reference.h"
#include "mud/test.h"
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

    /* The escaped string */
    std::string escaped;

    /* The unescaped string */
    std::string unescaped;

    /* Any exception being thrown */
    size_t exception_hash;
    std::string exception_what;
END_CONTEXT()

FEATURE("Character Reference")
      DEFINE_WHEN ("The string is unescaped",
        [](context& ctx) {
            try {
               ctx.unescaped = mud::html::char_reference::unescape(ctx.escaped);
            }
            catch (std::exception& ex) {
                ctx.exception_hash = typeid(ex).hash_code();
                ctx.exception_what = ex.what();
            }
        })
      DEFINE_THEN("An ambiguous ampersand exception is thrown",
        [](context& ctx) {
            ASSERT(true, ctx.exception_hash != 0);
            ASSERT("Ambiguous ampersand", ctx.exception_what);
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Character Reference type traits")
    GIVEN("A char_reference type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::html::char_reference>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::html::char_reference>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::html::char_reference>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::html::char_reference>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::html::char_reference>::value);
        })

  SCENARIO("Unescaping named character references")
    GIVEN("A named character reference",
        [](context& ctx) {
            ctx.escaped = ctx.sample().entry<std::string>("ref");
        })
    WHEN ("The string is unescaped")
    THEN ("The result is the corresponding code point",
        [](context& ctx) {
            ASSERT(ctx.sample().entry<std::string>("code-point"),
                   ctx.unescaped);
        })
    SAMPLES("ref", "code-point")
        SAMPLE("&amp;", "\x26")
        SAMPLE("&lt;", "\x3C")
        SAMPLE("&gt;", "\x3E")
        SAMPLE("&quot;", "\x22")
        SAMPLE("&cdot;", "\xC4\x8B");
        SAMPLE("&awint;", "\xE2\xA8\x91");
        SAMPLE("&copf;", "\xF0\x9D\x95\x94");
    END_SAMPLES()

  SCENARIO("Unescaping decimal character references")
    GIVEN("A named character reference",
        [](context& ctx) {
            ctx.escaped = ctx.sample().entry<std::string>("ref");
        })
    WHEN ("The string is unescaped")
    THEN ("The result is the corresponding code point",
        [](context& ctx) {
            ASSERT(ctx.sample().entry<std::string>("code-point"),
                   ctx.unescaped);
        })
    SAMPLES("ref", "code-point")
        SAMPLE("&#38;", "\x26")
        SAMPLE("&#267;", "\xC4\x8B");
        SAMPLE("&#10769;", "\xE2\xA8\x91")
        SAMPLE("&#120148;", "\xF0\x9D\x95\x94");
    END_SAMPLES()

  SCENARIO("Unescaping hexadecimal character references")
    GIVEN("A named character reference",
        [](context& ctx) {
            ctx.escaped = ctx.sample().entry<std::string>("ref");
        })
    WHEN ("The string is unescaped")
    THEN ("The result is the corresponding code point",
        [](context& ctx) {
            ASSERT(ctx.sample().entry<std::string>("code-point"),
                   ctx.unescaped);
        })
    SAMPLES("ref", "code-point")
        SAMPLE("&#x26;", "\x26")
        SAMPLE("&#x10B;", "\xC4\x8B");
        SAMPLE("&#x2A11;", "\xE2\xA8\x91")
        SAMPLE("&#x1D554;", "\xF0\x9D\x95\x94");
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
