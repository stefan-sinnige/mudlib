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
    mud::crypto::data_t to_data(const std::string& s) {
        mud::crypto::data_t d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from data to string */
    std::string to_string(const mud::crypto::data_t& b) {
        std::stringstream sstr;
        sstr << b;
        return sstr.str();
    }

    /* Data objects */
    mud::crypto::data_t lhs;
    mud::crypto::data_t rhs;
    mud::crypto::data_t result;

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

  SCENARIO("Type traits")
    GIVEN("An data_t type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::crypto::data_t>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::crypto::data_t>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::crypto::data_t>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::crypto::data_t>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::crypto::data_t>::value);
        })

  SCENARIO("A data object can be padded to the begin")
    GIVEN("A data object", [](context& ctx) {
            ctx.result = ctx.to_data(ctx.sample<std::string>("data"));
        })
    WHEN ("Tha object is padded to the begin", [](context& ctx) {
            size_t sz = ctx.sample<size_t>("size");
            uint8_t val = ctx.sample<uint8_t>("value");
            ctx.result.pad_begin(sz, val);
        })
    THEN ("The result is the padded", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, size_t, uint8_t, std::string)
        HEADINGS("data", "size", "value", "result")
        SAMPLE(            "", 4, 0x00,     "00000000")
        SAMPLE(        "dead", 4, 0x00,     "0000dead")
        SAMPLE(        "dead", 4, 0x3b,     "3b3bdead")
        SAMPLE(    "deadbeef", 4, 0x00,     "deadbeef")
        SAMPLE("babedeadbeef", 4, 0x00, "babedeadbeef")
    END_SAMPLES()

  SCENARIO("A data object can be padded to the end")
    GIVEN("A data object", [](context& ctx) {
            ctx.result = ctx.to_data(ctx.sample<std::string>("data"));
        })
    WHEN ("Tha object is padded to the end", [](context& ctx) {
            size_t sz = ctx.sample<size_t>("size");
            uint8_t val = ctx.sample<uint8_t>("value");
            ctx.result.pad_end(sz, val);
        })
    THEN ("The result is the padded", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, size_t, uint8_t, std::string)
        HEADINGS("data", "size", "value", "result")
        SAMPLE(            "", 4, 0x00,     "00000000")
        SAMPLE(        "dead", 4, 0x00,     "dead0000")
        SAMPLE(        "dead", 4, 0x3b,     "dead3b3b")
        SAMPLE(    "deadbeef", 4, 0x00,     "deadbeef")
        SAMPLE("babedeadbeef", 4, 0x00, "babedeadbeef")
    END_SAMPLES()

  SCENARIO("A data object can be bit-wise OR'd")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_data(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_data(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The objects are OR'd", [](context& ctx) {
            ctx.result = ctx.lhs | ctx.rhs;
        })
    THEN ("The result is the OR'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("00000000",     "deadbeef", "deadbeef")
        SAMPLE("deadbeef",     "00000000", "deadbeef")
        SAMPLE("dead0000",     "0000beef", "deadbeef")
        SAMPLE("dead0000", "0000beef1234", "deadbeef")
    END_SAMPLES()

  SCENARIO("A data object can be bit-wise AND'd")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_data(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_data(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The objects are AND'd", [](context& ctx) {
            ctx.result = ctx.lhs & ctx.rhs;
        })
    THEN ("The result is the AND'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("dead0000",     "0000beef", "00000000")
        SAMPLE("deadbeef",     "babecafe", "9aac8aee")
        SAMPLE("deadbeef", "babecafe1234", "9aac8aee")
    END_SAMPLES()

  SCENARIO("A data object can be bit-wise XOR'd")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_data(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_data(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The objects are XOR'd", [](context& ctx) {
            ctx.result = ctx.lhs ^ ctx.rhs;
        })
    THEN ("The result is the XOR'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("dead0000",     "0000beef", "deadbeef")
        SAMPLE("deadbeef",     "babecafe", "64137411")
        SAMPLE("deadbeef", "babecafe1234", "64137411")
    END_SAMPLES()

  SCENARIO("A data object can be bit-wise shifted-left")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_data(ctx.sample<std::string>("lhs"));
            ctx.pos = ctx.sample<size_t>("pos");
        })
    WHEN ("The objects are shifted", [](context& ctx) {
            ctx.result = ctx.lhs << ctx.pos;
        })
    THEN ("The result is the shifted", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("lhs", "pos", "result")
        SAMPLE("deadbeef",  0, "deadbeef")
        SAMPLE("deadbeef",  1, "bd5b7dde")
        SAMPLE("deadbeef",  2, "7ab6fbbc")
        SAMPLE("deadbeef",  3, "f56df778")
        SAMPLE("deadbeef",  4, "eadbeef0")
        SAMPLE("deadbeef",  8, "adbeef00")
        SAMPLE("deadbeef",  9, "5b7dde00")
        SAMPLE("deadbeef", 10, "b6fbbc00")
        SAMPLE("deadbeef", 11, "6df77800")
        SAMPLE("deadbeef", 12, "dbeef000")
        SAMPLE("deadbeef", 16, "beef0000")
        SAMPLE("deadbeef", 27, "78000000")
        SAMPLE("deadbeef", 28, "f0000000")
        SAMPLE("deadbeef", 31, "80000000")
        SAMPLE("deadbeef", 32, "00000000")
        SAMPLE("deadbeef", 64, "00000000")
    END_SAMPLES()

  SCENARIO("A data object can be bit-wise shifted-right")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_data(ctx.sample<std::string>("lhs"));
            ctx.pos = ctx.sample<size_t>("pos");
        })
    WHEN ("The objects are shifted", [](context& ctx) {
            ctx.result = ctx.lhs >> ctx.pos;
        })
    THEN ("The result is the shifted", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.result));
        })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("lhs", "pos", "result")
        SAMPLE("deadbeef",  0, "deadbeef")
        SAMPLE("deadbeef",  1, "6f56df77")
        SAMPLE("deadbeef",  2, "37ab6fbb")
        SAMPLE("deadbeef",  3, "1bd5b7dd")
        SAMPLE("deadbeef",  4, "0deadbee")
        SAMPLE("deadbeef",  8, "00deadbe")
        SAMPLE("deadbeef",  9, "006f56df")
        SAMPLE("deadbeef", 10, "0037ab6f")
        SAMPLE("deadbeef", 11, "001bd5b7")
        SAMPLE("deadbeef", 12, "000deadb")
        SAMPLE("deadbeef", 16, "0000dead")
        SAMPLE("deadbeef", 27, "0000001b")
        SAMPLE("deadbeef", 28, "0000000d")
        SAMPLE("deadbeef", 31, "00000001")
        SAMPLE("deadbeef", 32, "00000000")
        SAMPLE("deadbeef", 64, "00000000")
    END_SAMPLES()

  SCENARIO("A counter object can be incremented")
    GIVEN("An counter object initialised with a value", [](context& ctx) {
            ctx.counter = ctx.to_data(ctx.sample<std::string>("initial"));
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
