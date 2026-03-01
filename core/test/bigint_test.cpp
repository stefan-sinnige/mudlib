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

#include "mud/core/bigint.h"
#include "mud/test.h"
#include <memory>
#include <ranges>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* A size value */
    size_t size;

    /* A big integer value */
    mud::core::bigint value;

    /* Big integer values. */
    mud::core::bigint op1, op2;

    /* A saved exception */
    std::exception_ptr eptr;
END_CONTEXT()

FEATURE("Big Integer")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Big Integer type traits")
    GIVEN("A big integer type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::bigint>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::bigint>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::bigint>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::bigint>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::bigint>::value);
        })

  SCENARIO("Creation from built-in C++ integer types")
    GIVEN("A bigint created from a built-in type",
        [](context& ctx) {
            // Cannot rely on the number of bits being used as that depends on
            // the underlying CPU architecture of the numbering scheme. Most
            // are 2's complement, but is not guaranteed. What is guaranteed is
            // the maximum number of bits though.
            auto type_name = ctx.sample<std::string>("type-name");
            if (type_name == "int8_t") {
                int8_t min_value = static_cast<int8_t>(
                        ctx.sample<int64_t>("min"));
                int8_t max_value = static_cast<int8_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "uint8_t") {
                uint8_t min_value = static_cast<uint8_t>(
                        ctx.sample<int64_t>("min"));
                uint8_t max_value = static_cast<uint8_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "int16_t") {
                int16_t min_value = static_cast<int16_t>(
                        ctx.sample<int64_t>("min"));
                int16_t max_value = static_cast<int16_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "uint16_t") {
                uint16_t min_value = static_cast<uint16_t>(
                        ctx.sample<int64_t>("min"));
                uint16_t max_value = static_cast<uint16_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "int32_t") {
                int32_t min_value = static_cast<int32_t>(
                        ctx.sample<int64_t>("min"));
                int32_t max_value = static_cast<int32_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "uint32_t") {
                uint32_t min_value = static_cast<uint32_t>(
                        ctx.sample<int64_t>("min"));
                uint32_t max_value = static_cast<uint32_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "int64_t") {
                int64_t min_value = static_cast<int64_t>(
                        ctx.sample<int64_t>("min"));
                int64_t max_value = static_cast<int64_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else
            if (type_name == "uint64_t") {
                uint64_t min_value = static_cast<uint64_t>(
                        ctx.sample<int64_t>("min"));
                uint64_t max_value = static_cast<uint64_t>(
                        ctx.sample<uint64_t>("max"));
                ctx.op1 = mud::core::bigint(min_value);
                ctx.op2 = mud::core::bigint(max_value);
            }
            else {
                mud::test::AssertFailed(__FILE__, __LINE__,
                    "Unsupported type-name");
            }
        })
    WHEN ("The bigint is examined", [](context& ctx) {})
    THEN ("The bit-size is equal or smaller to the size of the built-in type",
        [](context& ctx) {
            ASSERT(ctx.op1.bits() <= ctx.sample<std::size_t>("bits"), true);
            ASSERT(ctx.op2.bits() <= ctx.sample<std::size_t>("bits"), true);
        })
     AND ("The minimum and maximum values can be used",
        [](context& ctx) {
            std::stringstream min_str, max_str, op1_str, op2_str;
            auto type_name = ctx.sample<std::string>("type-name");
            if (type_name == "int8_t") {
                min_str << static_cast<int>(ctx.sample<int64_t>("min"));
                max_str << static_cast<int>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "uint8_t") {
                min_str << static_cast<int>(ctx.sample<int64_t>("min"));
                max_str << static_cast<int>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "int16_t") {
                min_str << static_cast<int16_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<int16_t>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "uint16_t") {
                min_str << static_cast<uint16_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<uint16_t>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "int32_t") {
                min_str << static_cast<int32_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<int32_t>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "uint32_t") {
                min_str << static_cast<uint32_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<uint32_t>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "int64_t") {
                min_str << static_cast<int64_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<int64_t>(ctx.sample<uint64_t>("max"));
            }
            else
            if (type_name == "uint64_t") {
                min_str << static_cast<uint64_t>(ctx.sample<int64_t>("min"));
                max_str << static_cast<uint64_t>(ctx.sample<uint64_t>("max"));
            }
            op1_str << ctx.op1;
            op2_str << ctx.op2;
            ASSERT(op1_str.str(), min_str.str());
            ASSERT(op2_str.str(), max_str.str());
        })
    SAMPLES(std::string, std::size_t, int64_t, uint64_t)
        HEADINGS("type-name", "bits", "min", "max")
        SAMPLE("int8_t",    8,  INT8_MIN,   INT8_MAX)
        SAMPLE("uint8_t",   8,         0,  UINT8_MAX)
        SAMPLE("int16_t",  16, INT16_MIN,  INT16_MAX)
        SAMPLE("uint16_t", 16,         0, UINT16_MAX)
        SAMPLE("int32_t",  32, INT32_MIN,  INT32_MAX)
        SAMPLE("uint32_t", 32,         0, UINT32_MAX)
        SAMPLE("int64_t",  64, INT64_MIN,  INT64_MAX)
        SAMPLE("uint64_t", 64,         0, UINT64_MAX)
    END_SAMPLES()

  SCENARIO("Creation from binary array")
    GIVEN("A bigint created from a binary array",
        [](context& ctx) {
            ctx.value = mud::core::bigint(
                    ctx.sample<const uint8_t*>("array"),
                    ctx.sample<size_t>("size"));
        })
    WHEN ("The bigint is examined", [](context& ctx) {})
    THEN ("The value corresponds to an unsigned integer",
        [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.value;
            ASSERT(ctx.sample<std::string>("value"), sstr.str());
        })
    SAMPLES(const uint8_t*, std::size_t, std::string)
        HEADINGS("array", "size", "value")
        SAMPLE((const uint8_t*)nullptr, 0, "0")
        SAMPLE((const uint8_t*)"", 0, "0")
        SAMPLE((const uint8_t*)"\x01", 1, "1")
        SAMPLE((const uint8_t*)"\xFF", 1, "255")
        SAMPLE((const uint8_t*)"\x80\xFF", 2, "33023")
        SAMPLE((const uint8_t*)"\xCC\x78\x5E\x16\xFE\x19\x36\xD4", 8,
             "14733629633787934420");
        SAMPLE((const uint8_t*)
             "\x1B\x30\x96\x4E\xC3\x95\xDC\x24\x06\x95\x28\xD5\x4B\xBD\xA4\x0D"
             "\x16\xE9\x66\xEF\x9A\x70\xEB\x21\xB5\xB2\x94\x3A\x32\x1C\xDF\x10"
             "\x39\x17\x45\x57\x0C\xCA\x94\x20\xC6\xEC\xB3\xB7\x2E\xD2\xEE\x8B"
             "\x02\xEA\x27\x35\xC6\x1A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
             "\x00\x00", 66,
             "9332621544394415268169923885626670049071596826438162146859296389"
             "5217599993229915608941463976156518286253697920827223758251185210"
             "916864000000000000000000000000") // 100!
    END_SAMPLES()

  SCENARIO("Creation from decimal string")
    GIVEN("A bigint created from a decimal string",
        [](context& ctx) {
            ctx.value = mud::core::bigint(ctx.sample<std::string>("decimal"));
        })
    WHEN ("The bigint is examined", [](context& ctx) {})
    THEN ("The value corresponds to an unsigned integer",
        [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.value;
            ASSERT(ctx.sample<std::string>("decimal"), sstr.str());
            ASSERT(ctx.sample<size_t>("size"), ctx.value.size());
            ASSERT(0, memcmp(ctx.sample<const uint8_t*>("value"),
                             ctx.value.data(), ctx.value.size()));
        })
    SAMPLES(std::string, size_t, const uint8_t*)
        HEADINGS("decimal", "size", "value")
        SAMPLE(   "0",  1, (const uint8_t*)"\x00")
        SAMPLE( "243",  1, (const uint8_t*)"\xF3")
        SAMPLE( "256",  2, (const uint8_t*)"\x01\x00")
        SAMPLE("-256",  2, (const uint8_t*)"\x01\x00")
        SAMPLE("33023", 2, (const uint8_t*)"\x80\xFF")
        SAMPLE("14733629633787934420", 8, (const uint8_t*)
             "\xCC\x78\x5E\x16\xFE\x19\x36\xD4")
        SAMPLE(
             "9332621544394415268169923885626670049071596826438162146859296389"
             "5217599993229915608941463976156518286253697920827223758251185210"
             "916864000000000000000000000000", 66, (const uint8_t*) // 100!
             "\x1B\x30\x96\x4E\xC3\x95\xDC\x24\x06\x95\x28\xD5\x4B\xBD\xA4\x0D"
             "\x16\xE9\x66\xEF\x9A\x70\xEB\x21\xB5\xB2\x94\x3A\x32\x1C\xDF\x10"
             "\x39\x17\x45\x57\x0C\xCA\x94\x20\xC6\xEC\xB3\xB7\x2E\xD2\xEE\x8B"
             "\x02\xEA\x27\x35\xC6\x1A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
             "\x00\x00")
    END_SAMPLES()

  SCENARIO("Unary plus and minus on big integers")
    GIVEN("A big integer",
        [](context& ctx) {
            ctx.value = mud::core::bigint(ctx.sample<std::string>("value"));
        })
    WHEN ("When the unary operations are applied",
        [](context& ctx) {
            for (char op: std::views::reverse(ctx.sample<std::string>("unary")))
            {
                if (op == '-') {
                    ctx.value = -ctx.value;
                }
                else
                if (op == '+') {
                    ctx.value = +ctx.value;
                }
            }
        })
    THEN ("The result is the correct applied unary",
        [](context& ctx) {
            std::stringstream result;
            result << ctx.value;
            ASSERT(ctx.sample<std::string>("result"), result.str());
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("value", "unary", "result")
        SAMPLE( "123",  "+",  "123")
        SAMPLE( "123",  "-", "-123")
        SAMPLE( "123", "+-", "-123")
        SAMPLE( "123", "-+", "-123")
        SAMPLE( "123", "--",  "123")
        SAMPLE("-321",  "+", "-321")
        SAMPLE("-321",  "-",  "321")
        SAMPLE("-321", "+-",  "321")
        SAMPLE("-321", "-+",  "321")
        SAMPLE("-321", "--", "-321")
    END_SAMPLES()

  SCENARIO("Subtraction of big integers")
    GIVEN("Two big integer numbers",
        [](context& ctx) {
            ctx.op1 = mud::core::bigint(ctx.sample<std::string>("minuend"));
            ctx.op2 = mud::core::bigint(ctx.sample<std::string>("subtrahend"));
        })
    WHEN ("When the integers are subtracted",
        [](context& ctx) {
            ctx.value = ctx.op1 - ctx.op2;
        })
    THEN ("The result is the arithmetic difference",
        [](context& ctx) {
            std::stringstream result;
            result << ctx.value;
            ASSERT(ctx.sample<std::string>("result"), result.str());
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("minuend", "subtrahend", "result")
        SAMPLE(    "0",    "0",     "0")
        SAMPLE(  "251",    "8",   "243") // no borrow
        SAMPLE(  "320",    "1",   "319") // borrow from next byte
        SAMPLE(  "256",    "1",   "255") // borrow from next byte - to zero
        SAMPLE("65536",    "1", "65535") // borrow from next byte - ripple
        SAMPLE( "-658",  "257",  "-915") // Result negative
        SAMPLE( "-852", "-678",  "-174") // Result negative
        SAMPLE( "-371", "-587",   "216") // Result positive
        SAMPLE("92738174616395816581613",
               "18471465914659125813850",
               "74266708701736690767763")
    END_SAMPLES()

  SCENARIO("Multiplication of big integers")
    GIVEN("Two big integer numbers",
        [](context& ctx) {
           ctx.op1 = mud::core::bigint(ctx.sample<std::string>("multiplicant"));
           ctx.op2 = mud::core::bigint(ctx.sample<std::string>("multiplier"));
        })
    WHEN ("When the integers are subtracted",
        [](context& ctx) {
            ctx.value = ctx.op1 * ctx.op2;
        })
    THEN ("The result is the arithmetic multiplication",
        [](context& ctx) {
            std::stringstream result;
            result << ctx.value;
            ASSERT(ctx.sample<std::string>("result"), result.str());
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("multiplicant", "multiplier", "result")
        SAMPLE(    "0",    "0",      "0")
        SAMPLE(   "87",    "0",      "0")
        SAMPLE(    "0",   "98",      "0")
        SAMPLE(  "283",  "271",  "76693")
        SAMPLE(  "271",  "283",  "76693")
        SAMPLE( "-138",   "52",  "-7176") // Result negatuve
        SAMPLE(  "203",  "-73", "-14819") // Result negative
        SAMPLE( "-267",  "-47",  "12549") // Result positive
        SAMPLE("92738174616395816581613",
               "18471465914659125813850",
               "1713010031414461476338455832797372993570740050")
    END_SAMPLES()

  SCENARIO("Division of big integers (quotient)")
    GIVEN("Two big integer numbers",
        [](context& ctx) {
           ctx.op1 = mud::core::bigint(ctx.sample<std::string>("dividend"));
           ctx.op2 = mud::core::bigint(ctx.sample<std::string>("divisor"));
        })
    WHEN ("When the integers are divided",
        [](context& ctx) {
            ctx.value = ctx.op1 / ctx.op2;
        })
    THEN ("The result is the arithmetic division with a quotient part",
        [](context& ctx) {
            std::stringstream result;
            result << ctx.value;
            ASSERT(ctx.sample<std::string>("result"), result.str());
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("dividend", "divisor", "result")
        SAMPLE(    "0",  "871",      "0")
        SAMPLE(  "657",  "761",      "0")
        SAMPLE( "1980",   "83",     "23")
        SAMPLE( "3395",   "35",     "97")
        SAMPLE( "-257",    "5",    "-51") // Result negative
        SAMPLE(  "684",  "-13",    "-52") // Result negative
        SAMPLE("92738174616395816581613",
               "18471465914659125813850",
               "5")
    END_SAMPLES()

  SCENARIO("Division of big integers (remainder)")
    GIVEN("Two big integer numbers",
        [](context& ctx) {
           ctx.op1 = mud::core::bigint(ctx.sample<std::string>("dividend"));
           ctx.op2 = mud::core::bigint(ctx.sample<std::string>("divisor"));
        })
    WHEN ("When the integers are divided",
        [](context& ctx) {
            ctx.value = ctx.op1 % ctx.op2;
        })
    THEN ("The result is the arithmetic division with a remainder part",
        [](context& ctx) {
            std::stringstream result;
            result << ctx.value;
            ASSERT(ctx.sample<std::string>("result"), result.str());
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("dividend", "divisor", "result")
        SAMPLE(    "0",  "871",      "0")
        SAMPLE(  "657",  "761",    "657")
        SAMPLE( "1980",   "83",     "71")
        SAMPLE( "3395",   "35",      "0")
        SAMPLE( "-257",    "5",      "2") // Result positive
        SAMPLE(  "684",  "-13",      "8") // Result positive
        SAMPLE("92738174616395816581613",
               "18471465914659125813850",
               "380845043100187512363")
    END_SAMPLES()

  SCENARIO("Division by zero raises exception")
    GIVEN("Two big integers with one of them 0",
        [](context& ctx) {
           ctx.op1 = 192;
           ctx.op2 = 0;
        })
    WHEN ("The integer division is a division by zero",
        [](context& ctx) {
            try {
                ctx.value = ctx.op1 / ctx.op2;
            }
            catch (...) {
                ctx.eptr = std::current_exception();
            }
        })
    THEN ("A std::overflow_error exception is raised",
        [](context& ctx) {
            ASSERT_THROW(std::overflow_error,
                         std::rethrow_exception(ctx.eptr));
        })
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
