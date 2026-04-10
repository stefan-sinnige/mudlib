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

#include "mud/crypto/x25519.h"
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
        delete alice;
        delete bob;
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

    /* Alice and Bob */
    mud::crypto::x25519 *alice = nullptr;
    mud::crypto::x25519 *bob = nullptr;
END_CONTEXT()

FEATURE("X25519")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("X25519 type traits")
    GIVEN("An X25519 type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::crypto::x25519>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::crypto::x25519>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::crypto::x25519>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::crypto::x25519>::value);
        })
    AND  ("The type is  move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::crypto::x25519>::value);
        })

  SCENARIO("Associated public/private keypair")
    GIVEN("An X25519 key-pair", [](context& ctx) {
          auto private_key = ctx.to_data(ctx.sample<std::string>("private"));
          ctx.alice = new mud::crypto::x25519(private_key);
    })
    WHEN ("The public key is examined", [](context&) {})
    THEN ("It is a associated to the private key", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("public"),
                 ctx.to_string(ctx.alice->public_key()));
    })
    SAMPLES(std::string, std::string)
        HEADINGS("private", "public")
        SAMPLE("a8ababababababababababababababab"    // pycurve25519
               "ababababababababababababababab6b",
               "e3712d851a0e5d79b831c5e34ab22b41"
               "a198171de209b8b8faca23a11c624859")
        SAMPLE("c8cdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcd4d",
               "b5bea823d9c9ff576091c54b7c596c0a"
               "e296884f0e150290e88455d7fba6126f")
        SAMPLE("77076d0a7318a57d3c16c17251b26645"    // RFC-7748
               "df4c2f87ebc0992ab177fba51db92c2a",
               "8520f0098930a754748b7ddcb43ef75a"
               "0dbf3a0d26381af4eba4a98eaa9b4e6a")
        SAMPLE("5dab087e624a8a4b79e17f8b83800ee6"
               "6f3bb1292618b6fd1c2f8b27ff88e0eb",
               "de9edb7d7b7dc1b4d35b61c2ece43537"
               "3f8343c85b78674dadfc7e146f882b4f")
        SAMPLE("202122232425262728292a2b2c2d2e2f"    // tls13.xargs.org
               "303132333435363738393a3b3c3d3e3f",
               "358072d6365880d1aeea329adf912138"
               "3851ed21a28e3b75e965d0d2cd166254")
        SAMPLE("909192939495969798999a9b9c9d9e9f"
               "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
               "9fd7ad6dcff4298dd3f96d5b1b2af910"
               "a0535b1488d7f8fabb349a982880b615")
    END_SAMPLES()

  SCENARIO("Keys can be exchanged successfully")
    GIVEN("Alice has a public / private key pair", [](context& ctx) {
          auto private_key = ctx.to_data(ctx.sample<std::string>("private-a"));
          ctx.alice = new mud::crypto::x25519(private_key);
        })
    AND  ("Bob has a public / private key pair", [](context& ctx) {
          auto private_key = ctx.to_data(ctx.sample<std::string>("private-b"));
          ctx.bob = new mud::crypto::x25519(private_key);
        })
    WHEN ("The public keys are exchanged", [](context& ctx) {
          ctx.alice->peer(ctx.bob->public_key());
          ctx.bob->peer(ctx.alice->public_key());
        })
    THEN ("The secret shared keys are the same", [](context& ctx) {
          auto expected = ctx.sample<std::string>("shared");
          ASSERT(expected, ctx.to_string(ctx.alice->shared_key()));
          ASSERT(expected, ctx.to_string(ctx.bob->shared_key()));
        })
    SAMPLES(std::string, std::string, std::string)
        HEADINGS("private-a", "private-b", "shared")
        SAMPLE("a8ababababababababababababababab"    // pycurve25519
               "ababababababababababababababab6b",
               "c8cdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
               "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcd4d",
               "235101b705734aae8d4c2d9d0f1baf90"
               "bbb2a8c233d831a80d43815bb47ead10")
        SAMPLE("77076d0a7318a57d3c16c17251b26645"    // RFC-7748
               "df4c2f87ebc0992ab177fba51db92c2a",
               "5dab087e624a8a4b79e17f8b83800ee6"
               "6f3bb1292618b6fd1c2f8b27ff88e0eb",
               "4a5d9d5ba4ce2de1728e3bf480350f25"
               "e07e21c947d19e3376f09b3c1e161742")
        SAMPLE("202122232425262728292a2b2c2d2e2f"    // tls13.xargs.org
               "303132333435363738393a3b3c3d3e3f",
               "909192939495969798999a9b9c9d9e9f"
               "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf",
               "df4a291baa1eb7cfa6934b29b474baad"
               "2697e29f1f920dcc77c8a0a088447624")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
