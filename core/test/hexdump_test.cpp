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

#include "mud/core/hexdump.h"
#include "mud/test.h"
#include <sstream>
#include <string>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The data */
    const char* input_data;
    size_t input_size;

    /* The enerated output */
    std::string output;
END_CONTEXT()

FEATURE("Hexadecimal Dump")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Dump output of a binary buffer")
    GIVEN("A buffer with data", [](context& ctx) {
        ctx.input_data = ctx.sample<const char*>("data");
        ctx.input_size = ctx.sample<size_t>("size");
      })
    WHEN ("The buffer is dumped", [](context& ctx) {
        std::stringstream sstr;
        sstr << mud::core::hexdump(ctx.input_data, ctx.input_size);
        ctx.output = sstr.str();
      })
    THEN ("The buffer is output correctly", [](context& ctx) {
        auto expected = ctx.sample<std::string>("output");
        ASSERT(expected, ctx.output);
      })
    SAMPLES(const char*, size_t, std::string)
      HEADINGS("data", "size", "output")
      SAMPLE("", 0,
"0000                                                                    ")
      SAMPLE("\x00", 1,
"0000  00                                                .               ")
      SAMPLE("\x1e\x1f\x20\x21", 4,
"0000  1e 1f 20 21                                       .. !            ")
      SAMPLE("aAbBcCdD", 8,
"0000  61 41 62 42 63 43 64 44                           aAbBcCdD        ")
      SAMPLE("0123456789abcdef", 16,
"0000  30 31 32 33 34 35 36 37  38 39 61 62 63 64 65 66  0123456789abcdef")
      SAMPLE("0123456789abcdefghijklmnopqrstuvwxyz", 36,
"0000  30 31 32 33 34 35 36 37  38 39 61 62 63 64 65 66  0123456789abcdef\n" 
"0010  67 68 69 6a 6b 6c 6d 6e  6f 70 71 72 73 74 75 76  ghijklmnopqrstuv\n" 
"0020  77 78 79 7a                                       wxyz            ")
   END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
