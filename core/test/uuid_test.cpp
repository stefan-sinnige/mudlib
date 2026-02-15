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

#include "mud/core/uuid.h"
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

    /* The UUID values */
    mud::core::uuid uuid;

    /* A string */
    std::string str;
END_CONTEXT()

FEATURE("UUID")
  DEFINE_GIVEN("A UUID object", [](context& ctx){
  })
  DEFINE_WHEN("The UUID is examined", [](context& ctx){
  })
  DEFINE_WHEN("The UUID is represented from a string", [](context& ctx){
      ctx.uuid = mud::core::uuid(ctx.str);
  })
  DEFINE_WHEN("The UUID is represented as a string", [](context& ctx){
      ctx.str = ctx.uuid.str();
  })
  DEFINE_THEN("The UUID is not a null value", [](context& ctx){
      ASSERT(false, ctx.uuid.null());
  })
  DEFINE_THEN("The UUID is a null value", [](context& ctx){
      ASSERT(true, ctx.uuid.null());
  })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("UUID type traits")
    GIVEN("A UUID type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::uuid>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::uuid>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::uuid>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::uuid>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::uuid>::value);
        })

  SCENARIO("Creation of a UUID")
    GIVEN("A UUID object")
    WHEN("The UUID is examined")
    THEN("The UUID is not a null value")

  SCENARIO("Reading a valid UUID")
    GIVEN("A string representation of a UUID", [](context& ctx){
        ctx.str = "032DB337-8E24-4F9C-835D-0A21F4719818";
    })
    WHEN("The UUID is represented from a string")
    THEN("The UUID represents the same value", [](context& ctx){
        ASSERT(std::string("032DB337-8E24-4F9C-835D-0A21F4719818"),
               ctx.uuid.str());
    })

  SCENARIO("Reading a valid UUID with mixed camel-case")
    GIVEN("A string representation of a UUID", [](context& ctx){
        ctx.str = "032Db337-8E24-4f9C-835d-0A21f4719818";
    })
    WHEN("The UUID is represented from a string")
    THEN("The UUID represents the same value", [](context& ctx){
        ASSERT(std::string("032DB337-8E24-4F9C-835D-0A21F4719818"),
               ctx.uuid.str());
    })

  SCENARIO("Reading an invalid UUID")
    GIVEN("A random strin", [](context& ctx){
        ctx.str = "This is some string that is not a UUID";
    })
    WHEN("The UUID is represented from a string")
    THEN("The UUID is a null value")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
