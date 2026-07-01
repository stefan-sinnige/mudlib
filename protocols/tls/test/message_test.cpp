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
    mud::core::buffer to_data(const std::string& s) {
        mud::core::buffer d;
        std::stringstream sstr(s);
        sstr >> d;
        return d;
    }

    /* Convert from string to random */
    mud::crypto::random_t to_random(const std::string& s) {
        mud::crypto::random_t r;
        std::stringstream sstr(s);
        sstr >> r;
        return r;
    }

    /* Convert from string to key */
    mud::crypto::key_t to_key(const std::string& s) {
        mud::crypto::key_t k;
        std::stringstream sstr(s);
        sstr >> k;
        return k;
    }

    /* Convert from data to string */
    std::string to_string(const mud::core::buffer& d) {
        std::stringstream sstr;
        sstr << d;
        return sstr.str();
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
        ctx.istr >> ctx.record;
      })
    THEN ("The record header is defined accordingly", [](context& ctx){
        ASSERT(mud::tls::record_content_type_t::handshake,
               ctx.record->content_type());
        ASSERT(mud::tls::protocol_version_t::tls_10,
               ctx.record->legacy_record_version());
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
        ASSERT(mud::tls::handshake_type_t::client_hello,
               ctx.handshake->msg_type());
        ASSERT(244, ctx.handshake->length());
      })

  SCENARIO("A default ClientHello message can be constructed")
    GIVEN("A ClientHello message with default setting", [](context& ctx){
            ctx.client_hello = std::make_shared<mud::tls::client_hello>();
      })
    WHEN ("The message is examined", [](context &ctx) {})
    THEN ("The default content values are set", [](context& ctx) {
            ASSERT(mud::tls::protocol_version_t::tls_10,
                   ctx.client_hello->legacy_record_version());
            ASSERT(mud::tls::protocol_version_t::tls_12,
                   ctx.client_hello->legacy_version());
            ASSERT(32, ctx.client_hello->random().size());
      })

  SCENARIO("A default ServerHello message can be constructed")
    GIVEN("A ServerHello message with default setting", [](context& ctx){
            ctx.server_hello = std::make_shared<mud::tls::server_hello>();
      })
    WHEN ("The message is examined", [](context &ctx) {})
    THEN ("The default content values are set", [](context& ctx) {
            ASSERT(mud::tls::protocol_version_t::tls_12,
                   ctx.server_hello->legacy_record_version());
            ASSERT(mud::tls::protocol_version_t::tls_12,
                   ctx.server_hello->legacy_version());
            ASSERT(32, ctx.server_hello->random().size());
      })

  SCENARIO("A ClientHello message can be written")
    GIVEN("A defined ClientHello message", [](context& ctx){
        ctx.client_hello = std::make_shared<mud::tls::client_hello>();
        ctx.client_hello->random(ctx.to_random(
                "000102030405060708090a0b0c0d0e0f"
                "101112131415161718191a1b1c1d1e1f"));
        ctx.client_hello->legacy_session_id(ctx.to_data(
                "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
                "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"));
        auto exts = ctx.client_hello->extensions();
        {
            std::vector<mud::tls::client_key_share::entry_t> key_shares;
            mud::tls::client_key_share::entry_t key_share =
                { mud::tls::key_exchange_group_t::x25519,
                  ctx.to_key("358072d6365880d1aeea329adf912138"
                             "3851ed21a28e3b75e965d0d2cd166254") };
            key_shares.push_back(key_share);
            auto ext = std::make_shared<mud::tls::client_key_share>();
            ext->shares(key_shares);
            exts.push_back(ext);
        }
        ctx.client_hello->extensions(exts);
      })
    WHEN ("The message is written", [](context& ctx){
        ctx.ostr << *ctx.client_hello;
      })
    THEN ("The message is formatted accordingly", [](context& ctx){
        const char* expected = 
            "0303000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d"
            "1e1f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfc"
            "fdfeff000613011302130401000031002b0003020304003300260024001d0020"
            "358072d6365880d1aeea329adf9121383851ed21a28e3b75e965d0d2cd166254";
        std::string str = ctx.ostr.str();
        mud::core::buffer data((const uint8_t*)str.data(), str.size());
        ASSERT(expected, ctx.to_string(data));
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
        ASSERT(mud::tls::protocol_version_t::tls_12,
               ctx.client_hello->legacy_version());
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

        /* Extensions (only the supported ones can be queried. There are 10 in
         * the message but only 6 are supoprted. */
        auto extensions = ctx.client_hello->extensions();
        ASSERT(6, extensions.size());
        size_t idx = 0;

        /* Server-name extension */
        {
            auto ext = std::dynamic_pointer_cast<mud::tls::server_name>(
                    extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(1, ext->names().size());
            ASSERT("example.ulfheim.net", ext->names()[0]);
        }
        ++idx;

        /* Supported-groups extension */
        {
            auto ext = std::dynamic_pointer_cast<mud::tls::supported_groups>(
                    extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(10, ext->groups().size());
            ASSERT(mud::tls::key_exchange_group_t::x25519, ext->groups()[0]);
            ASSERT(0x0017, (uint16_t)ext->groups()[1]);
            ASSERT(0x001e, (uint16_t)ext->groups()[2]);
            ASSERT(0x0019, (uint16_t)ext->groups()[3]);
            ASSERT(0x0018, (uint16_t)ext->groups()[4]);
            ASSERT(0x0100, (uint16_t)ext->groups()[5]);
            ASSERT(0x0101, (uint16_t)ext->groups()[6]);
            ASSERT(0x0102, (uint16_t)ext->groups()[7]);
            ASSERT(0x0103, (uint16_t)ext->groups()[8]);
            ASSERT(0x0104, (uint16_t)ext->groups()[9]);
        }
        ++idx;

        /* Signature-algorithms extension */
        {
            auto ext = std::dynamic_pointer_cast<mud::tls::signature_algorithms>
                    (extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(14, ext->algorithms().size());
            ASSERT(0x0403, (uint16_t)ext->algorithms()[ 0]);
            ASSERT(0x0503, (uint16_t)ext->algorithms()[ 1]);
            ASSERT(0x0603, (uint16_t)ext->algorithms()[ 2]);
            ASSERT(0x0807, (uint16_t)ext->algorithms()[ 3]);
            ASSERT(0x0808, (uint16_t)ext->algorithms()[ 4]);
            ASSERT(0x0809, (uint16_t)ext->algorithms()[ 5]);
            ASSERT(0x080a, (uint16_t)ext->algorithms()[ 6]);
            ASSERT(0x080b, (uint16_t)ext->algorithms()[ 7]);
            ASSERT(0x0804, (uint16_t)ext->algorithms()[ 8]);
            ASSERT(0x0805, (uint16_t)ext->algorithms()[ 9]);
            ASSERT(0x0806, (uint16_t)ext->algorithms()[10]);
            ASSERT(mud::tls::signature_algorithm_t::rsa_pkcs1_sha256,
                   ext->algorithms()[11]);
            ASSERT(0x0501, (uint16_t)ext->algorithms()[12]);
            ASSERT(0x0601, (uint16_t)ext->algorithms()[13]);
        }
        ++idx;

        /* Supported-versions extension */
        {
            auto ext = std::dynamic_pointer_cast<
                    mud::tls::client_supported_versions>(extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(1, ext->versions().size());
            ASSERT(mud::tls::protocol_version_t::tls_13, ext->versions()[0]);
        }
        ++idx;

        /* PSK key exchange modes extension */
        {
            auto ext = std::dynamic_pointer_cast<
                    mud::tls::psk_key_exchange_modes>(extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(1, ext->modes().size());
            ASSERT(mud::tls::key_exchange_mode_t::psk_dhe_ke, ext->modes()[0]);
        }
        ++idx;

        /* Key-share extension */
        {
            auto ext = std::dynamic_pointer_cast<mud::tls::client_key_share>(
                    extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(1, ext->shares().size());
            ASSERT(mud::tls::key_exchange_group_t::x25519,
                   ext->shares()[0].first);
            ASSERT(ctx.to_data("358072d6365880d1aeea329adf912138"
                               "3851ed21a28e3b75e965d0d2cd166254"),
                   ext->shares()[0].second);
        }
        ++idx;
      })

  SCENARIO("A ServerHello message can be written")
    GIVEN("A defined ServerHello message", [](context& ctx){
      })
    WHEN ("The message is written", [](context& ctx){
      })
    THEN ("The message is formatted accordingly", [](context& ctx){
        const char* expected = 
            "0303707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d"
            "8e8f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfc"
            "fdfeff130200002e002b0002030400330024001d00209fd7ad6dcff4298dd3f9"
            "6d5b1b2af910a0535b1488d7f8fabb349a982880b615";
      })

  SCENARIO("A ServerHello message can be read")
    GIVEN("A binary ServerHello message", [](context& ctx){
        const char* message = 
            "0303707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d"
            "8e8f20e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfc"
            "fdfeff130200002e002b0002030400330024001d00209fd7ad6dcff4298dd3f9"
            "6d5b1b2af910a0535b1488d7f8fabb349a982880b615";
        auto data = ctx.to_data(message);
        std::string str((const char*)data.data(), data.size());
        ctx.istr = std::istringstream(str);
      })
    WHEN ("The message is read", [](context& ctx){
        ctx.server_hello = std::make_shared<mud::tls::server_hello>();
        ctx.istr >> *ctx.server_hello;
      })
    THEN ("The message is defined accordingly", [](context& ctx){
        ASSERT(mud::tls::protocol_version_t::tls_12,
               ctx.server_hello->legacy_version());
        ASSERT(ctx.to_data("707172737475767778797a7b7c7d7e7f"
                           "808182838485868788898a8b8c8d8e8f"),
               ctx.server_hello->random());
        ASSERT(ctx.to_data("e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
                           "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"),
               ctx.server_hello->legacy_session_id());
        ASSERT(mud::tls::cipher_suite_t::tls_aes_256_gcm_sha384,
               ctx.server_hello->cipher_suite());
        ASSERT(mud::tls::compression_method_t::null,
               ctx.server_hello->legacy_compression_method());

        /* Extensions. */
        auto extensions = ctx.server_hello->extensions();
        ASSERT(2, extensions.size());
        size_t idx = 0;

        /* Supported-versions extension */
        {
            auto ext = std::dynamic_pointer_cast<
                    mud::tls::server_supported_versions>(extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(mud::tls::protocol_version_t::tls_13, ext->version());
        }
        ++idx;

        /* Key-share extension */
        {
            auto ext = std::dynamic_pointer_cast<mud::tls::server_key_share>(
                    extensions[idx]);
            ASSERT(true, ext != nullptr);
            ASSERT(mud::tls::key_exchange_group_t::x25519,
                   ext->share().first);
            ASSERT(ctx.to_data("9fd7ad6dcff4298dd3f96d5b1b2af910"
                               "a0535b1488d7f8fabb349a982880b615"),
                   ext->share().second);
        }
        ++idx;
      })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
