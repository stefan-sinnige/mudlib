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
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        a = b = result = 0;
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Calculator values */
    int a, b, result;
    std::vector<int> input;
END_CONTEXT()

FEATURE("Gherkin")

  // Pre-defined steps
  DEFINE_GIVEN("first value of 2", [](context& ctx) {
        ctx.a = 2; })
  DEFINE_GIVEN("second value of 3", [](context& ctx) {
        ctx.b = 3; })
  DEFINE_WHEN("calculating the sum", [](context& ctx) {
        ctx.result = ctx.a + ctx.b; })
  DEFINE_WHEN("calculating the product", [](context& ctx) {
        ctx.result = ctx.a * ctx.b; })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Addition (fixed numbers)")
    GIVEN ("first value of 2")
      AND ("second value of 3")
     WHEN ("calculating the sum")
     THEN ("the sum is 5", [](context& ctx) {
            ASSERT(ctx.result , 5); })

  SCENARIO("Multiplication (fixed numbers)")
    GIVEN ("first value of 2")
      AND ("second value of 3")
     WHEN ("calculating the product")
     THEN ("the product is 6", []( context& ctx) {
            ASSERT(ctx.result , 6); })

  SCENARIO("Addition")
    GIVEN ("first value of <first>", [](context& ctx) {
           ctx.a = ctx.sample<int>("first"); })
      AND ("second value of <second>", [](context& ctx) {
           ctx.b = ctx.sample<int>("second"); })
    WHEN  ("calculating the sum")
    THEN  ("the sum is <result>", [](context& ctx) {
            ASSERT(ctx.result, ctx.sample<int>("result")); })
    SAMPLES(int, int, int, std::string)
        HEADINGS("first", "second", "result", "comment")
        SAMPLE(0, 0, 0, "This is")
        SAMPLE(4, 0, 4, "a Gherkin")
        SAMPLE(0, 4, 4, "scenario")
        SAMPLE(5, 4, 9, "with samples")
    END_SAMPLES()

  SCENARIO("Average")
    GIVEN ("a list of values", [](context& ctx) {
           std::cout << "Data has " << ctx.data()->rows() << " rows" << std::endl;
           for (int i = 0; i < ctx.data()->rows(); ++i)
           {
               ctx.input.push_back(ctx.data<int>(i, "value"));
           }})
      DATA(int, std::string)
          HEADINGS("value", "comment")
          DATA_ROW(5, "First row")
          DATA_ROW(6, "Second row")
          DATA_ROW(7, "Third row")
      END_DATA()
    WHEN  ("calculating the average", [](context& ctx) {
               int total = 0;
               for(auto value : ctx.input) {
                   total += value;
               }
               ctx.result = total / ctx.input.size();
           })
    THEN  ("the average is 6", [](context& ctx) {
            ASSERT(ctx.result, 6); })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
