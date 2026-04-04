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

#include "mud/crypto/aes.h"
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
        delete aes;
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

    /* Generic multi-key aes */
    mud::crypto::basic_aes* aes = nullptr;

    /* The AES keying material */
    mud::crypto::material_t material;

    /* The input block */
    mud::crypto::data_t input;

    /* The output block */
    mud::crypto::data_t output;

    /* The plaintext */
    mud::crypto::data_t plaintext;

    /* The ciphertext */
    mud::crypto::data_t ciphertext;

    /* The authentication tag */
    mud::crypto::data_t tag;

    /* A text stream */
    std::stringstream text_stream;

END_CONTEXT()

FEATURE("AES")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("AES type traits")
    GIVEN("An AES type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::crypto::aes<128>>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::crypto::aes<128>>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::crypto::aes<128>>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::crypto::aes<128>>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::crypto::aes<128>>::value);
        })

  SCENARIO("Basic AES block cipher can encrypt one block")
    GIVEN("An AES block cipher with a key and plaintext", [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.aes = new mud::crypto::basic_aes(ctx.material.key().size()*8);
        })
    WHEN ("The plaintext is AES encrypted", [](context& ctx) {
          ctx.input = ctx.to_data(ctx.sample<std::string>("plaintext"));
          ctx.aes->forward(ctx.input, ctx.output, ctx.material.key());
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.output));
        })
    SAMPLES(size_t, std::string, std::string, std::string)
        HEADINGS("keysize", "key", "plaintext", "ciphertext")
        SAMPLE(128,
               "2b7e151628aed2a6abf7158809cf4f3c",
               "3243f6a8885a308d313198a2e0370734",
               "3925841d02dc09fbdc118597196a0b32")
        SAMPLE(192,
               "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
               "3243f6a8885a308d313198a2e0370734",
               "585e9fb6c2722b9af4f492c12bb024c1")
        SAMPLE(256,
               "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
               "3243f6a8885a308d313198a2e0370734",
               "3021613a973e582f4a29234137aec494")
    END_SAMPLES()

  SCENARIO("Basic AES block cipher can decrypt one block")
    GIVEN("An AES block cipher with a key and plaintext", [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.aes = new mud::crypto::basic_aes(ctx.material.key().size()*8);
        })
    WHEN ("The ciphertext is AES decrypted", [](context& ctx) {
          ctx.input = ctx.to_data(ctx.sample<std::string>("ciphertext"));
          ctx.aes->inverse(ctx.input, ctx.output, ctx.material.key());
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.output));
        })
    SAMPLES(size_t, std::string, std::string, std::string)
        HEADINGS("keysize", "key", "ciphertext", "plaintext")
        SAMPLE(128,
               "2b7e151628aed2a6abf7158809cf4f3c",
               "3925841d02dc09fbdc118597196a0b32",
               "3243f6a8885a308d313198a2e0370734")
        SAMPLE(192,
               "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
               "585e9fb6c2722b9af4f492c12bb024c1",
               "3243f6a8885a308d313198a2e0370734")
        SAMPLE(256,
               "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
               "3021613a973e582f4a29234137aec494",
               "3243f6a8885a308d313198a2e0370734")
    END_SAMPLES()

  SCENARIO("AES-128 ECB can encrypt with an example vector")
    GIVEN("An AES-128 ECB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes_128_ecb(ctx.material);
          aes_128_ecb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "43b1cd7f598ece23881b00e3ed030688"
               "7b0c785e27e8ad3f8223207104725dd4")
    END_SAMPLES()

  SCENARIO("AES-192 ECB can encrypt with an example vector")
    GIVEN("An AES-192 ECB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_192_ECB aes_192_ecb(ctx.material);
          aes_192_ecb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("ciphertext"),
                   ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "bd334f1d6e45f25ff712a214571fa5cc"
               "974104846d0ad3ad7734ecb3ecee4eef"
               "ef7afd2270e2e60adce0ba2face6444e"
               "9a4b41ba738d6c72fb16691603c18e0e")
    END_SAMPLES()

  SCENARIO("AES-256 ECB can encrypt with an example vector")
    GIVEN("An AES-256 ECB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_256_ECB aes_256_ecb(ctx.material);
          aes_256_ecb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "f3eed1bdb5d2a03c064b5a7e3db181f8"
               "591ccb10d410ed26dc5ba74a31362870"
               "b6ed21b99ca6f4f9f153e7b1beafed1d"
               "23304b7a39f9f3ff067d8d8f9e24ecc7")
    END_SAMPLES()

  SCENARIO("AES-128 CBC can encrypt with an example vector")
    GIVEN("An AES-128 CBC with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CBC aes_128_cbc(ctx.material);
          aes_128_cbc.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "7649abac8119b246cee98e9b12e9197d"
               "5086cb9b507219ee95db113a917678b2"
               "73bed6b8e3c1743b7116e69e22229516"
               "3ff1caa1681fac09120eca307586e1a7")
    END_SAMPLES()

  SCENARIO("AES-192 CBC can encrypt with an example vector")
    GIVEN("An AES-192 CBC with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_192_CBC aes_192_cbc(ctx.material);
          aes_192_cbc.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "4f021db243bc633d7178183a9fa071e8"
               "b4d9ada9ad7dedf4e5e738763f69145a"
               "571b242012fb7ae07fa9baac3df102e0"
               "08b0e27988598881d920a9e64f5615cd")
    END_SAMPLES()

  SCENARIO("AES-256 CBC can encrypt with an example vector")
    GIVEN("An AES-256 CBC with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_256_CBC aes_256_cbc(ctx.material);
          aes_256_cbc.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "f58c4c04d6e5f1ba779eabfb5f7bfbd6"
               "9cfc4e967edb808d679f777bc6702c7d"
               "39f23369a9d9bacfa530e26304231461"
               "b2eb05e2c39be9fcda6c19078c6a9d1b")
    END_SAMPLES()

  SCENARIO("AES-128 CFB can encrypt with an example vector")
    GIVEN("An AES-128 CFB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CFB aes_128_cfb(ctx.material);
          aes_128_cfb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("ciphertext"),
                   ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce58b"
               "26751f67a3cbb140b1808cf187a4f4df"
               "c04b05357c5d1c0eeac4c66f9ff7f2e6")
    END_SAMPLES()

  SCENARIO("AES-192 CFB can encrypt with an example vector")
    GIVEN("An AES-192 CFB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_192_CFB aes_192_cfb(ctx.material);
          aes_192_cfb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "cdc80d6fddf18cab34c25909c99a4174"
               "67ce7f7f81173621961a2b70171d3d7a"
               "2e1e8a1dd59b88b1c8e60fed1efac4c9"
               "c05f9f9ca9834fa042ae8fba584b09ff")
    END_SAMPLES()

  SCENARIO("AES-256 CFB can encrypt with an example vector")
    GIVEN("An AES-256 CFB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_256_CFB aes_256_cfb(ctx.material);
          aes_256_cfb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "dc7e84bfda79164b7ecd8486985d3860"
               "39ffed143b28b1c832113c6331e5407b"
               "df10132415e54b92a13ed0a8267ae2f9"
               "75a385741ab9cef82031623d55b1e471")
    END_SAMPLES()

  SCENARIO("AES-128 CTR can encrypt with an example vector")
    GIVEN("An AES-128 CTR with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CTR aes_128_ctr(ctx.material);
          aes_128_ctr.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffdff"
               "5ae4df3edbd5d35e5b4f09020db03eab"
               "1e031dda2fbe03d1792170a0f3009cee")
    END_SAMPLES()

  SCENARIO("AES-192 CTR can encrypt with an example vector")
    GIVEN("An AES-192 CTR with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_192_CTR aes_192_ctr(ctx.material);
          aes_192_ctr.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "plaintext", "ciphertext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "1abc932417521ca24f2b0459fe7e6e0b"
               "090339ec0aa6faefd5ccc2c6f4ce8e94"
               "1e36b26bd1ebc670d1bd1d665620abf7"
               "4f78a7f6d29809585a97daec58c6b050")
    END_SAMPLES()

  SCENARIO("AES-256 CTR can encrypt with an example vector")
    GIVEN("An AES-256 CTR with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_256_CTR aes_256_ctr(ctx.material);
          aes_256_ctr.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "plaintext", "ciphertext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "601ec313775789a5b7a7f504bbf3d228"
               "f443e3ca4d62b59aca84e990cacaf5c5"
               "2b0930daa23de94ce87017ba2d84988d"
               "dfc9c58db67aada613c2dd08457941a6")
    END_SAMPLES()

  SCENARIO("AES-128 GCM can encrypt with an example vector")
    GIVEN("An AES-128 GCM with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_GCM aes_128_gcm(ctx.material);
          aes_128_gcm.encrypt(ctx.plaintext, ctx.ciphertext);
          ctx.tag = aes_128_gcm.authentication_tag();
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "plaintext", "ciphertext", "tag")
        SAMPLE("00000000000000000000000000000000",  // Test Case 1
               "000000000000000000000000",
               "",
               "",
               "",
               "58e2fccefa7e3061367f1d57a4e7455a")
        SAMPLE("00000000000000000000000000000000",  // Test Case 2
               "000000000000000000000000",
               "",
               "00000000000000000000000000000000",
               "0388dace60b6a392f328c2b971b2fe78",
               "ab6e47d42cec13bdf53a67b21257bddf")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 3
               "cafebabefacedbaddecaf888",
               "",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "42831ec2217774244b7221b784d0d49c"
               "e3aa212f2c02a4e035c17e2329aca12e"
               "21d514b25466931c7d8f6a5aac84aa05"
               "1ba30b396a0aac973d58e091473f5985",
               "4d5c2af327cd64a62cf35abd2ba6fab4")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 4
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "42831ec2217774244b7221b784d0d49c"
               "e3aa212f2c02a4e035c17e2329aca12e"
               "21d514b25466931c7d8f6a5aac84aa05"
               "1ba30b396a0aac973d58e091",
               "5bc94fbc3221a5db94fae95ae7121a47")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 5
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "61353b4c2806934a777ff51fa22a4755"
               "699b2a714fcdc6f83766e5f97b6c7423"
               "73806900e49f24b22b097544d4896b42"
               "4989b5e1ebac0f07c23f4598",
               "3612d2e79e3b0785561be14aaca2fccb")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 6
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "8ce24998625615b603a033aca13fb894"
               "be9112a5c3a211a8ba262a3cca7e2ca7"
               "01e4a9a4fba43c90ccdcb281d48c7c6f"
               "d62875d2aca417034c34aee5",
               "619cc5aefffe0bfa462af43c1699d050")
    END_SAMPLES()

  SCENARIO("AES-192 GCM can encrypt with an example vector")
    GIVEN("An AES-192 GCM with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_192_GCM aes_192_gcm(ctx.material);
          aes_192_gcm.encrypt(ctx.plaintext, ctx.ciphertext);
          ctx.tag = aes_192_gcm.authentication_tag();
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "plaintext", "ciphertext", "tag")
        SAMPLE("00000000000000000000000000000000"   // Test Case 7
               "0000000000000000",
               "000000000000000000000000",
               "",
               "",
               "",
               "cd33b28ac773f74ba00ed1f312572435")
        SAMPLE("00000000000000000000000000000000"   // Test Case 8
               "0000000000000000",
               "000000000000000000000000",
               "",
               "00000000000000000000000000000000",
               "98e7247c07f0fe411c267e4384b0f600",
               "2ff58d80033927ab8ef4d4587514f0fb")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 9
               "feffe9928665731c",
               "cafebabefacedbaddecaf888",
               "",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "3980ca0b3c00e841eb06fac4872a2757"
               "859e1ceaa6efd984628593b40ca1e19c"
               "7d773d00c144c525ac619d18c84a3f47"
               "18e2448b2fe324d9ccda2710acade256",
               "9924a7c8587336bfb118024db8674a14")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 10
               "feffe9928665731c",
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "3980ca0b3c00e841eb06fac4872a2757"
               "859e1ceaa6efd984628593b40ca1e19c"
               "7d773d00c144c525ac619d18c84a3f47"
               "18e2448b2fe324d9ccda2710",
               "2519498e80f1478f37ba55bd6d27618c")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 11 
               "feffe9928665731c",
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "0f10f599ae14a154ed24b36e25324db8"
               "c566632ef2bbb34f8347280fc4507057"
               "fddc29df9a471f75c66541d4d4dad1c9"
               "e93a19a58e8b473fa0f062f7",
               "65dcc57fcf623a24094fcca40d3533f8")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 12 
               "feffe9928665731c",
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "d27e88681ce3243c4830165a8fdcf9ff"
               "1de9a1d8e6b447ef6ef7b79828666e45"
               "81e79012af34ddd9e2f037589b292db3"
               "e67c036745fa22e7e9b7373b",
               "dcf566ff291c25bbb8568fc3d376a6d9")
    END_SAMPLES()

  SCENARIO("AES-256 GCM can encrypt with an example vector")
    GIVEN("An AES-256 GCM with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_256_GCM aes_256_gcm(ctx.material);
          aes_256_gcm.encrypt(ctx.plaintext, ctx.ciphertext);
          ctx.tag = aes_256_gcm.authentication_tag();
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "plaintext", "ciphertext", "tag")
        SAMPLE("00000000000000000000000000000000"   // Test Case 13
               "00000000000000000000000000000000",
               "000000000000000000000000",
               "",
               "",
               "",
               "530f8afbc74536b9a963b4f1c4cb738b")
        SAMPLE("00000000000000000000000000000000"   // Test Case 14
               "00000000000000000000000000000000",
               "000000000000000000000000",
               "",
               "00000000000000000000000000000000",
               "cea7403d4d606b6e074ec5d3baf39d18",
               "d0d1c8a799996bf0265b98b5d48ab919")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 15
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbaddecaf888",
               "",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "522dc1f099567d07f47f37a32a84427d"
               "643a8cdcbfe5c0c97598a2bd2555d1aa"
               "8cb08e48590dbb3da7b08b1056828838"
               "c5f61e6393ba7a0abcc9f662898015ad",
               "b094dac5d93471bdec1a502270e3cc6c")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 16
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "522dc1f099567d07f47f37a32a84427d"
               "643a8cdcbfe5c0c97598a2bd2555d1aa"
               "8cb08e48590dbb3da7b08b1056828838"
               "c5f61e6393ba7a0abcc9f662",
               "76fc6ece0f4e1768cddf8853bb2d551b")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 17 
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "c3762df1ca787d32ae47c13bf19844cb"
               "af1ae14d0b976afac52ff7d79bba9de0"
               "feb582d33934a4f0954cc2363bc73f78"
               "62ac430e64abe499f47c9b1f",
               "3a337dbf46a792c45e454913fe2ea8f2")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 17 
               "feffe9928665731c6d6a8f9467308308",
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "5a8def2f0c9e53f1f75d7853659e2a20"
               "eeb2b22aafde6419a058ab4f6f746bf4"
               "0fc0c3b780f244452da3ebf1c5d82cde"
               "a2418997200ef82e44ae7e3f",
               "a44a8266ee1c8eb0c8b5d4cf5ae9f19a")
    END_SAMPLES()

  SCENARIO("AES-128 ECB can decrypt with an example vector")
    GIVEN("An AES-128 ECB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes_128_ecb(ctx.material);
          aes_128_ecb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "43b1cd7f598ece23881b00e3ed030688"
               "7b0c785e27e8ad3f8223207104725dd4",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-192 ECB can decrypt with an example vector")
    GIVEN("An AES-192 ECB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The plaintext is decrypted", [](context& ctx) {
          mud::crypto::AES_192_ECB aes_192_ecb(ctx.material);
          aes_192_ecb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "bd334f1d6e45f25ff712a214571fa5cc"
               "974104846d0ad3ad7734ecb3ecee4eef"
               "ef7afd2270e2e60adce0ba2face6444e"
               "9a4b41ba738d6c72fb16691603c18e0e",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-256 ECB can decrypt with an example vector")
    GIVEN("An AES-256 ECB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The plaintext is decrypted", [](context& ctx) {
          mud::crypto::AES_256_ECB aes_256_ecb(ctx.material);
          aes_256_ecb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "f3eed1bdb5d2a03c064b5a7e3db181f8"
               "591ccb10d410ed26dc5ba74a31362870"
               "b6ed21b99ca6f4f9f153e7b1beafed1d"
               "23304b7a39f9f3ff067d8d8f9e24ecc7",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-128 CBC can decrypt with an example vector")
    GIVEN("An AES-128 CBC with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CBC aes_128_cbc(ctx.material);
          aes_128_cbc.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "5086cb9b507219ee95db113a917678b2"
               "73bed6b8e3c1743b7116e69e22229516"
               "3ff1caa1681fac09120eca307586e1a7",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-192 CBC can decrypt with an example vector")
    GIVEN("An AES-192 CBC with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_192_CBC aes_192_cbc(ctx.material);
          aes_192_cbc.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "000102030405060708090a0b0c0d0e0f",
               "4f021db243bc633d7178183a9fa071e8"
               "b4d9ada9ad7dedf4e5e738763f69145a"
               "571b242012fb7ae07fa9baac3df102e0"
               "08b0e27988598881d920a9e64f5615cd",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-256 CBC can decrypt with an example vector")
    GIVEN("An AES-256 CBC with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_256_CBC aes_256_cbc(ctx.material);
          aes_256_cbc.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "000102030405060708090a0b0c0d0e0f",
               "f58c4c04d6e5f1ba779eabfb5f7bfbd6"
               "9cfc4e967edb808d679f777bc6702c7d"
               "39f23369a9d9bacfa530e26304231461"
               "b2eb05e2c39be9fcda6c19078c6a9d1b",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-128 CFB can decrypt with an example vector")
    GIVEN("An AES-128 CFB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CFB aes_128_cfb(ctx.material);
          aes_128_cfb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce58b"
               "26751f67a3cbb140b1808cf187a4f4df"
               "c04b05357c5d1c0eeac4c66f9ff7f2e6",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-192 CFB can decrypt with an example vector")
    GIVEN("An AES-192 CFB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_192_CFB aes_192_cfb(ctx.material);
          aes_192_cfb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "000102030405060708090a0b0c0d0e0f",
               "cdc80d6fddf18cab34c25909c99a4174"
               "67ce7f7f81173621961a2b70171d3d7a"
               "2e1e8a1dd59b88b1c8e60fed1efac4c9"
               "c05f9f9ca9834fa042ae8fba584b09ff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-256 CFB can decrypt with an example vector")
    GIVEN("An AES-256 CFB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_256_CFB aes_256_cfb(ctx.material);
          aes_256_cfb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "000102030405060708090a0b0c0d0e0f",
               "dc7e84bfda79164b7ecd8486985d3860"
               "39ffed143b28b1c832113c6331e5407b"
               "df10132415e54b92a13ed0a8267ae2f9"
               "75a385741ab9cef82031623d55b1e471",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-128 CTR can decrypt with an example vector")
    GIVEN("An AES-128 CTR with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CTR aes_128_ctr(ctx.material);
          aes_128_ctr.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffdff"
               "5ae4df3edbd5d35e5b4f09020db03eab"
               "1e031dda2fbe03d1792170a0f3009cee",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-192 CTR can decrypt with an example vector")
    GIVEN("An AES-192 CTR with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_192_CTR aes_192_ctr(ctx.material);
          aes_192_ctr.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "ciphertext", "plaintext")
        SAMPLE("8e73b0f7da0e6452c810f32b809079e5"
               "62f8ead2522c6b7b",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "1abc932417521ca24f2b0459fe7e6e0b"
               "090339ec0aa6faefd5ccc2c6f4ce8e94"
               "1e36b26bd1ebc670d1bd1d665620abf7"
               "4f78a7f6d29809585a97daec58c6b050",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-256 CTR can decrypt with an example vector")
    GIVEN("An AES-256 CTR with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_256_CTR aes_256_ctr(ctx.material);
          aes_256_ctr.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "ciphertext", "plaintext")
        SAMPLE("603deb1015ca71be2b73aef0857d7781"
               "1f352c073b6108d72d9810a30914dff4",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "601ec313775789a5b7a7f504bbf3d228"
               "f443e3ca4d62b59aca84e990cacaf5c5"
               "2b0930daa23de94ce87017ba2d84988d"
               "dfc9c58db67aada613c2dd08457941a6",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES-128 GCM can decrypt with an example vector")
    GIVEN("An AES-128 GCM with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_GCM aes_128_gcm(ctx.material);
          aes_128_gcm.decrypt(ctx.ciphertext, ctx.plaintext);
          ctx.tag = aes_128_gcm.authentication_tag();
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "ciphertext", "plaintext", "tag")
        SAMPLE("00000000000000000000000000000000",  // Test Case 1
               "000000000000000000000000",
               "",
               "",
               "",
               "58e2fccefa7e3061367f1d57a4e7455a")
        SAMPLE("00000000000000000000000000000000",  // Test Case 2
               "000000000000000000000000",
               "",
               "0388dace60b6a392f328c2b971b2fe78",
               "00000000000000000000000000000000",
               "ab6e47d42cec13bdf53a67b21257bddf")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 3
               "cafebabefacedbaddecaf888",
               "",
               "42831ec2217774244b7221b784d0d49c"
               "e3aa212f2c02a4e035c17e2329aca12e"
               "21d514b25466931c7d8f6a5aac84aa05"
               "1ba30b396a0aac973d58e091473f5985",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "4d5c2af327cd64a62cf35abd2ba6fab4")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 4
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "42831ec2217774244b7221b784d0d49c"
               "e3aa212f2c02a4e035c17e2329aca12e"
               "21d514b25466931c7d8f6a5aac84aa05"
               "1ba30b396a0aac973d58e091",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "5bc94fbc3221a5db94fae95ae7121a47")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 5
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "61353b4c2806934a777ff51fa22a4755"
               "699b2a714fcdc6f83766e5f97b6c7423"
               "73806900e49f24b22b097544d4896b42"
               "4989b5e1ebac0f07c23f4598",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "3612d2e79e3b0785561be14aaca2fccb")
        SAMPLE("feffe9928665731c6d6a8f9467308308",  // Test Case 6
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "8ce24998625615b603a033aca13fb894"
               "be9112a5c3a211a8ba262a3cca7e2ca7"
               "01e4a9a4fba43c90ccdcb281d48c7c6f"
               "d62875d2aca417034c34aee5",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "619cc5aefffe0bfa462af43c1699d050")
    END_SAMPLES()

  SCENARIO("AES-192 GCM can decrypt with an example vector")
    GIVEN("An AES-192 GCM with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_192_GCM aes_192_gcm(ctx.material);
          aes_192_gcm.decrypt(ctx.ciphertext, ctx.plaintext);
          ctx.tag = aes_192_gcm.authentication_tag();
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "ciphertext", "plaintext", "tag")
        SAMPLE("00000000000000000000000000000000"   // Test Case 7
               "0000000000000000",
               "000000000000000000000000",
               "",
               "",
               "",
               "cd33b28ac773f74ba00ed1f312572435")
        SAMPLE("00000000000000000000000000000000"   // Test Case 8
               "0000000000000000",
               "000000000000000000000000",
               "",
               "98e7247c07f0fe411c267e4384b0f600",
               "00000000000000000000000000000000",
               "2ff58d80033927ab8ef4d4587514f0fb")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 9
               "feffe9928665731c",
               "cafebabefacedbaddecaf888",
               "",
               "3980ca0b3c00e841eb06fac4872a2757"
               "859e1ceaa6efd984628593b40ca1e19c"
               "7d773d00c144c525ac619d18c84a3f47"
               "18e2448b2fe324d9ccda2710acade256",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "9924a7c8587336bfb118024db8674a14")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 10
               "feffe9928665731c",
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "3980ca0b3c00e841eb06fac4872a2757"
               "859e1ceaa6efd984628593b40ca1e19c"
               "7d773d00c144c525ac619d18c84a3f47"
               "18e2448b2fe324d9ccda2710",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "2519498e80f1478f37ba55bd6d27618c")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 11 
               "feffe9928665731c",
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "0f10f599ae14a154ed24b36e25324db8"
               "c566632ef2bbb34f8347280fc4507057"
               "fddc29df9a471f75c66541d4d4dad1c9"
               "e93a19a58e8b473fa0f062f7",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "65dcc57fcf623a24094fcca40d3533f8")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 12 
               "feffe9928665731c",
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "d27e88681ce3243c4830165a8fdcf9ff"
               "1de9a1d8e6b447ef6ef7b79828666e45"
               "81e79012af34ddd9e2f037589b292db3"
               "e67c036745fa22e7e9b7373b",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "dcf566ff291c25bbb8568fc3d376a6d9")
    END_SAMPLES()

  SCENARIO("AES-256 GCM can decrypt with an example vector")
    GIVEN("An AES-256 GCM with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.aad(ctx.to_data(ctx.sample<std::string>("aad")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_256_GCM aes_256_gcm(ctx.material);
          aes_256_gcm.decrypt(ctx.ciphertext, ctx.plaintext);
          ctx.tag = aes_256_gcm.authentication_tag();
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    AND  ("The authentication tag is calculated", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("tag"),
                 ctx.to_string(ctx.tag));
        })
    SAMPLES(std::string, std::string, std::string, std::string, std::string,
            std::string)
        HEADINGS("key", "iv", "aad", "ciphertext", "plaintext", "tag")
        SAMPLE("00000000000000000000000000000000"   // Test Case 13
               "00000000000000000000000000000000",
               "000000000000000000000000",
               "",
               "",
               "",
               "530f8afbc74536b9a963b4f1c4cb738b")
        SAMPLE("00000000000000000000000000000000"   // Test Case 14
               "00000000000000000000000000000000",
               "000000000000000000000000",
               "",
               "cea7403d4d606b6e074ec5d3baf39d18",
               "00000000000000000000000000000000",
               "d0d1c8a799996bf0265b98b5d48ab919")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 15
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbaddecaf888",
               "",
               "522dc1f099567d07f47f37a32a84427d"
               "643a8cdcbfe5c0c97598a2bd2555d1aa"
               "8cb08e48590dbb3da7b08b1056828838"
               "c5f61e6393ba7a0abcc9f662898015ad",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b391aafd255",
               "b094dac5d93471bdec1a502270e3cc6c")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 16
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbaddecaf888",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "522dc1f099567d07f47f37a32a84427d"
               "643a8cdcbfe5c0c97598a2bd2555d1aa"
               "8cb08e48590dbb3da7b08b1056828838"
               "c5f61e6393ba7a0abcc9f662",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "76fc6ece0f4e1768cddf8853bb2d551b")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 17 
               "feffe9928665731c6d6a8f9467308308",
               "cafebabefacedbad",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "c3762df1ca787d32ae47c13bf19844cb"
               "af1ae14d0b976afac52ff7d79bba9de0"
               "feb582d33934a4f0954cc2363bc73f78"
               "62ac430e64abe499f47c9b1f",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "3a337dbf46a792c45e454913fe2ea8f2")
        SAMPLE("feffe9928665731c6d6a8f9467308308"   // Test Case 17 
               "feffe9928665731c6d6a8f9467308308",
               "9313225df88406e555909c5aff5269aa"
               "6a7a9538534f7da1e4c303d2a318a728"
               "c3c0c95156809539fcf0e2429a6b5254"
               "16aedbf5a0de6a57a637b39b",
               "feedfacedeadbeeffeedfacedeadbeef"
               "abaddad2",
               "5a8def2f0c9e53f1f75d7853659e2a20"
               "eeb2b22aafde6419a058ab4f6f746bf4"
               "0fc0c3b780f244452da3ebf1c5d82cde"
               "a2418997200ef82e44ae7e3f",
               "d9313225f88406e5a55909c5aff5269a"
               "86a7a9531534f7da2e4c303d8a318a72"
               "1c3c0c95956809532fcf0e2449a6b525"
               "b16aedf5aa0de657ba637b39",
               "a44a8266ee1c8eb0c8b5d4cf5ae9f19a")
    END_SAMPLES()

  SCENARIO("AES ECB can encrypt with padding")
    GIVEN("An AES-128 ECB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes_128_ecb(ctx.material);
          aes_128_ecb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "9e197020026bcdee188eeda4d2d83c4e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "b23dd7754aaa5b9ffe7d3cc5e7bbd386")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "73e15c314562a5fe796655a9dfc43c5b")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "5d569b5e2c7bac7313ad79f359798fe6")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "a254be88e037ddd9d79fb6411c3f9df8")
    END_SAMPLES()

  SCENARIO("AES CBC can encrypt with padding")
    GIVEN("An AES-128 CBC with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CBC aes_128_cbc(ctx.material);
          aes_128_cbc.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae",
               "7649abac8119b246cee98e9b12e9197d"
               "34d2d260173113008c28112c77668c86")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d",
               "7649abac8119b246cee98e9b12e9197d"
               "813a1616da05e0fec242a20f1c5b77aa")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af",
               "7649abac8119b246cee98e9b12e9197d"
               "0ed367b319636d40f3cf021cf6394b47")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e",
               "7649abac8119b246cee98e9b12e9197d"
               "cb856aebf22b76e1bb917d2fe54848cb")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51",
               "7649abac8119b246cee98e9b12e9197d"
               "5086cb9b507219ee95db113a917678b2"
               "55e21d7100b988ffec32feeafaf23538")
    END_SAMPLES()

  SCENARIO("AES CFB can encrypt without padding")
    GIVEN("An AES-128 CFB with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CFB aes_128_cfb(ctx.material);
          aes_128_cfb.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a6")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1c")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce5")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce58b")
    END_SAMPLES()

  SCENARIO("AES CTR can encrypt without padding")
    GIVEN("An AES-128 CTR with keying material and plaintext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_CTR aes_128_ctr(ctx.material);
          aes_128_ctr.encrypt(ctx.plaintext, ctx.ciphertext);
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("ciphertext"),
                 ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae",
               "874d6191b620e3261bef6864990db6ce"
               "98")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d",
               "874d6191b620e3261bef6864990db6ce"
               "9806")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9ff")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffd")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffdff")
    END_SAMPLES()

  SCENARIO("AES ECB can decrypt with padding")
    GIVEN("An AES-128 ECB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes_128_ecb(ctx.material);
          aes_128_ecb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "9e197020026bcdee188eeda4d2d83c4e",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "b23dd7754aaa5b9ffe7d3cc5e7bbd386",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "73e15c314562a5fe796655a9dfc43c5b",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "5d569b5e2c7bac7313ad79f359798fe6",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "a254be88e037ddd9d79fb6411c3f9df8",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51")
    END_SAMPLES()

  SCENARIO("AES CBC can decrypt with padding")
    GIVEN("An AES-128 CBC with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CBC aes_128_cbc(ctx.material);
          aes_128_cbc.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "34d2d260173113008c28112c77668c86",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "813a1616da05e0fec242a20f1c5b77aa",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "0ed367b319636d40f3cf021cf6394b47",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "cb856aebf22b76e1bb917d2fe54848cb",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "7649abac8119b246cee98e9b12e9197d"
               "5086cb9b507219ee95db113a917678b2"
               "55e21d7100b988ffec32feeafaf23538",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51")
    END_SAMPLES()

  SCENARIO("AES CFB can decrypt without padding")
    GIVEN("An AES-128 CFB with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.iv(ctx.to_data(ctx.sample<std::string>("iv")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CFB aes_128_cfb(ctx.material);
          aes_128_cfb.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "iv", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a6",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce5",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "000102030405060708090a0b0c0d0e0f",
               "3b3fd92eb72dad20333449f8e83cfb4a"
               "c8a64537a0b3a93fcde3cdad9f1ce58b",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51")
    END_SAMPLES()

  SCENARIO("AES CTR can decrypt without padding")
    GIVEN("An AES-128 CTR with keying material and ciphertext",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.counter(ctx.to_data(ctx.sample<std::string>("counter")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_CTR aes_128_ctr(ctx.material);
          aes_128_ctr.decrypt(ctx.ciphertext, ctx.plaintext);
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string, std::string)
        HEADINGS("key", "counter", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "98",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "9806",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9ff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffd",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff",
               "874d6191b620e3261bef6864990db6ce"
               "9806f66b7970fdff8617187bb9fffdff",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51")
    END_SAMPLES()

  SCENARIO("AES without padding can encrypt with an ostream")
    GIVEN("An AES-128 ECB with keying material and plaintext stream",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes(ctx.material);
          std::ostream ostr(aes.sbuf(ctx.text_stream.rdbuf()));
          uint8_t* cptr = ctx.plaintext.data();
          uint8_t* eptr = cptr + ctx.plaintext.size();
          while (cptr < eptr) {
              if (!ostr.put((char)*cptr++)) {
                  break;
              }
          }
          aes.sbuf()->close();
          std::string str = ctx.text_stream.str();
          ctx.ciphertext = mud::crypto::data_t((const uint8_t*)str.data(),
                                               str.size());
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("ciphertext"),
                   ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "43b1cd7f598ece23881b00e3ed030688"
               "7b0c785e27e8ad3f8223207104725dd4")
    END_SAMPLES()

  SCENARIO("AES without padding can decrypt with an istream")
    GIVEN("An AES-128 ECB with keying material and ciphertext stream",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
          ctx.text_stream.str(std::string((const char*)ctx.ciphertext.data(),
                                          ctx.ciphertext.size()));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes(ctx.material);
          std::istream istr(aes.sbuf(ctx.text_stream.rdbuf()));
          uint8_t ch;
          while (istr.get((char&)ch)) {
              ctx.plaintext.append(mud::crypto::data_t(&ch, 1));
          }
          aes.sbuf()->close();
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "43b1cd7f598ece23881b00e3ed030688"
               "7b0c785e27e8ad3f8223207104725dd4",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51"
               "30c81c46a35ce411e5fbc1191a0a52ef"
               "f69f2445df4f9b17ad2b417be66c3710")
    END_SAMPLES()

  SCENARIO("AES with padding can encrypt with an ostream")
    GIVEN("An AES-128 ECB with keying material and plaintext stream",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.plaintext = ctx.to_data(ctx.sample<std::string>("plaintext"));
        })
    WHEN ("The plaintext is encrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes(ctx.material);
          std::ostream ostr(aes.sbuf(ctx.text_stream.rdbuf()));
          uint8_t* cptr = ctx.plaintext.data();
          uint8_t* eptr = cptr + ctx.plaintext.size();
          while (cptr < eptr) {
              if (!ostr.put((char)*cptr++)) {
                  break;
              }
          }
          aes.sbuf()->close();
          std::string str = ctx.text_stream.str();
          ctx.ciphertext = mud::crypto::data_t((const uint8_t*)str.data(),
                                               str.size());
        })
    THEN ("The ciphertext contains the encrypted data", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("ciphertext"),
                   ctx.to_string(ctx.ciphertext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "plaintext", "ciphertext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "9e197020026bcdee188eeda4d2d83c4e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "b23dd7754aaa5b9ffe7d3cc5e7bbd386")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "73e15c314562a5fe796655a9dfc43c5b")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "5d569b5e2c7bac7313ad79f359798fe6")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "a254be88e037ddd9d79fb6411c3f9df8")
    END_SAMPLES()

  SCENARIO("AES with padding can decrypt with an istream")
    GIVEN("An AES-128 ECB with keying material and ciphertext stream",
        [](context& ctx) {
          ctx.material.key(ctx.to_data(ctx.sample<std::string>("key")));
          ctx.material.padding(mud::crypto::padding_t::pkcs7);
          ctx.ciphertext = ctx.to_data(ctx.sample<std::string>("ciphertext"));
          ctx.text_stream.str(std::string((const char*)ctx.ciphertext.data(),
                                          ctx.ciphertext.size()));
        })
    WHEN ("The ciphertext is decrypted", [](context& ctx) {
          mud::crypto::AES_128_ECB aes(ctx.material);
          std::istream istr(aes.sbuf(ctx.text_stream.rdbuf()));
          uint8_t ch;
          while (istr.get((char&)ch)) {
              ctx.plaintext.append(mud::crypto::data_t(&ch, 1));
          }
          aes.sbuf()->close();
        })
    THEN ("The plaintext contains the decrypted data", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("plaintext"),
                 ctx.to_string(ctx.plaintext));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("key", "ciphertext", "plaintext")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "9e197020026bcdee188eeda4d2d83c4e",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "b23dd7754aaa5b9ffe7d3cc5e7bbd386",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "73e15c314562a5fe796655a9dfc43c5b",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "5d569b5e2c7bac7313ad79f359798fe6",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e")
        SAMPLE("2b7e151628aed2a6abf7158809cf4f3c",
               "3ad77bb40d7a3660a89ecaf32466ef97"
               "f5d3d58503b9699de785895a96fdbaaf"
               "a254be88e037ddd9d79fb6411c3f9df8",
               "6bc1bee22e409f96e93d7e117393172a"
               "ae2d8a571e03ac9c9eb76fac45af8e51")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
