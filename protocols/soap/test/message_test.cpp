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

#include "mud/soap/message.h"
#include "mud/test.h"
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* A SOAP message. */
    std::unique_ptr<mud::soap::message> msg;
END_CONTEXT()

FEATURE("SOAP Message")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("A SOAP message",
      [](context& ctx){
          ctx.msg = std::make_unique<mud::soap::message>();
      });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("An SOAP Message type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::soap::message>::value);
        })
    THEN ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::soap::message>::value);
        })
    THEN ("The type is assignable",
        [](context& ctx) {
            ASSERT(true, std::is_assignable<
                  mud::soap::message,
                  mud::soap::message>::value);
        })

  SCENARIO("Default message")
    GIVEN("A SOAP message")
    WHEN ("A message field is queried", [](context&){})
    THEN ("The header is empty",
        [](context& ctx) {
            auto seq = ctx.msg->xml()->root()->elements("Header");
            ASSERT(1, seq.size());
            auto header = std::static_pointer_cast<mud::xml::element>(seq[0]);
            ASSERT(0, header->children().size());
        })
    AND  ("The body is empty",
        [](context& ctx) {
            auto seq = ctx.msg->xml()->root()->elements("Body");
            ASSERT(1, seq.size());
            auto body = std::static_pointer_cast<mud::xml::element>(seq[0]);
            ASSERT(0, body->children().size());
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
