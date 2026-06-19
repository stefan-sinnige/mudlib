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

#include "mud/crypto/hkdf.h"
#include "mud/crypto/kdf.h"
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

    /* KDF algorithm */
    mud::crypto::basic_kdf *algorithm = nullptr;

    /* Keying material (input) */
    mud::crypto::kdf_material_t material;

    /* Calculated key */
    mud::crypto::key_t key;

END_CONTEXT()

FEATURE("KDF")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("KDF type traits")
    GIVEN("An KDF type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::crypto::basic_kdf>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::crypto::basic_kdf>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::crypto::basic_kdf>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::crypto::basic_kdf>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::crypto::basic_kdf>::value);
        })

  SCENARIO("HKDF hash with an example vector")
    GIVEN("Input Keying material", [](context& ctx) {
    })
    AND  ("A HKDF algorithm", [](context& ctx) {
        ctx.material.mac_algorithm(ctx.sample<std::string>("mac"));
        ctx.material.ikm(ctx.to_buffer(ctx.sample<std::string>("ikm")));
        ctx.material.salt(ctx.to_buffer(ctx.sample<std::string>("salt")));
        ctx.algorithm = new mud::crypto::hkdf(ctx.material);
    })
    WHEN ("A key is derived", [](context& ctx) {
        auto key_size = ctx.sample<size_t>("length");
        auto info = ctx.to_buffer(ctx.sample<std::string>("info"));
        ctx.key = ctx.algorithm->key(key_size, info);
    })
    THEN ("The key is derived correctly", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("key"),
                 ctx.to_string(ctx.key));
    })
    SAMPLES(std::string, std::string, std::string, std::string, size_t, std::string)
        HEADINGS("mac", "ikm", "salt", "info", "length", "key")
        SAMPLE("HMAC-SHA-256",                          // RFC 5869
               "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
               "0b0b0b0b0b0b",
               "000102030405060708090a0b0c",
               "f0f1f2f3f4f5f6f7f8f9",
               42,
               "3cb25f25faacd57a90434f64d0362f2a"
               "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
               "34007208d5b887185865")
        SAMPLE("HMAC-SHA-256",
               "000102030405060708090a0b0c0d0e0f"
               "101112131415161718191a1b1c1d1e1f"
               "202122232425262728292a2b2c2d2e2f"
               "303132333435363738393a3b3c3d3e3f"
               "404142434445464748494a4b4c4d4e4f",
               "606162636465666768696a6b6c6d6e6f"
               "707172737475767778797a7b7c7d7e7f"
               "808182838485868788898a8b8c8d8e8f"
               "909192939495969798999a9b9c9d9e9f"
               "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
               "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
               "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
               "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
               "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               82,
               "b11e398dc80327a1c8e7f78c596a4934"
               "4f012eda2d4efad8a050cc4c19afa97c"
               "59045a99cac7827271cb41c65e590e09"
               "da3275600c2f09b8367793a9aca3db71"
               "cc30c58179ec3e87c14c01d5c1f3434f"
               "1d87")
        SAMPLE("HMAC-SHA-256",
               "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
               "0b0b0b0b0b0b",
               "",
               "",
               42,
               "8da4e775a563c18f715f802a063c5a31"
               "b8a11f5c5ee1879ec3454e5f3c738d2d"
               "9d201395faa4b61a96c8")
        SAMPLE("HMAC-SHA-1",
               "0b0b0b0b0b0b0b0b0b0b0b",
               "000102030405060708090a0b0c",
               "f0f1f2f3f4f5f6f7f8f9",
               42,
               "085a01ea1b10f36933068b56efa5ad81"
               "a4f14b822f5b091568a9cdd4f155fda2"
               "c22e422478d305f3f896")
        SAMPLE("HMAC-SHA-1",
               "000102030405060708090a0b0c0d0e0f"
               "101112131415161718191a1b1c1d1e1f"
               "202122232425262728292a2b2c2d2e2f"
               "303132333435363738393a3b3c3d3e3f"
               "404142434445464748494a4b4c4d4e4f",
               "606162636465666768696a6b6c6d6e6f"
               "707172737475767778797a7b7c7d7e7f"
               "808182838485868788898a8b8c8d8e8f"
               "909192939495969798999a9b9c9d9e9f"
               "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
               "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
               "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
               "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
               "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               82,
               "0bd770a74d1160f7c9f12cd5912a06eb"
               "ff6adcae899d92191fe4305673ba2ffe"
               "8fa3f1a4e5ad79f3f334b3b202b2173c"
               "486ea37ce3d397ed034c7f9dfeb15c5e"
               "927336d0441f4c4300e2cff0d0900b52"
               "d3b4")
        SAMPLE("HMAC-SHA-1",
               "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
               "0b0b0b0b0b0b",
               "",
               "",
               42,
               "0ac1af7002b3d761d1e55298da9d0506"
               "b9ae52057220a306e07b6b87e8df21d0"
               "ea00033de03984d34918")
        SAMPLE("HMAC-SHA-1",
               "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"
               "0c0c0c0c0c0c",
               "",
               "",
               42,
               "2c91117204d745f3500d636a62f64f0a"
               "b3bae548aa53d423b0d1f27ebba6f5e5"
               "673a081d70cce7acfc48")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
