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

#include "mud/crypto/shake.h"
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
    mud::crypto::data_t to_data(const std::string& s) {
        mud::crypto::data_t d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from data to string */
    std::string to_string(const mud::crypto::data_t& d) {
        std::stringstream sstr;
        sstr << d;
        return sstr.str();
    }

    /* Hashing algorithm */
    mud::crypto::basic_hash *algorithm = nullptr;

    /* Input binary data */
    mud::crypto::data_t input;

    /* Calculated hash */
    mud::crypto::data_t hash;

END_CONTEXT()

FEATURE("SHAKE")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("SHAKE type traits")
    GIVEN("An SHAKE type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::crypto::shake_128>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::crypto::shake_128>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::crypto::shake_128>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::crypto::shake_128>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::crypto::shake_128>::value);
        })

  SCENARIO("SHAKE-128 hash with an example vector")
    GIVEN("A SHAKE-128 algorithm", [](context& ctx) {
        auto hash_bits = ctx.sample<size_t>("hash-bits");
        ctx.algorithm = new mud::crypto::shake_128(hash_bits);
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::crypto::data_t((const uint8_t*)data.data(),
                                        data.size());
    })
    WHEN ("The hash is calculated", [](context& ctx) {
        ctx.algorithm->update(ctx.input);
        ctx.algorithm->close();
        ctx.hash = ctx.algorithm->hash();
    })
    THEN ("The hash is calculated correctly", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("hash"),
                 ctx.to_string(ctx.hash));
    })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("input", "hash-bits", "hash")
        SAMPLE("",
               128,
               "7f9c2ba4e88f827d616045507605853e")
        SAMPLE("abc",
               128,
               "5881092dd818bf5cf8a3ddb793fbcba7")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               128,
               "1a96182b50fb8c7e74e0a707788f55e9")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               128,
               "7b6df6ff181173b6d7898d7ff63fb07b")
    END_SAMPLES()

  SCENARIO("SHAKE-256 hash with an example vector")
    GIVEN("A SHAKE-256 algorithm", [](context& ctx) {
        auto hash_bits = ctx.sample<size_t>("hash-bits");
        ctx.algorithm = new mud::crypto::shake_256(hash_bits);
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::crypto::data_t((const uint8_t*)data.data(),
                                        data.size());
    })
    WHEN ("The hash is calculated", [](context& ctx) {
        ctx.algorithm->update(ctx.input);
        ctx.algorithm->close();
        ctx.hash = ctx.algorithm->hash();
    })
    THEN ("The hash is calculated correctly", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("hash"),
                 ctx.to_string(ctx.hash));
    })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("input", "hash-bits", "hash")
        SAMPLE("",
               128,
               "46b9dd2b0ba88d13233b3feb743eeb24")
        SAMPLE("abc",
               128,
               "483366601360a8771c6863080cc4114d")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               128,
               "4d8c2dd2435a0128eefbb8c36f6f8713")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               128,
               "98be04516c04cc73593fef3ed0352ea9")
    END_SAMPLES()

  SCENARIO("SHAKE-128 variable hash length")
    GIVEN("A SHAKE-128 algorithm", [](context& ctx) {
        auto hash_bits = ctx.sample<size_t>("hash-bits");
        ctx.algorithm = new mud::crypto::shake_128(hash_bits);
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::crypto::data_t((const uint8_t*)data.data(),
                                        data.size());
    })
    WHEN ("The hash is calculated", [](context& ctx) {
        ctx.algorithm->update(ctx.input);
        ctx.algorithm->close();
        ctx.hash = ctx.algorithm->hash();
    })
    THEN ("The hash is calculated correctly", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("hash"),
                 ctx.to_string(ctx.hash));
    })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("input", "hash-bits", "hash")
        SAMPLE("",
               128,
               "7f9c2ba4e88f827d616045507605853e")
        SAMPLE("",
               256,
               "7f9c2ba4e88f827d616045507605853e"
               "d73b8093f6efbc88eb1a6eacfa66ef26")
        SAMPLE("",
               512,
               "7f9c2ba4e88f827d616045507605853e"
               "d73b8093f6efbc88eb1a6eacfa66ef26"
               "3cb1eea988004b93103cfb0aeefd2a68"
               "6e01fa4a58e8a3639ca8a1e3f9ae57e2")
        SAMPLE("",
               4096,
               "7f9c2ba4e88f827d616045507605853e"
               "d73b8093f6efbc88eb1a6eacfa66ef26"
               "3cb1eea988004b93103cfb0aeefd2a68"
               "6e01fa4a58e8a3639ca8a1e3f9ae57e2"
               "35b8cc873c23dc62b8d260169afa2f75"
               "ab916a58d974918835d25e6a435085b2"
               "badfd6dfaac359a5efbb7bcc4b59d538"
               "df9a04302e10c8bc1cbf1a0b3a5120ea"
               "17cda7cfad765f5623474d368ccca8af"
               "0007cd9f5e4c849f167a580b14aabdef"
               "aee7eef47cb0fca9767be1fda69419df"
               "b927e9df07348b196691abaeb580b32d"
               "ef58538b8d23f87732ea63b02b4fa0f4"
               "873360e2841928cd60dd4cee8cc0d4c9"
               "22a96188d032675c8ac850933c7aff15"
               "33b94c834adbb69c6115bad4692d8619"
               "f90b0cdf8a7b9c264029ac185b70b83f"
               "2801f2f4b3f70c593ea3aeeb613a7f1b"
               "1de33fd75081f592305f2e4526edc096"
               "31b10958f464d889f31ba010250fda7f"
               "1368ec2967fc84ef2ae9aff268e0b170"
               "0affc6820b523a3d917135f2dff2ee06"
               "bfe72b3124721d4a26c04e53a75e30e7"
               "3a7a9c4a95d91c55d495e9f51dd0b5e9"
               "d83c6d5e8ce803aa62b8d654db53d09b"
               "8dcff273cdfeb573fad8bcd45578bec2"
               "e770d01efde86e721a3f7c6cce275dab"
               "e6e2143f1af18da7efddc4c7b70b5e34"
               "5db93cc936bea323491ccb38a388f546"
               "a9ff00dd4e1300b9b2153d2041d205b4"
               "43e41b45a653f2a5c4492c1add544512"
               "dda2529833462b71a41a45be97290b6f")
    END_SAMPLES()

  SCENARIO("SHAKE-256 variable hash length")
    GIVEN("A SHAKE-256 algorithm", [](context& ctx) {
        auto hash_bits = ctx.sample<size_t>("hash-bits");
        ctx.algorithm = new mud::crypto::shake_256(hash_bits);
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::crypto::data_t((const uint8_t*)data.data(),
                                        data.size());
    })
    WHEN ("The hash is calculated", [](context& ctx) {
        ctx.algorithm->update(ctx.input);
        ctx.algorithm->close();
        ctx.hash = ctx.algorithm->hash();
    })
    THEN ("The hash is calculated correctly", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("hash"),
                 ctx.to_string(ctx.hash));
    })
    SAMPLES(std::string, size_t, std::string)
        HEADINGS("input", "hash-bits", "hash")
        SAMPLE("",
               128,
               "46b9dd2b0ba88d13233b3feb743eeb24")
        SAMPLE("",
               256,
               "46b9dd2b0ba88d13233b3feb743eeb24"
               "3fcd52ea62b81b82b50c27646ed5762f")
        SAMPLE("",
               512,
               "46b9dd2b0ba88d13233b3feb743eeb24"
               "3fcd52ea62b81b82b50c27646ed5762f"
               "d75dc4ddd8c0f200cb05019d67b592f6"
               "fc821c49479ab48640292eacb3b7c4be")
        SAMPLE("",
               4096,
               "46b9dd2b0ba88d13233b3feb743eeb24"
               "3fcd52ea62b81b82b50c27646ed5762f"
               "d75dc4ddd8c0f200cb05019d67b592f6"
               "fc821c49479ab48640292eacb3b7c4be"
               "141e96616fb13957692cc7edd0b45ae3"
               "dc07223c8e92937bef84bc0eab862853"
               "349ec75546f58fb7c2775c38462c5010"
               "d846c185c15111e595522a6bcd16cf86"
               "f3d122109e3b1fdd943b6aec468a2d62"
               "1a7c06c6a957c62b54dafc3be87567d6"
               "77231395f6147293b68ceab7a9e0c58d"
               "864e8efde4e1b9a46cbe854713672f5c"
               "aaae314ed9083dab4b099f8e300f01b8"
               "650f1f4b1d8fcf3f3cb53fb8e9eb2ea2"
               "03bdc970f50ae55428a91f7f53ac266b"
               "28419c3778a15fd248d339ede785fb7f"
               "5a1aaa96d313eacc890936c173cdcd0f"
               "ab882c45755feb3aed96d477ff96390b"
               "f9a66d1368b208e21f7c10d04a3dbd4e"
               "360633e5db4b602601c14cea737db3dc"
               "f722632cc77851cbdde2aaf0a33a07b3"
               "73445df490cc8fc1e4160ff118378f11"
               "f0477de055a81a9eda57a4a2cfb0c839"
               "29d310912f729ec6cfa36c6ac6a75837"
               "143045d791cc85eff5b21932f23861bc"
               "f23a52b5da67eaf7baae0f5fb1369db7"
               "8f3ac45f8c4ac5671d85735cdddb09d2"
               "b1e34a1fc066ff4a162cb263d6541274"
               "ae2fcc865f618abe27c124cd8b074ccd"
               "516301b91875824d09958f341ef274bd"
               "ab0bae316339894304e35877b0c28a9b"
               "1fd166c796b9cc258a064a8f57e27f2a")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
