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

#include "mud/core/bufferstream.h"
#include "mud/test.h"
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Convert from string to buffer */
    mud::core::buffer to_buffer(const std::string& s) {
        mud::core::buffer d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from buffer to string */
    std::string to_string(const mud::core::buffer& b) {
        std::stringstream sstr;
        sstr << b;
        return sstr.str();
    }

    /* Buffer and stream objects */
    mud::core::buffer buffer;
    mud::core::bufferstream stream;
    mud::core::ibufferstream istream;
    mud::core::obufferstream ostream;
END_CONTEXT()

FEATURE("Binary Buffer Streams")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits of ibufferstream")
    GIVEN("An ibufferstream type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::ibufferstream>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::ibufferstream>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::ibufferstream>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::ibufferstream>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::ibufferstream>::value);
        })

  SCENARIO("Type traits of obufferstream")
    GIVEN("An obufferstream type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::obufferstream>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::obufferstream>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::obufferstream>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::obufferstream>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::obufferstream>::value);
        })

  SCENARIO("Type traits of bufferstream")
    GIVEN("An bufferstream type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::bufferstream>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::bufferstream>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::bufferstream>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::bufferstream>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::bufferstream>::value);
        })

  SCENARIO("An empty input bufferstream can be read")
    GIVEN("An empty input bufferstream", [](context& ctx) { })
    WHEN ("The input stream is read", [](context& ctx) { })
    THEN ("No data is read", [](context& ctx) {
            ASSERT(std::char_traits<char>::eof(), ctx.istream.get());
            ASSERT(false, ctx.istream.good());
            ASSERT(false, ctx.istream.bad());
            ASSERT(true, ctx.istream.eof());
            ASSERT(true, ctx.istream.fail());
        })

  SCENARIO("An input bufferstream can read all bytes in a buffer")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("010203040506070809");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("The input stream is fully read", [](context& ctx) { })
    THEN ("The full buffer is read", [](context& ctx) {
            char ch;
            int i = 0;
            while (ctx.istream.get(ch)) {
                ++i;
                ASSERT(i, ch);
            }
            ASSERT(i, 9);
            ASSERT(false, ctx.istream.good());
            ASSERT(false, ctx.istream.bad());
            ASSERT(true, ctx.istream.eof());
            ASSERT(true, ctx.istream.fail());
        })

  SCENARIO("An input bufferstream can be assigned a different buffer")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("0123456789abcdef");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("The different buffer is assigned", [](context& ctx) {
            auto buffer = ctx.to_buffer("010203040506070809");
            ctx.istream.buf(std::move(buffer));
        })
     AND ("The input stream is fully read", [](context& ctx) { })
    THEN ("The new buffer is read", [](context& ctx) {
            char ch;
            int i = 0;
            while (ctx.istream.get(ch)) {
                ++i;
                ASSERT(i, ch);
            }
            ASSERT(i, 9);
            ASSERT(false, ctx.istream.good());
            ASSERT(false, ctx.istream.bad());
            ASSERT(true, ctx.istream.eof());
            ASSERT(true, ctx.istream.fail());
        })

  SCENARIO("An input bufferstream can unget a character")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("0123456789abcdef");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("A character is extracted and made available again",
        [](context& ctx) {
            ctx.istream.get();
            ctx.istream.unget();
        })
    THEN ("The next character is the original one", [](context& ctx) {
            char ch;
            ctx.istream.get(ch);
            ASSERT(1, ch);
        })

  SCENARIO("An input bufferstream can unget multiple characters")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("0123456789abcdef");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("A number of characters are extracted and made available again",
        [](context& ctx) {
            for (int i = 0; i < 6; ++i) {
                ctx.istream.get();
            }
            for (int i = 0; i < 3; ++i) {
                ctx.istream.unget();
            }
        })
    THEN ("The next character is the onext in line", [](context& ctx) {
            char ch;
            ctx.istream.get(ch);
            ASSERT(0x67, ch);
        })

  SCENARIO("An input bufferstream can put-back the same charactes")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("0123456789abcdef");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("A character is extracted and put back",
        [](context& ctx) {
            char ch;
            ctx.istream.get(ch);
            ctx.istream.putback(ch);
        })
    THEN ("The next character is the replaced one", [](context& ctx) {
            char ch;
            ctx.istream.get(ch);
            ASSERT(0x01, ch);
        })

  SCENARIO("An input bufferstream cannot put-back a different same charactes")
    GIVEN("An initialised bufferstream", [](context& ctx) { 
            auto buffer = ctx.to_buffer("0123456789abcdef");
            ctx.istream = mud::core::ibufferstream(std::move(buffer));
        })
    WHEN ("A character is extracted", [](context& ctx) {
            char ch;
            ctx.istream.get(ch);
        })
    THEN ("A different character cannot be put back", [](context& ctx) {
            char ch = 0xCC;
            ASSERT(false, ctx.istream.putback(ch).good());
        })

  SCENARIO("An empty output bufferstream can be retrieved")
    GIVEN("An output bufferstream", [](context& ctx) { })
    WHEN ("The no data is written to the buffer", [](context& ctx) { })
    THEN ("An empty buffer can be retrieved", [](context& ctx) {
            auto result = ctx.ostream.buf();
            ASSERT(0, result.size());
        })

  SCENARIO("An output bufferstream can be written to by string")
    GIVEN("An empty output bufferstream", [](context& ctx) { })
    WHEN ("The data is written to the buffer", [](context& ctx) {
            ctx.ostream << "Hello";
        })
    THEN ("A buffer with the same data can be retrieved", [](context& ctx) {
            auto result = ctx.ostream.buf();
            ASSERT(5, result.size());
            ASSERT('H', result[0]); ASSERT('e', result[1]);
            ASSERT('l', result[2]); ASSERT('l', result[3]);
            ASSERT('o', result[4]);
        })

  SCENARIO("An output bufferstream can be written to sequentially")
    GIVEN("An empty output bufferstream", [](context& ctx) { })
    WHEN ("The data is written to the buffer", [](context& ctx) {
            for (int i = 0; i < 1024; ++i) {
                ctx.ostream.put(i);
            }
        })
    THEN ("A buffer with the same data can be retrieved", [](context& ctx) {
            auto result = ctx.ostream.buf();
            ASSERT(1024, result.size());
            for (int i = 0; i < 1024; ++i) {
                ASSERT(i & 0xFF, result[i]);
            }
        })

  SCENARIO("An output bufferstream can be written to by string")
    GIVEN("An empty output bufferstream", [](context& ctx) { })
    WHEN ("The data is written to the buffer", [](context& ctx) {
            ctx.ostream << "Hello";
        })
    THEN ("A buffer with the same data can be retrieved", [](context& ctx) {
            auto result = ctx.ostream.buf();
            ASSERT(5, result.size());
            ASSERT('H', result[0]); ASSERT('e', result[1]);
            ASSERT('l', result[2]); ASSERT('l', result[3]);
            ASSERT('o', result[4]);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
