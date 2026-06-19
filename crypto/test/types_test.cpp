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

#include "mud/crypto/types.h"
#include "mud/test.h"
#include <sstream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Convert from string to data */
    mud::core::buffer to_buffer(const std::string& s) {
        mud::core::buffer d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from data to string */
    std::string to_string(const mud::core::buffer& b) {
        std::stringstream sstr;
        sstr << b;
        return sstr.str();
    }

    /* Data objects */
    mud::core::buffer lhs;
    mud::core::buffer rhs;
    mud::core::buffer result;

    /* A position */
    size_t pos;

    /* A counter object */
    mud::crypto::counter_t counter;
END_CONTEXT()

FEATURE("Cryptographic Types")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("A counter object can be incremented")
    GIVEN("An counter object initialised with a value", [](context& ctx) {
            ctx.counter = ctx.to_buffer(ctx.sample<std::string>("initial"));
        })
    WHEN ("The counter is incremented", [](context& ctx) {
            ++ctx.counter;
        })
    THEN ("The counter object's value has increased by one", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.counter));
        })
    SAMPLES(std::string, std::string)
        HEADINGS("initial", "result")
        SAMPLE("00000000", "00000001")
        SAMPLE("12345678", "12345679")
        SAMPLE("000000FF", "00000100")  // Carry over to next byte
        SAMPLE("FFFFFFFF", "00000000")  // Wrap around
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
