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

#include "mud/core/buffer.h"
#include "mud/test.h"
#include <sstream>
#include <type_traits>

using endian_t = mud::core::endian::endian_t;

/* clang-format off */

/* Fake message (TLV structure):
 *    01 0c
 *       02 06 112233445566
 *       03 02 aabb
 */
char sample_msg[] = "010c02061122334455660302aabb";

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

    /* Convert from buffer view to string */
    std::string to_string(const mud::core::buffer_view& b) {
        std::stringstream sstr;
        sstr << b;
        return sstr.str();
    }

    /* Buffer objects */
    mud::core::buffer lhs;
    mud::core::buffer rhs;
    mud::core::buffer buffer;

    /* Buffer view objects */
    mud::core::buffer_view view;
    mud::core::buffer_view sub_view;

    /* A position */
    size_t pos;
    mud::core::buffer::iterator iter;
    mud::core::buffer::const_iterator citer;

    /* Integral values */
    uint16_t ui16;
END_CONTEXT()

FEATURE("Binary Buffer")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An buffer type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::buffer>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::buffer>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::buffer>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::buffer>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::buffer>::value);
        })

  SCENARIO("An empty buffer can be created")
    GIVEN("An empty buffer", [](context& ctx) {
            ctx.buffer = mud::core::buffer(0);
        })
    WHEN ("The buffers is examined", [](context& ctx) { })
    THEN ("The buffers is empty", [](context& ctx) {
            ASSERT(0, ctx.buffer.size());
        })

  SCENARIO("A buffer with an initial size can be created")
    GIVEN("A buffer with an initial size", [](context& ctx) {
            ctx.buffer = mud::core::buffer(256);
        })
    WHEN ("The buffers is examined", [](context& ctx) { })
    THEN ("The buffers is empty", [](context& ctx) {
            ASSERT(256, ctx.buffer.size());
            for (int i = 0; i < 256; ++i) {
                ASSERT(0, ctx.buffer[i]);
            }
        })

  SCENARIO("A buffer can be created from a hex string")
    GIVEN("A buffer created from a hex string", [](context& ctx) {
            const char* msg = "0123456789abcdeffedcba9876543210"
                              "00112233445566778899aabbccddeeff"
                              "ffeeddccbbaa99887766554433221100";
            ctx.buffer = ctx.to_buffer(msg);
        })
    WHEN ("The buffers is examined", [](context& ctx) { })
    THEN ("The buffer contains the hex string", [](context& ctx) {
            ASSERT(3*16, ctx.buffer.size());
            ASSERT(0x01, ctx.buffer[ 0]); ASSERT(0x23, ctx.buffer[ 1]);
            ASSERT(0xef, ctx.buffer[ 7]); ASSERT(0xfe, ctx.buffer[ 8]);
            ASSERT(0x32, ctx.buffer[14]); ASSERT(0x10, ctx.buffer[15]);
            ASSERT(0x00, ctx.buffer[16]); ASSERT(0x11, ctx.buffer[17]);
            ASSERT(0x77, ctx.buffer[23]); ASSERT(0x88, ctx.buffer[24]);
            ASSERT(0xee, ctx.buffer[30]); ASSERT(0xff, ctx.buffer[31]);
            ASSERT(0xff, ctx.buffer[32]); ASSERT(0xee, ctx.buffer[33]);
            ASSERT(0x88, ctx.buffer[39]); ASSERT(0x77, ctx.buffer[40]);
            ASSERT(0x11, ctx.buffer[46]); ASSERT(0x00, ctx.buffer[47]);
        })

  SCENARIO("A buffer is deep copied")
    GIVEN("A non-empty buffer", [](context& ctx) {
            ctx.lhs = ctx.to_buffer("0123456789abcdeffedcba9876543210");
        })
    WHEN ("The buffers is copied", [](context& ctx) {
            ctx.buffer = ctx.lhs;
        })
     AND ("The buffer is modified", [](context& ctx) {
            ctx.buffer.clear();
        })
    THEN ("The original buffer is unaltered", [](context& ctx) {
            ASSERT("0123456789abcdeffedcba9876543210",
                   ctx.to_string(ctx.lhs));
        })

  SCENARIO("Buffers can be bit-wise OR'd")
    GIVEN("Two buffer objects", [](context& ctx) {
            ctx.lhs = ctx.to_buffer(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_buffer(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The buffers are OR'd", [](context& ctx) {
            ctx.buffer = ctx.lhs | ctx.rhs;
        })
    THEN ("The result is the OR'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.buffer));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("00000000",     "deadbeef", "deadbeef")
        SAMPLE("deadbeef",     "00000000", "deadbeef")
        SAMPLE("dead0000",     "0000beef", "deadbeef")
        SAMPLE("dead0000", "0000beef1234", "deadbeef")
    END_SAMPLES()

  SCENARIO("Buffers can be bit-wise AND'd")
    GIVEN("Two buffer objects", [](context& ctx) {
            ctx.lhs = ctx.to_buffer(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_buffer(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The buffers are AND'd", [](context& ctx) {
            ctx.buffer = ctx.lhs & ctx.rhs;
        })
    THEN ("The result is the AND'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.buffer));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("dead0000",     "0000beef", "00000000")
        SAMPLE("deadbeef",     "babecafe", "9aac8aee")
        SAMPLE("deadbeef", "babecafe1234", "9aac8aee")
    END_SAMPLES()

  SCENARIO("Buffers can be bit-wise XOR'd")
    GIVEN("Two data objects", [](context& ctx) {
            ctx.lhs = ctx.to_buffer(ctx.sample<std::string>("lhs"));
            ctx.rhs = ctx.to_buffer(ctx.sample<std::string>("rhs"));
        })
    WHEN ("The buffers are XOR'd", [](context& ctx) {
            ctx.buffer = ctx.lhs ^ ctx.rhs;
        })
    THEN ("The result is the XOR'd result", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.buffer));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("lhs", "rhs", "result")
        SAMPLE("dead0000",     "0000beef", "deadbeef")
        SAMPLE("deadbeef",     "babecafe", "64137411")
        SAMPLE("deadbeef", "babecafe1234", "64137411")
    END_SAMPLES()

  SCENARIO("A buffer can be bit-wise shifted-left")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.lhs = ctx.to_buffer(ctx.sample<std::string>("lhs"));
            ctx.pos = ctx.sample<size_t>("pos");
        })
    WHEN ("The buffer is shifted", [](context& ctx) {
            ctx.buffer = ctx.lhs << ctx.pos;
        })
    THEN ("The result is the shifted", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.buffer));
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

  SCENARIO("A buffer can be bit-wise shifted-right")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.lhs = ctx.to_buffer(ctx.sample<std::string>("lhs"));
            ctx.pos = ctx.sample<size_t>("pos");
        })
    WHEN ("The buffer shifted", [](context& ctx) {
            ctx.buffer = ctx.lhs >> ctx.pos;
        })
    THEN ("The result is the shifted", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("result"),
                   ctx.to_string(ctx.buffer));
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

  SCENARIO("A buffer can be expanded by a single byte at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("adbeef");
        })
    WHEN ("The buffer is expanded at the beginning", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin(), 0xde);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded by a single byte at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbe");
        })
    WHEN ("The buffer is expanded at the end", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cend(), 0xef);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded by a single byte in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("debeef");
        })
    WHEN ("The buffer is expanded in the middle", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin()+1, 0xad);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with padding bytes at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the beginning", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin(), 4, 0x00);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("00000000deadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with padding bytes at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the end", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cend(), 4, 0x00);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeef00000000"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with padding bytes in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded in the middle", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin()+2, 4, 0x00);
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("dead00000000beef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the beginning", [](context& ctx) {
            uint8_t buf[2] = { 0xba, 0xbe };
            ctx.buffer.insert(ctx.buffer.cbegin(), buf, sizeof(buf));
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("babedeadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the end", [](context& ctx) {
            uint8_t buf[2] = { 0xba, 0xbe };
            ctx.buffer.insert(ctx.buffer.cend(), buf, sizeof(buf));
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeefbabe"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded in the middle", [](context& ctx) {
            uint8_t buf[2] = { 0xba, 0xbe };
            ctx.buffer.insert(ctx.buffer.cbegin()+2, buf, sizeof(buf));
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbabebeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with a buffer at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the beginning", [](context& ctx) {
            mud::core::buffer buf( { 0xba, 0xbe } );
            ctx.buffer.insert(ctx.buffer.cbegin(), buf.cbegin(), buf.cend());
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("babedeadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with a buffer at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the end", [](context& ctx) {
            mud::core::buffer buf( { 0xba, 0xbe } );
            ctx.buffer.insert(ctx.buffer.cend(), buf.cbegin(), buf.cend());
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeefbabe"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with a buffer in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded in the middle", [](context& ctx) {
            mud::core::buffer buf( { 0xba, 0xbe } );
            ctx.buffer.insert(ctx.buffer.cbegin()+2, buf.cbegin(), buf.cend());
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbabebeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with an initialiser list at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the beginning", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin(), { 0xba, 0xbe });
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("babedeadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with an initialiser list at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded at the end", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cend(), { 0xba, 0xbe });
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbeefbabe"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be expanded with an initialiser list in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is expanded in the middle", [](context& ctx) {
            ctx.buffer.insert(ctx.buffer.cbegin()+2, { 0xba, 0xbe });
        })
    THEN ("The result is the expanded", [](context& ctx) {
            ASSERT(std::string("deadbabebeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be iterated over")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer is iterated", [](context&) {})
    THEN ("The iterator points to all the elements", [](context& ctx) {
            auto iter = ctx.buffer.begin();
            ASSERT((uint8_t)0xde, *iter++);
            ASSERT((uint8_t)0xad, *iter++);
            ASSERT((uint8_t)0xbe, *iter++);
            ASSERT((uint8_t)0xef, *iter++);
            ASSERT(true, iter == ctx.buffer.end());
        })

  SCENARIO("A buffer can be modified through an iterator")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("dead00ef");
        })
    WHEN ("The buffer is modified through an iterator", [](context& ctx) {
            auto iter = ctx.buffer.begin() + 2;
            *iter = 0xbe;
        })
    THEN ("The result is modified", [](context& ctx) {
            ASSERT(std::string("deadbeef"), ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can be cleared")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("dead00ef");
        })
    WHEN ("The buffer is cleared", [](context& ctx) {
            ctx.buffer.clear();
        })
    THEN ("The buffer is empty", [](context& ctx) {
            ASSERT(0, ctx.buffer.size());
        })

  SCENARIO("An empty buffer can be cleared")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = mud::core::buffer();
        })
    WHEN ("The buffer is cleared", [](context& ctx) {
            ctx.buffer.clear();
        })
    THEN ("The buffer is empty", [](context& ctx) {
            ASSERT(0, ctx.buffer.size());
        })

  SCENARIO("A buffer can erase a single byte at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's first element is erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.begin());
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("adbeef"), ctx.to_string(ctx.buffer));
            ASSERT(true, ctx.iter == ctx.buffer.begin());
            ASSERT(0xad, *ctx.iter);
        })

  SCENARIO("A buffer can erase a single byte at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's last element is erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.end() - 1);
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("deadbe"), ctx.to_string(ctx.buffer));
            ASSERT(true, ctx.iter == ctx.buffer.end());
        })

  SCENARIO("A buffer can erase a single byte in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's element is erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.begin()+2);
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("deadef"), ctx.to_string(ctx.buffer));
            ASSERT(0xef, *ctx.iter);
        })

  SCENARIO("A buffer can erase a range of bytes at the beginning")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's first elements are erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.begin(),
                                        ctx.buffer.begin()+2);
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("beef"), ctx.to_string(ctx.buffer));
            ASSERT(true, ctx.iter == ctx.buffer.begin());
            ASSERT(0xbe, *ctx.iter);
        })

  SCENARIO("A buffer can erase a range of bytes at the end")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's last elements are erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.end() - 2,
                                        ctx.buffer.end());
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("dead"), ctx.to_string(ctx.buffer));
            ASSERT(true, ctx.iter == ctx.buffer.end());
        })

  SCENARIO("A buffer can erase a range of bytes in the middle")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("deadbeef");
        })
    WHEN ("The buffer's element is erased", [](context& ctx) {
            ctx.iter = ctx.buffer.erase(ctx.buffer.begin()+1,
                                        ctx.buffer.begin()+3);
        })
    THEN ("The result is the modified", [](context& ctx) {
            ASSERT(std::string("deef"), ctx.to_string(ctx.buffer));
            ASSERT(0xef, *ctx.iter);
        })

  SCENARIO("A buffer can get an integral value at a byte position")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("0100081122334455667788");
        })
    WHEN ("An integral value is retrieved at a position", [](context& ctx) {
            ctx.ui16 = ctx.buffer.get<uint16_t>(1, endian_t::big);
        })
    THEN ("The value is applied", [](context& ctx) {
            ASSERT(8, ctx.ui16);
        })

  SCENARIO("A buffer can set an integral value at a byte position")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("0100001122334455667788");
        })
    WHEN ("An integral value is set at a position", [](context& ctx) {
            ctx.buffer.set<uint16_t>(1, 8, endian_t::big);
        })
    THEN ("The value is applied", [](context& ctx) {
            ASSERT(std::string("0100081122334455667788"),
                   ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer can get an integral value at an iterator position")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("0100081122334455667788");
            ctx.citer = ctx.buffer.cbegin();
            ++ctx.citer;
        })
    WHEN ("An integral value is retrieved at a position", [](context& ctx) {
            ctx.ui16 = ctx.buffer.get<uint16_t>(ctx.citer, endian_t::big);
        })
    THEN ("The value is applied", [](context& ctx) {
            ASSERT(8, ctx.ui16);
        })

  SCENARIO("A buffer can set an integral value at an iterator position")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer("0100001122334455667788");
            ctx.iter = ctx.buffer.begin();
            ++ctx.iter;
        })
    WHEN ("An integral value is set at a position", [](context& ctx) {
            ctx.buffer.set<uint16_t>(ctx.iter, 8, endian_t::big);
        })
    THEN ("The value is applied", [](context& ctx) {
            ASSERT(std::string("0100081122334455667788"),
                   ctx.to_string(ctx.buffer));
        })

  SCENARIO("A buffer view can be created of a buffer")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
        })
    WHEN ("A buffer view is created", [](context& ctx) {
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
        })
    THEN ("The view is successfully created", [](context& ctx) {
            ASSERT(std::string("0206112233445566"), ctx.to_string(ctx.view));
        })

  SCENARIO("An empty buffer view can be created of a buffer")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
        })
    WHEN ("A buffer view is created", [](context& ctx) {
            ctx.view = ctx.buffer.view(ctx.buffer.begin(), ctx.buffer.begin());
        })
    THEN ("The view is successfully created", [](context& ctx) {
            ASSERT(0, ctx.view.size());
            ASSERT(std::string(""), ctx.to_string(ctx.view));
        })

  SCENARIO("A buffer view can be iterated over")
    GIVEN("A buffer view object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
        })
    WHEN ("A buffer view is iterated over", [](context& ctx) { })
    THEN ("The view can access the binary data", [](context& ctx) {
            auto iter = ctx.view.begin();
            ASSERT((uint8_t)0x02, *iter++);
            ASSERT((uint8_t)0x06, *iter++);
            ASSERT((uint8_t)0x11, *iter++);
            ASSERT((uint8_t)0x22, *iter++);
            ASSERT((uint8_t)0x33, *iter++);
            ASSERT((uint8_t)0x44, *iter++);
            ASSERT((uint8_t)0x55, *iter++);
            ASSERT((uint8_t)0x66, *iter++);
            ASSERT(true, iter == ctx.view.end());
        })

  SCENARIO("A buffer view can modify the buffer data")
    GIVEN("A buffer view object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
        })
    WHEN ("A buffer view modifies the data", [](context& ctx) {
            ctx.view[2] = 0x01;
            ctx.view[4] = 0x03;
        })
    THEN ("The buffer's data is modified", [](context& ctx) {
            ASSERT((uint8_t)0x01, ctx.buffer[4]);
            ASSERT((uint8_t)0x03, ctx.buffer[6]);
        })

  SCENARIO("A buffer sub-view can be created of a buffer view")
    GIVEN("A buffer object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
        })
    WHEN ("A buffer sub-view is created", [](context& ctx) {
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
            ctx.sub_view = ctx.view.view(ctx.view.begin()+2,
                                         ctx.view.end());
        })
    THEN ("The sub-view is successfully created", [](context& ctx) {
            ASSERT(std::string("112233445566"), ctx.to_string(ctx.sub_view));
        })

  SCENARIO("A buffer sub-view can be iterated over")
    GIVEN("A buffer sub-view object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
            ctx.sub_view = ctx.view.view(ctx.view.begin()+2,
                                         ctx.view.end());
        })
    WHEN ("A buffer sub-view is iterated over", [](context& ctx) { })
    THEN ("The view can access the binary data", [](context& ctx) {
            auto iter = ctx.sub_view.begin();
            ASSERT((uint8_t)0x11, *iter++);
            ASSERT((uint8_t)0x22, *iter++);
            ASSERT((uint8_t)0x33, *iter++);
            ASSERT((uint8_t)0x44, *iter++);
            ASSERT((uint8_t)0x55, *iter++);
            ASSERT((uint8_t)0x66, *iter++);
            ASSERT(true, iter == ctx.sub_view.end());
        })

  SCENARIO("A buffer view can modify the buffer data")
    GIVEN("A buffer sub-view object", [](context& ctx) {
            ctx.buffer = ctx.to_buffer(sample_msg);
            ctx.view = ctx.buffer.view(ctx.buffer.begin()+2,
                                       ctx.buffer.begin()+10);
            ctx.sub_view = ctx.view.view(ctx.view.begin()+2,
                                         ctx.view.end());
        })
    WHEN ("A buffer sub-view modifies the data", [](context& ctx) {
            ctx.sub_view[0] = 0x01;
            ctx.sub_view[2] = 0x03;
        })
    THEN ("The buffer's data is modified", [](context& ctx) {
            ASSERT((uint8_t)0x01, ctx.buffer[4]);
            ASSERT((uint8_t)0x03, ctx.buffer[6]);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
