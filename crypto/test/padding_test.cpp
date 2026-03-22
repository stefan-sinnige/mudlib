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

#include "mud/crypto/exception.h"
#include "mud/crypto/padding.h"
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
    }

    /* Convert from string to data */
    mud::crypto::data_t to_data(const std::string& s) {
        mud::crypto::data_t d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from data to string */
    std::string to_string(const mud::crypto::data_t& b) {
        std::stringstream sstr;
        sstr << b;
        return sstr.str();
    }

    /* A data object */
    mud::crypto::data_t padded;

    /* A data object */
    mud::crypto::data_t unpadded;

    /* Flag to indicate ifan exception has been raised */
    bool exception_raised = false;
END_CONTEXT()

FEATURE("Padding")
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("pkcs7_padding type traits")
    GIVEN("An pkcs7_padding type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::crypto::pkcs7_padding>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::crypto::pkcs7_padding>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::crypto::pkcs7_padding>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::crypto::pkcs7_padding>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::crypto::pkcs7_padding>::value);
        })

  SCENARIO("Data can be padded using PKCS#7 padding algorithm")
    GIVEN("Cryptographic data of a certain size", [](context& ctx) {
            ctx.unpadded = ctx.to_data(ctx.sample<std::string>("unpadded"));
        })
    WHEN ("The data is padded for an 8-byte block size", [](context& ctx) {
            mud::crypto::pkcs7_padding padder(8);
            ctx.padded = padder.pad(ctx.unpadded);
        })
    THEN ("The padded data is added correctly", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("padded"),
                   ctx.to_string(ctx.padded));
        })
    SAMPLES(std::string, std::string)
        HEADINGS("unpadded", "padded")
        SAMPLE(                        "", "0808080808080808")
        SAMPLE(                      "ae", "ae07070707070707")
        SAMPLE(                    "ae60", "ae60060606060606")
        SAMPLE(            "ae609181f34b", "ae609181f34b0202")
        SAMPLE(          "ae609181f34b85", "ae609181f34b8501")
        SAMPLE(        "ae609181f34b8501", "ae609181f34b85010808080808080808")
        SAMPLE(      "ae609181f34b8501f2", "ae609181f34b8501f207070707070707")
    END_SAMPLES()

  SCENARIO("Data can be unpadded using PKCS#7 padding algorithm")
    GIVEN("Cryptographic padded data of a certain size", [](context& ctx) {
            ctx.padded = ctx.to_data(ctx.sample<std::string>("padded"));
        })
    WHEN ("The data is padded for an 8-byte block size", [](context& ctx) {
            mud::crypto::pkcs7_padding padder(8);
            ctx.unpadded = padder.unpad(ctx.padded);
        })
    THEN ("The padded data is added correctly", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("unpadded"),
                   ctx.to_string(ctx.unpadded));
        })
    SAMPLES(std::string, std::string)
        HEADINGS("padded", "unpadded")
        SAMPLE("0808080808080808",                 "")
        SAMPLE("ae07070707070707",                 "ae")
        SAMPLE("ae60060606060606",                 "ae60")
        SAMPLE("ae609181f34b0202",                 "ae609181f34b")
        SAMPLE("ae609181f34b8501",                 "ae609181f34b85")
        SAMPLE("ae609181f34b85010808080808080808", "ae609181f34b8501")
        SAMPLE("ae609181f34b8501f207070707070707", "ae609181f34b8501f2")
    END_SAMPLES()

  SCENARIO("An exception is thrown if PKCS#7 unpadding can not be performed")
    GIVEN("Cryptographic data larger than block-size", [](context& ctx) {
            ctx.padded = ctx.to_data(ctx.sample<std::string>("padded"));
        })
    WHEN ("The data is padded for an 8-byte block size", [](context& ctx) {
            try {
                mud::crypto::pkcs7_padding padder(8);
                ctx.unpadded = padder.unpad(ctx.padded);
            }
            catch(mud::crypto::padding_error ex) {
                ctx.exception_raised = true;
            }
        })
    THEN ("A padding excpetion is thrown", [](context& ctx) {
            ASSERT(true, ctx.exception_raised);
        })
    SAMPLES(std::string)
        HEADINGS("padded")
        SAMPLE("ae609181f34b850102")
        SAMPLE("bf090909090909090909")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
