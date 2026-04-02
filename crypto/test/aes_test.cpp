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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
          ctx.aes = new mud::crypto::basic_aes(ctx.material.key().size()*8);
        })
    WHEN ("The plaintext is AES encrypted", [](context& ctx) {
          ctx.input = ctx.to_data(ctx.sample<std::string>("plaintext"));
          ctx.aes->encrypt(ctx.input, ctx.output, ctx.material.key());
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
          ctx.aes = new mud::crypto::basic_aes(ctx.material.key().size()*8);
        })
    WHEN ("The ciphertext is AES decrypted", [](context& ctx) {
          ctx.input = ctx.to_data(ctx.sample<std::string>("ciphertext"));
          ctx.aes->decrypt(ctx.input, ctx.output, ctx.material.key());
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
            mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
            mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
            ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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

  SCENARIO("AES-128 ECB can decrypt with an example vector")
    GIVEN("An AES-128 ECB with keying material and ciphertext",
        [](context& ctx) {
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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

  SCENARIO("AES ECB can encrypt with padding")
    GIVEN("An AES-128 ECB with keying material and plaintext",
        [](context& ctx) {
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::iv_t iv = ctx.to_data(ctx.sample<std::string>("iv"));
          ctx.material = mud::crypto::material_t(key, iv);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          mud::crypto::counter_t counter = ctx.to_data(ctx.sample<std::string>(
                "counter"));
          ctx.material = mud::crypto::material_t(key, counter);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
          mud::crypto::key_t key = ctx.to_data(ctx.sample<std::string>("key"));
          ctx.material = mud::crypto::material_t(key);
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
