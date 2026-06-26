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

#include "mud/crypto/sha.h"
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

    /* Hashing algorithm */
    mud::crypto::basic_hash *algorithm = nullptr;

    /* Input binary data */
    mud::core::buffer input;

    /* Calculated hash */
    mud::core::buffer hash;

    /* A text stream */
    std::stringstream text_stream;

END_CONTEXT()

const char lorum_ipsum[] = 
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vestibulum "
    "viverra lacus eu euismod. Maecenas tincidunt ante et venenatis bibendum. "
    "Duis sem velit, luctus id finibus non, cursus non odio. Vestibulum "
    "malesuada auctor arcu. Phasellus mollis tortor quis venenatis semper. "
    "Vivamus porta, arcu eget porta ultricies, tellus sapien aliquam mauris, "
    "at posuere nunc arcu a leo. Maecenas sit amet tempus ex. Donec purus "
    "risus, consectetur et leo sit amet, condimentum ultrices felis. Sed non "
    "metus porta, laoreet lectus non, tempus massa. Morbi rhoncus nulla sit "
    "amet sodales pulvinar. Nulla condimentum tempor purus ut fermentum. "
    "Curabitur ornare consectetur nisi in scelerisque. Donec accumsan nibh "
    "vitae libero accumsan, sit amet facilisis odio molestie. Curabitur eu "
    "eros at magna elementum aliquet. Suspendisse consectetur eleifend massa, "
    "non dapibus ex ultricies quis. Sed eu ullamcorper justo, in eleifend "
    "magna.";

FEATURE("SHA")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("SHA type traits")
    GIVEN("An SHA type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::crypto::sha_1>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::crypto::sha_1>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::crypto::sha_1>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::crypto::sha_1>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::crypto::sha_1>::value);
        })

  SCENARIO("SHA-1 hash with an example vector")
    GIVEN("A SHA-1 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_1;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "da39a3ee5e6b4b0d3255bfef95601890"
               "afd80709")
        SAMPLE("abc",
               "a9993e364706816aba3e25717850c26c"
               "9cd0d89d")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "84983e441c3bd26ebaae4aa1f95129e5"
               "e54670f1")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "a49b2446a02c645bf419f995b6709125"
               "3a04a259")
    END_SAMPLES()

  SCENARIO("SHA-224 hash with an example vector")
    GIVEN("A SHA-224 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_224;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "d14a028c2a3a2bc9476102bb288234c4"
               "15a2b01f828ea62ac5b3e42f")
        SAMPLE("abc",
               "23097d223405d8228642a477bda255b3"
               "2aadbce4bda0b3f7e36c9da7")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "75388b16512776cc5dba5da1fd890150"
               "b0c6455cb4f58b1952522525")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "c97ca9a559850ce97a04a96def6d99a9"
               "e0e0e2ab14e6b8df265fc0b3")
    END_SAMPLES()

  SCENARIO("SHA-256 hash with an example vector")
    GIVEN("A SHA-256 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_256;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "e3b0c44298fc1c149afbf4c8996fb924"
               "27ae41e4649b934ca495991b7852b855")
        SAMPLE("abc",
               "ba7816bf8f01cfea414140de5dae2223"
               "b00361a396177a9cb410ff61f20015ad")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "248d6a61d20638b8e5c026930c3e6039"
               "a33ce45964ff2167f6ecedd419db06c1")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "cf5b16a778af8380036ce59e7b049237"
               "0b249b11e8f07a51afac45037afee9d1")
    END_SAMPLES()

  SCENARIO("SHA-384 hash with an example vector")
    GIVEN("A SHA-384 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_384;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "38b060a751ac96384cd9327eb1b1e36a"
               "21fdb71114be07434c0cc7bf63f6e1da"
               "274edebfe76f65fbd51ad2f14898b95b")
        SAMPLE("abc",
               "cb00753f45a35e8bb5a03d699ac65007"
               "272c32ab0eded1631a8b605a43ff5bed"
               "8086072ba1e7cc2358baeca134c825a7")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "3391fdddfc8dc7393707a65b1b470939"
               "7cf8b1d162af05abfe8f450de5f36bc6"
               "b0455a8520bc4e6f5fe95b1fe3c8452b")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "09330c33f71147e83d192fc782cd1b47"
               "53111b173b3b05d22fa08086e3b0f712"
               "fcc7c71a557e2db966c3e9fa91746039")
    END_SAMPLES()

  SCENARIO("SHA-512 hash with an example vector")
    GIVEN("A SHA-512 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_512;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "cf83e1357eefb8bdf1542850d66d8007"
               "d620e4050b5715dc83f4a921d36ce9ce"
               "47d0d13c5d85f2b0ff8318d2877eec2f"
               "63b931bd47417a81a538327af927da3e")
        SAMPLE("abc",
               "ddaf35a193617abacc417349ae204131"
               "12e6fa4e89a97ea20a9eeee64b55d39a"
               "2192992a274fc1a836ba3c23a3feebbd"
               "454d4423643ce80e2a9ac94fa54ca49f")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "204a8fc6dda82f0a0ced7beb8e08a416"
               "57c16ef468b228a8279be331a703c335"
               "96fd15c13b1b07f9aa1d3bea57789ca0"
               "31ad85c7a71dd70354ec631238ca3445")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "8e959b75dae313da8cf4f72814fc143f"
               "8f7779c6eb9f7fa17299aeadb6889018"
               "501d289e4900f7e4331b99dec4b5433a"
               "c7d329eeb6dd26545e96e55b874be909")
    END_SAMPLES()

  SCENARIO("SHA-512/224 hash with an example vector")
    GIVEN("A SHA-512/224 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_512_224;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "6ed0dd02806fa89e25de060c19d3ac86"
               "cabb87d6a0ddd05c333b84f4")
        SAMPLE("abc",
               "4634270f707b6a54daae7530460842e2"
               "0e37ed265ceee9a43e8924aa")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "e5302d6d54bb242275d1e7622d68df6e"
               "b02dedd13f564c13dbda2174")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "23fec5bb94d60b23308192640b0c4533"
               "35d664734fe40e7268674af9")
    END_SAMPLES()

  SCENARIO("SHA-512/256 hash with an example vector")
    GIVEN("A SHA-512/256 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_512_256;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "c672b8d1ef56ed28ab87c3622c511406"
               "9bdd3ad7b8f9737498d0c01ecef0967a")
        SAMPLE("abc",
               "53048e2681941ef99b2e29b76b4c7dab"
               "e4c2d0c634fc6d46e0e2f13107e7af23")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "bde8e1f9f19bb9fd3406c90ec6bc47bd"
               "36d8ada9f11880dbc8a22a7078b6a461")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "3928e184fb8690f840da3988121d31be"
               "65cb9d3ef83ee6146feac861e19b563a")
    END_SAMPLES()

  SCENARIO("SHA3-224 hash with an example vector")
    GIVEN("A SHA3-224 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha3_224;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "6b4e03423667dbb73b6e15454f0eb1ab"
               "d4597f9a1b078e3f5b5a6bc7")
        SAMPLE("abc",
               "e642824c3f8cf24ad09234ee7d3c766f"
               "c9a3a5168d0c94ad73b46fdf")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "8a24108b154ada21c9fd5574494479ba"
               "5c7e7ab76ef264ead0fcce33")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "543e6868e1666c1a643630df77367ae5"
               "a62a85070a51c14cbf665cbc")
    END_SAMPLES()

  SCENARIO("SHA3-256 hash with an example vector")
    GIVEN("A SHA3-256 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha3_256;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "a7ffc6f8bf1ed76651c14756a061d662"
               "f580ff4de43b49fa82d80a4b80f8434a")
        SAMPLE("abc",
               "3a985da74fe225b2045c172d6bd390bd"
               "855f086e3e9d525b46bfe24511431532")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "41c0dba2a9d6240849100376a8235e2c"
               "82e1b9998a999e21db32dd97496d3376")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "916f6061fe879741ca6469b43971dfdb"
               "28b1a32dc36cb3254e812be27aad1d18")
    END_SAMPLES()

  SCENARIO("SHA3-384 hash with an example vector")
    GIVEN("A SHA3-384 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha3_384;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "0c63a75b845e4f7d01107d852e4c2485"
               "c51a50aaaa94fc61995e71bbee983a2a"
               "c3713831264adb47fb6bd1e058d5f004")
        SAMPLE("abc",
               "ec01498288516fc926459f58e2c6ad8d"
               "f9b473cb0fc08c2596da7cf0e49be4b2"
               "98d88cea927ac7f539f1edf228376d25")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "991c665755eb3a4b6bbdfb75c78a492e"
               "8c56a22c5c4d7e429bfdbc32b9d4ad5a"
               "a04a1f076e62fea19eef51acd0657c22")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "79407d3b5916b59c3e30b09822974791"
               "c313fb9ecc849e406f23592d04f625dc"
               "8c709b98b43b3852b337216179aa7fc7")
    END_SAMPLES()

  SCENARIO("SHA3-512 hash with an example vector")
    GIVEN("A SHA3-512 algorithm", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha3_512;
    })
    AND  ("An input binary string", [](context& ctx) {
        auto data = ctx.sample<std::string>("input");
        ctx.input = mud::core::buffer((const uint8_t*)data.data(),
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
    SAMPLES(std::string, std::string)
        HEADINGS("input", "hash")
        SAMPLE("",
               "a69f73cca23a9ac5c8b567dc185a756e"
               "97c982164fe25859e0d1dcc1475c80a6"
               "15b2123af1f5f94c11e3e9402c3ac558"
               "f500199d95b6d3e301758586281dcd26")
        SAMPLE("abc",
               "b751850b1a57168a5693cd924b6b096e"
               "08f621827444f70d884f5d0240d2712e"
               "10e116e9192af3c91a7ec57647e39340"
               "57340b4cf408d5a56592f8274eec53f0")
        SAMPLE("abcdbcdecdefdefgefghfghighijhijk"
               "ijkljklmklmnlmnomnopnopq",
               "04a371e84ecfb5b8b77cb48610fca818"
               "2dd457ce6f326a0fd3d7ec2f1e91636d"
               "ee691fbe0c985302ba1b0d8dc78c0863"
               "46b533b49c030d99a27daf1139d6e75e")
        SAMPLE("abcdefghbcdefghicdefghijdefghijk"
               "efghijklfghijklmghijklmnhijklmno"
               "ijklmnopjklmnopqklmnopqrlmnopqrs"
               "mnopqrstnopqrstu",
               "afebb2ef542e6579c50cad06d2e578f9"
               "f8dd6881d7dc824d26360feebf18a4fa"
               "73e3261122948efcfd492e74e82e2189"
               "ed0fb440d187f382270cb455f21dd185")
    END_SAMPLES()

  SCENARIO("SHA can be generated with an ostream")
    GIVEN("A SHA algorithm and an output stream", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_1;
    })
    WHEN ("Text is written to the output stream", [](context& ctx) {
        std::ostream ostr(ctx.algorithm->sbuf(ctx.text_stream.rdbuf()));
        ostr << lorum_ipsum << std::flush;
        ctx.algorithm->close();
    })
    THEN("The SHA is generated", [](context& ctx) {
        ASSERT("42ec62ab686554e2ed44e60e918fe13daf52cc84",
               ctx.to_string(ctx.algorithm->hash()));
    })

  SCENARIO("SHA can be generated with an istream")
    GIVEN("A SHA algorithm and an input stream", [](context& ctx) {
        ctx.algorithm = new mud::crypto::sha_1;
        ctx.text_stream.str(lorum_ipsum);
    })
    WHEN ("The input stream is read", [](context& ctx) {
        std::istream istr(ctx.algorithm->sbuf(ctx.text_stream.rdbuf()));
        uint8_t ch;
        while (istr.get((char&)ch)) {} 
        ctx.algorithm->close();
    })
    THEN("The SHA is generated", [](context& ctx) {
        ASSERT("42ec62ab686554e2ed44e60e918fe13daf52cc84",
               ctx.to_string(ctx.algorithm->hash()));
    })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
