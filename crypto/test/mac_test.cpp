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

#include "mud/crypto/hmac.h"
#include "mud/crypto/mac.h"
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
        delete algorithm;
    }

    /* Convert from string to data */
    mud::core::buffer to_buffer(const std::string& s) {
        mud::core::buffer d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from data to string */
    std::string to_string(const mud::core::buffer& d) {
        std::stringstream sstr;
        sstr << d;
        return sstr.str();
    }

    /* MAC algorithm */
    mud::crypto::basic_mac *algorithm = nullptr;

    /* Keying material (input) */
    mud::crypto::mac_material_t material;

END_CONTEXT()

FEATURE("MAC")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("MAC type traits")
    GIVEN("An MAC type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::crypto::basic_mac>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::crypto::basic_mac>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::crypto::basic_mac>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::crypto::basic_mac>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::crypto::basic_mac>::value);
        })

  SCENARIO("HMAC-SHA-1 hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
        if (ctx.sample<size_t>("key-rpt") == 0) {
            ctx.material.key(ctx.to_buffer(ctx.sample<std::string>("key")));
        }
        else {
            mud::core::buffer key(ctx.sample<size_t>("key-rpt"));
            char ch = ctx.to_buffer(ctx.sample<std::string>("key"))[0];
            memset(key.data(), ch, key.size());
            ctx.material.key(key);
        }
        ctx.material.tag_size(ctx.sample<size_t>("tag-size"));
    })
    AND  ("A HMAC algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::hmac<mud::crypto::sha_1>(ctx.material);
    })
    WHEN ("A message is authenticated", [](context& ctx) {
        mud::core::buffer text;
        if (ctx.sample<size_t>("text-rpt") == 0) {
            text = ctx.to_buffer(ctx.sample<std::string>("text"));
        }
        else {
            text = mud::core::buffer(ctx.sample<size_t>("text-rpt"));
            char ch = ctx.to_buffer(ctx.sample<std::string>("text"))[0];
            memset(text.data(), ch, text.size());
        }
        ctx.algorithm->update(text);
        ctx.algorithm->close();
    })
    THEN ("The key is derived correctly", [](context& ctx) {
        ASSERT(ctx.sample<std::string>("tag"),
               ctx.to_string(ctx.algorithm->tag()));
    })
    SAMPLES(size_t, std::string, size_t, std::string, size_t, std::string)
        HEADINGS("key-rpt", "key", "text-rpt", "text", "tag-size", "tag")
        SAMPLE( 0, "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
                  "0b0b0b0b",
                0, "4869205468657265",
                0, "b617318655057264e28bc0b6fb378c8e"
                   "f146be00")
        SAMPLE( 0, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                   "aaaaaaaa",
               50, "dd",
               20, "125d7342b9ac11cd91a39af48aa17b4f"
                   "63f175d3")
        SAMPLE( 0, "0102030405060708090a0b0c0d0e0f10"
                  "111213141516171819",
               50, "cd",
                0, "4c9007f4026250c6bc8414f9bf50c86c"
                   "2d7235da")
        SAMPLE( 0, "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
                   "0c0c0c0c",
                0, "546573742057697468205472756e6361"
                   "74696f6e",
               12, "4c1a03424b55e07fe7f27be1")
        SAMPLE(80, "aa",
                0, "54657374205573696e67204c61726765"
                   "72205468616e20426c6f636b2d53697a"
                   "65204b6579202d2048617368204b6579"
                   "204669727374",
                0, "aa4ae5e15272d00e95705637ce8a3b55"
                   "ed402112")
        SAMPLE(80, "aa",
                0, "54657374205573696e67204c61726765"
                   "72205468616e20426c6f636b2d53697a"
                   "65204b657920616e64204c6172676572"
                   "205468616e204f6e6520426c6f636b2d"
                   "53697a652044617461",
                0, "e8e99d0f45237d786d6bbaa7965c7808"
                   "bbff1a91")
        SAMPLE(80, "aa",
                0, "54657374205573696e67204c61726765"
                   "72205468616e20426c6f636b2d53697a"
                   "65204b6579202d2048617368204b6579"
                   "204669727374",
                0, "aa4ae5e15272d00e95705637ce8a3b55"
                   "ed402112")
        SAMPLE(80, "aa",
                0, "54657374205573696e67204c61726765"
                   "72205468616e20426c6f636b2d53697a"
                   "65204b657920616e64204c6172676572"
                   "205468616e204f6e6520426c6f636b2d"
                   "53697a652044617461",
                0, "e8e99d0f45237d786d6bbaa7965c7808"
                   "bbff1a91")
    END_SAMPLES()

  SCENARIO("HMAC-SHA-224 hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
        ctx.material.key(ctx.to_buffer(ctx.sample<std::string>("key")));
        ctx.material.tag_size(ctx.sample<size_t>("tag-size"));
    })
    AND  ("A HMAC algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::hmac<mud::crypto::sha_224>(
                ctx.material);
    })
    WHEN ("A message is authenticated", [](context& ctx) {
        ctx.algorithm->update(ctx.to_buffer(ctx.sample<std::string>("text")));
        ctx.algorithm->close();
    })
    THEN ("The key is derived correctly", [](context& ctx) {
        ASSERT(ctx.sample<std::string>("tag"),
               ctx.to_string(ctx.algorithm->tag()));
    })
    SAMPLES(std::string, std::string, size_t, std::string)
        HEADINGS("key", "text", "tag-size", "tag")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"   // RFC 4231
               "aaaaaaaa",
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddd",
               0,
               "7fb3cb3588c6c1f6ffa9694d7d6ad264"
               "9365b0c1f65d69d1ec8333ea")
        SAMPLE("0102030405060708090a0b0c0d0e0f10"
               "111213141516171819",
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcd",
               0,
               "6c11506874013cac6a2abc1bb382627c"
               "ec6a90d86efc012de7afec5a")
        SAMPLE("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
               "0c0c0c0c",
               "546573742057697468205472756e6361"
               "74696f6e",
               16,
               "0e2aea68a90c8d37c988bcdb9fca6fa8")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54657374205573696e67204c61726765"
               "72205468616e20426c6f636b2d53697a"
               "65204b6579202d2048617368204b6579"
               "204669727374",
               0,
               "95e9a0db962095adaebe9b2d6f0dbce2"
               "d499f112f2d2b7273fa6870e")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54686973206973206120746573742075"
               "73696e672061206c6172676572207468"
               "616e20626c6f636b2d73697a65206b65"
               "7920616e642061206c61726765722074"
               "68616e20626c6f636b2d73697a652064"
               "6174612e20546865206b6579206e6565"
               "647320746f2062652068617368656420"
               "6265666f7265206265696e6720757365"
               "642062792074686520484d414320616c"
               "676f726974686d2e",
               0,
               "3a854166ac5d9f023f54d517d0b39dbd"
               "946770db9c2b95c9f6f565d1")
    END_SAMPLES()

  SCENARIO("HMAC-SHA-256 hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
        ctx.material.key(ctx.to_buffer(ctx.sample<std::string>("key")));
        ctx.material.tag_size(ctx.sample<size_t>("tag-size"));
    })
    AND  ("A HMAC algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::hmac<mud::crypto::sha_256>(
                ctx.material);
    })
    WHEN ("A message is authenticated", [](context& ctx) {
        ctx.algorithm->update(ctx.to_buffer(ctx.sample<std::string>("text")));
        ctx.algorithm->close();
    })
    THEN ("The key is derived correctly", [](context& ctx) {
        ASSERT(ctx.sample<std::string>("tag"),
               ctx.to_string(ctx.algorithm->tag()));
    })
    SAMPLES(std::string, std::string, size_t, std::string)
        HEADINGS("key", "text", "tag-size", "tag")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"   // RFC 4231
               "aaaaaaaa",
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddd",
               0,
               "773ea91e36800e46854db8ebd09181a7"
               "2959098b3ef8c122d9635514ced565fe")
        SAMPLE("0102030405060708090a0b0c0d0e0f10"
               "111213141516171819",
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcd",
               0,
               "82558a389a443c0ea4cc819899f2083a"
               "85f0faa3e578f8077a2e3ff46729665b")
        SAMPLE("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
               "0c0c0c0c",
               "546573742057697468205472756e6361"
               "74696f6e",
               16,
               "a3b6167473100ee06e0c796c2955552b")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54657374205573696e67204c61726765"
               "72205468616e20426c6f636b2d53697a"
               "65204b6579202d2048617368204b6579"
               "204669727374",
               0,
               "60e431591ee0b67f0d8a26aacbf5b77f"
               "8e0bc6213728c5140546040f0ee37f54")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54686973206973206120746573742075"
               "73696e672061206c6172676572207468"
               "616e20626c6f636b2d73697a65206b65"
               "7920616e642061206c61726765722074"
               "68616e20626c6f636b2d73697a652064"
               "6174612e20546865206b6579206e6565"
               "647320746f2062652068617368656420"
               "6265666f7265206265696e6720757365"
               "642062792074686520484d414320616c"
               "676f726974686d2e",
               0,
               "9b09ffa71b942fcb27635fbcd5b0e944"
               "bfdc63644f0713938a7f51535c3a35e2")
    END_SAMPLES()

  SCENARIO("HMAC-SHA-384 hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
        ctx.material.key(ctx.to_buffer(ctx.sample<std::string>("key")));
        ctx.material.tag_size(ctx.sample<size_t>("tag-size"));
    })
    AND  ("A HMAC algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::hmac<mud::crypto::sha_384>(
                ctx.material);
    })
    WHEN ("A message is authenticated", [](context& ctx) {
        ctx.algorithm->update(ctx.to_buffer(ctx.sample<std::string>("text")));
        ctx.algorithm->close();
    })
    THEN ("The key is derived correctly", [](context& ctx) {
        ASSERT(ctx.sample<std::string>("tag"),
               ctx.to_string(ctx.algorithm->tag()));
    })
    SAMPLES(std::string, std::string, size_t, std::string)
        HEADINGS("key", "text", "tag-size", "tag")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"   // RFC 4231
               "aaaaaaaa",
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddd",
               0,
               "88062608d3e6ad8a0aa2ace014c8a86f"
               "0aa635d947ac9febe83ef4e55966144b"
               "2a5ab39dc13814b94e3ab6e101a34f27")
        SAMPLE("0102030405060708090a0b0c0d0e0f10"
               "111213141516171819",
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcd",
               0,
               "3e8a69b7783c25851933ab6290af6ca7"
               "7a9981480850009cc5577c6e1f573b4e"
               "6801dd23c4a7d679ccf8a386c674cffb")
        SAMPLE("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
               "0c0c0c0c",
               "546573742057697468205472756e6361"
               "74696f6e",
               16,
               "3abf34c3503b2a23a46efc619baef897")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54657374205573696e67204c61726765"
               "72205468616e20426c6f636b2d53697a"
               "65204b6579202d2048617368204b6579"
               "204669727374",
               0,
               "4ece084485813e9088d2c63a041bc5b4"
               "4f9ef1012a2b588f3cd11f05033ac4c6"
               "0c2ef6ab4030fe8296248df163f44952")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54686973206973206120746573742075"
               "73696e672061206c6172676572207468"
               "616e20626c6f636b2d73697a65206b65"
               "7920616e642061206c61726765722074"
               "68616e20626c6f636b2d73697a652064"
               "6174612e20546865206b6579206e6565"
               "647320746f2062652068617368656420"
               "6265666f7265206265696e6720757365"
               "642062792074686520484d414320616c"
               "676f726974686d2e",
               0,
               "6617178e941f020d351e2f254e8fd32c"
               "602420feb0b8fb9adccebb82461e99c5"
               "a678cc31e799176d3860e6110c46523e")
    END_SAMPLES()

  SCENARIO("HMAC-SHA-512 hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
        ctx.material.key(ctx.to_buffer(ctx.sample<std::string>("key")));
        ctx.material.tag_size(ctx.sample<size_t>("tag-size"));
    })
    AND  ("A HMAC algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::hmac<mud::crypto::sha_512>(
                ctx.material);
    })
    WHEN ("A message is authenticated", [](context& ctx) {
        ctx.algorithm->update(ctx.to_buffer(ctx.sample<std::string>("text")));
        ctx.algorithm->close();
    })
    THEN ("The key is derived correctly", [](context& ctx) {
        ASSERT(ctx.sample<std::string>("tag"),
               ctx.to_string(ctx.algorithm->tag()));
    })
    SAMPLES(std::string, std::string, size_t, std::string)
        HEADINGS("key", "text", "tag-size", "tag")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"   // RFC 4231
               "aaaaaaaa",
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddddddddddddddddddddddddddddddd"
               "dddd",
               0,
               "fa73b0089d56a284efb0f0756c890be9"
               "b1b5dbdd8ee81a3655f83e33b2279d39"
               "bf3e848279a722c806b485a47e67c807"
               "b946a337bee8942674278859e13292fb")
        SAMPLE("0102030405060708090a0b0c0d0e0f10"
               "111213141516171819",
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcd",
               0,
               "b0ba465637458c6990e5a8c5f61d4af7"
               "e576d97ff94b872de76f8050361ee3db"
               "a91ca5c11aa25eb4d679275cc5788063"
               "a5f19741120c4f2de2adebeb10a298dd")
        SAMPLE("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
               "0c0c0c0c",
               "546573742057697468205472756e6361"
               "74696f6e",
               16,
               "415fad6271580a531d4179bc891d87a6")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54657374205573696e67204c61726765"
               "72205468616e20426c6f636b2d53697a"
               "65204b6579202d2048617368204b6579"
               "204669727374",
               0,
               "80b24263c7c1a3ebb71493c1dd7be8b4"
               "9b46d1f41b4aeec1121b013783f8f352"
               "6b56d037e05f2598bd0fd2215d6a1e52"
               "95e64f73f63f0aec8b915a985d786598")
        SAMPLE("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa",
               "54686973206973206120746573742075"
               "73696e672061206c6172676572207468"
               "616e20626c6f636b2d73697a65206b65"
               "7920616e642061206c61726765722074"
               "68616e20626c6f636b2d73697a652064"
               "6174612e20546865206b6579206e6565"
               "647320746f2062652068617368656420"
               "6265666f7265206265696e6720757365"
               "642062792074686520484d414320616c"
               "676f726974686d2e",
               0,
               "e37b6a775dc87dbaa4dfa9f96e5e3ffd"
               "debd71f8867289865df5a32d20cdc944"
               "b6022cac3c4982b10d5eeb55c3e4de15"
               "134676fb6de0446065c97440fa8c6a58")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
