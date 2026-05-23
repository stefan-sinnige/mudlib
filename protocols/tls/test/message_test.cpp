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

#include "mud/crypto/types.h"
#include "mud/protocols/tls/message.h"
#include "mud/test.h"
#include <sstream>
#include <string>
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

    /* Input string stream */
    std::istringstream istr;

    /* Output string stream */
    std::ostringstream ostr;

    /* A record message */
    std::shared_ptr<mud::tls::record> record;

    /* Handshake messages */
    std::shared_ptr<mud::tls::handshake> handshake;
    std::shared_ptr<mud::tls::client_hello> client_hello;
    std::shared_ptr<mud::tls::server_hello> server_hello;

END_CONTEXT()

FEATURE("TLS Message")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An TLS Message", [](context&){ });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("An TLS Message type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
      [](context& ctx) {
        ASSERT(true, std::is_default_constructible<
               mud::tls::handshake>::value);
      })
    THEN ("The type is copy-constructible",
      [](context& ctx) {
        ASSERT(true, std::is_copy_constructible<
               mud::tls::handshake>::value);
      })
    THEN ("The type is assignable",
      [](context& ctx) {
        ASSERT(true, std::is_assignable<
              mud::tls::handshake,
              mud::tls::handshake>::value);
      })

  SCENARIO("A TLS Record (header-only) can be read")
    GIVEN("A binary TLS message", [](context& ctx){
        const char* message = 
            "16030100f8010000f40303000102030405060708090a0b0c0d0e0f1011121314"
            "15161718191a1b1c1d1e1f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3"
            "f4f5f6f7f8f9fafbfcfdfeff000813021303130100ff010000a3000000180016"
            "0000136578616d706c652e756c666865696d2e6e6574000b000403000102000a"
            "00160014001d0017001e00190018010001010102010301040023000000160000"
            "00170000000d001e001c040305030603080708080809080a080b080408050806"
            "040105010601002b0003020304002d00020101003300260024001d0020358072"
            "d6365880d1aeea329adf9121383851ed21a28e3b75e965d0d2cd166254";
        auto data = ctx.to_data(message);
        std::string str((const char*)data.data(), data.size());
        ctx.istr = std::istringstream(str);
      })
    WHEN ("The record header is read", [](context& ctx){
        ctx.record = std::make_shared<mud::tls::record>();
        ctx.istr >> *ctx.record;
      })
    THEN ("The record header is defined accordingly", [](context& ctx){
        ASSERT(mud::tls::record::content_type_t::handshake,
               ctx.record->content_type());
        ASSERT(0x0301, ctx.record->legacy_record_version());
        ASSERT(248, ctx.record->length());
      })

  SCENARIO("A TLS Handshake (header-only) can be read")
    GIVEN("A binary TLS message", [](context& ctx){
        const char* message = 
            "010000f40303000102030405060708090a0b0c0d0e0f10111213141516171819"
            "1a1b1c1d1e1f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8"
            "f9fafbfcfdfeff000813021303130100ff010000a30000001800160000136578"
            "616d706c652e756c666865696d2e6e6574000b000403000102000a0016001400"
            "1d0017001e001900180100010101020103010400230000001600000017000000"
            "0d001e001c040305030603080708080809080a080b0804080508060401050106"
            "01002b0003020304002d00020101003300260024001d0020358072d6365880d1"
            "aeea329adf9121383851ed21a28e3b75e965d0d2cd166254";
        auto data = ctx.to_data(message);
        std::string str((const char*)data.data(), data.size());
        ctx.istr = std::istringstream(str);
      })
    WHEN ("The handshake header is read", [](context& ctx){
        ctx.handshake = std::make_shared<mud::tls::handshake>();
        ctx.istr >> *ctx.handshake;
      })
    THEN ("The handshake header is defined accordingly", [](context& ctx){
        ASSERT(mud::tls::handshake::type_t::client_hello,
               ctx.handshake->msg_type());
        ASSERT(244, ctx.handshake->length());
      })

  SCENARIO("A default ClientHello message can be constructed")
    GIVEN("A ClientHello message with default setting", [](context& ctx){
            ctx.client_hello = std::make_shared<mud::tls::client_hello>();
      })
    WHEN ("The message is examined", [](context &ctx) {})
    THEN ("The default content values are set", [](context& ctx) {
            ASSERT(0x0301, ctx.client_hello->legacy_record_version());
            ASSERT(0x0303, ctx.client_hello->legacy_version());
            ASSERT(32, ctx.client_hello->random().size());
      })

  SCENARIO("A default ServerHello message can be constructed")
    GIVEN("A ServerHello message with default setting", [](context& ctx){
            ctx.server_hello = std::make_shared<mud::tls::server_hello>();
      })
    WHEN ("The message is examined", [](context &ctx) {})
    THEN ("The default content values are set", [](context& ctx) {
            ASSERT(0x0303, ctx.server_hello->legacy_record_version());
            ASSERT(0x0303, ctx.server_hello->legacy_version());
            ASSERT(32, ctx.server_hello->random().size());
      })

  SCENARIO("A ClientHello message can be written")
    GIVEN("A defined ClientHello message", [](context& ctx){
      })
    WHEN ("The message is written", [](context& ctx){
      })
    THEN ("The message is formatted accordingly", [](context& ctx){
        const char* expected = 
            "0303000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d"
            "1e1f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfc"
            "fdfeff000813021303130100ff010000a30000001800160000136578616d706c"
            "652e756c666865696d2e6e6574000b000403000102000a00160014001d001700"
            "1e0019001801000101010201030104002300000016000000170000000d001e00"
            "1c040305030603080708080809080a080b080408050806040105010601002b00"
            "03020304002d00020101003300260024001d0020358072d6365880d1aeea329a"
            "df9121383851ed21a28e3b75e965d0d2cd166254";
      })

  SCENARIO("A ClientHello message can be read")
    GIVEN("A binary ClientHello message", [](context& ctx){
        const char* message = 
            "0303000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d"
            "1e1f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfc"
            "fdfeff000813021303130100ff010000a30000001800160000136578616d706c"
            "652e756c666865696d2e6e6574000b000403000102000a00160014001d001700"
            "1e0019001801000101010201030104002300000016000000170000000d001e00"
            "1c040305030603080708080809080a080b080408050806040105010601002b00"
            "03020304002d00020101003300260024001d0020358072d6365880d1aeea329a"
            "df9121383851ed21a28e3b75e965d0d2cd166254";
        auto data = ctx.to_data(message);
        std::string str((const char*)data.data(), data.size());
        ctx.istr = std::istringstream(str);
      })
    WHEN ("The message is read", [](context& ctx){
        ctx.client_hello = std::make_shared<mud::tls::client_hello>();
        ctx.istr >> *ctx.client_hello;
      })
    THEN ("The message is defined accordingly", [](context& ctx){
        ASSERT(0x0303, ctx.client_hello->legacy_version());
        ASSERT(ctx.to_data("000102030405060708090a0b0c0d0e0f"
                           "101112131415161718191a1b1c1d1e1f"),
               ctx.client_hello->random());
        ASSERT(ctx.to_data("e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
                           "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"),
               ctx.client_hello->legacy_session_id());
        auto cipher_suites = ctx.client_hello->cipher_suites();
        ASSERT(4, cipher_suites.size());
        ASSERT(mud::tls::cipher_suite_t::tls_aes_256_gcm_sha384,
               cipher_suites[0]);
        ASSERT(mud::tls::cipher_suite_t::tls_chacha20_poly1305_sha256,
               cipher_suites[1]);
        ASSERT(mud::tls::cipher_suite_t::tls_aes_128_gcm_sha256,
               cipher_suites[2]);
        ASSERT(0x00ff,
               static_cast<uint16_t>(cipher_suites[3]));
        auto compr_methods = ctx.client_hello->legacy_compression_methods();
        ASSERT(1, compr_methods.size());
        ASSERT(mud::tls::compression_method_t::null, compr_methods[0]);
      })

  SCENARIO("A ServerHello message can be written")
    GIVEN("A defined ServerHello message", [](context& ctx){
      })
    WHEN ("The message is written", [](context& ctx){
      })
    THEN ("The message is formatted accordingly", [](context& ctx){
        const char* expected = 
            "160303007a020000760303707172737475767778797a7b7c7d7e7f8081828384"
            "85868788898a8b8c8d8e8f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3"
            "f4f5f6f7f8f9fafbfcfdfeff130200002e002b0002030400330024001d00209f"
            "d7ad6dcff4298dd3f96d5b1b2af910a0535b1488d7f8fabb349a982880b615";
      })

  SCENARIO("A ServerHello message can be read")
    GIVEN("A binary ServerHello message", [](context& ctx){
        const char* message = 
            "160303007a020000760303707172737475767778797a7b7c7d7e7f8081828384"
            "85868788898a8b8c8d8e8f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3"
            "f4f5f6f7f8f9fafbfcfdfeff130200002e002b0002030400330024001d00209f"
            "d7ad6dcff4298dd3f96d5b1b2af910a0535b1488d7f8fabb349a982880b615";
      })
    WHEN ("The message is read", [](context& ctx){
      })
    THEN ("The message is defined accordingly", [](context& ctx){
      })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
