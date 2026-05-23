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

#include "mud/core/endian.h"
#include "mud/protocols/tls/exception.h"
#include "mud/protocols/tls/message.h"
#include <stdexcept>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

BEGIN_MUDLIB_TLS_NS

/* SSL 3.0 was given version 0x0300, and TLS 3.0 started off as SSL 3.1, was
 * given the next one up. Note that the version number in the message are
 * deprecated in favour of the 'Supported Version' extension. The legacy version
 * numbers are largely hard-coded now. */
const uint16_t tls_10_protocol_version = 0x0301;
const uint16_t tls_11_protocol_version = 0x0302;
const uint16_t tls_12_protocol_version = 0x0303;
const uint16_t tls_13_protocol_version = 0x0304;

/* ====================================================================== 
 * Record
 * ====================================================================== */

record::record()
    : _content_type(content_type_t::invalid)
    , _legacy_record_version(tls_10_protocol_version)
    , _length(0)
    , _encrypted(true)
{
}

record::record(content_type_t type, uint16_t legacy_version)
    : _content_type(type)
    , _legacy_record_version(legacy_version)
    , _encrypted(true)
{
}

/* ====================================================================== 
 * Handshake
 * ====================================================================== */

handshake::handshake()
    : _msg_type(type_t::unassigned)
    , _length(0)
{
}

handshake::handshake(type_t type)
    : record(record::content_type_t::handshake, tls_12_protocol_version)
    , _msg_type(type)
    , _length(0)
{
}

/* ====================================================================== 
 * Handshake - Client Hello
 * ====================================================================== */

client_hello::client_hello()
    : handshake(handshake::type_t::client_hello)
    , _legacy_version(tls_12_protocol_version)
    , _random(32)
{
    /* For compatibility reasons, it is common for the Client Hello message
     * to define TLS 1.0 for the record lgeacy version. */
    legacy_record_version(tls_10_protocol_version);

    /* Supported cipher suites */
    _cipher_suites.push_back(cipher_suite_t::tls_aes_128_gcm_sha256);
    _cipher_suites.push_back(cipher_suite_t::tls_aes_256_gcm_sha384);
    _cipher_suites.push_back(cipher_suite_t::tls_aes_128_ccm_sha256);

    /* Legacy compression methods */
    _legacy_compression_methods.push_back(compression_method_t::null);
}

/* ====================================================================== 
 * Handshake - Server Hello
 * ====================================================================== */

server_hello::server_hello()
    : handshake(handshake::type_t::server_hello)
    , _legacy_version(tls_12_protocol_version)
    , _random(32)
{
}

/* ====================================================================== 
 * Streaming operators
 * ====================================================================== */

std::istream&
operator>>(std::istream& istr, record& rec)
{
    /* Get the content type byte */
    int ch = istr.get();
    if (ch == std::istream::traits_type::eof()) {
        throw invalid_message("No record content-type");
    }
    switch (static_cast<record::content_type_t>(ch)) {
        case record::content_type_t::change_cipher_spec:
        case record::content_type_t::alert:
        case record::content_type_t::handshake:
        case record::content_type_t::application_data:
            rec._content_type = static_cast<record::content_type_t>(ch);
            break;
        default:
            throw invalid_message("Unrecognized record content-type");
    }

    /* Get the legacy version */
    uint16_t value_uint16;
    if (!istr.read((char*)&value_uint16, 2)) {
        throw invalid_message("No record legacy version");
    }
    rec._legacy_record_version = endian::convert(endian::native(),
            endian_t::big, value_uint16);

    /* Get the length */
    if (!istr.read((char*)&value_uint16, 2)) {
        throw invalid_message("No record length");
    }
    rec._length = endian::convert(endian_t::big, endian::native(),
                                  value_uint16);
    return istr;
}

std::istream&
operator>>(std::istream& istr, handshake& msg)
{
    /* Get the message handshake type byte */
    int ch = istr.get();
    if (ch == std::istream::traits_type::eof()) {
        throw invalid_message("No handshake message-type");
    }
    switch (static_cast<handshake::type_t>(ch)) {
        case handshake::type_t::client_hello:
        case handshake::type_t::server_hello:
        case handshake::type_t::new_session_ticket:
        case handshake::type_t::end_of_early_data:
        case handshake::type_t::encrypted_extensions:
        case handshake::type_t::certificate:
        case handshake::type_t::certificate_request:
        case handshake::type_t::finished:
        case handshake::type_t::key_update:
        case handshake::type_t::message_hash:
            msg._msg_type = static_cast<handshake::type_t>(ch);
            break;
        default:
            throw invalid_message("Unrecognized handshake message-type");
    }

    /* Get the length */
    uint32_t value_uint32 = 0;
    if (!istr.read(((char*)&value_uint32)+1, 3)) {
        throw invalid_message("No handshake length");
    }
    msg._length = endian::convert(
            endian_t::big, endian::native(), value_uint32);
    return istr;
}

std::istream&
operator>>(std::istream& istr, client_hello& msg)
{
    /* Get the legacy version */
    uint16_t value_uint16;
    if (!istr.read((char*)&value_uint16, 2)) {
        throw invalid_message("No client-hello legacy version");
    }
    msg._legacy_version = endian::convert(endian::native(), endian_t::big,
                                          value_uint16);

    /* The 32-byte random */
    mud::crypto::data_t random(32);
    if (!istr.read((char*)random.data(), 32)) {
        throw invalid_message("No client-hello random");
    }
    msg._random = random;

    /* The legacy session ID */
    int ch = istr.get();
    if (ch == std::istream::traits_type::eof()) {
        throw invalid_message("No client-hello session ID");
    }
    if (ch > 32) {
        throw invalid_message("Invalid client-hello session ID length");
    }
    mud::crypto::data_t legacy_session_id(ch);
    if (!istr.read((char*)legacy_session_id.data(), 32)) {
        throw invalid_message("No client-hello session ID");
    }
    msg._legacy_session_id = legacy_session_id;

    /* Cipher suites */
    if (!istr.read((char*)&value_uint16, 2)) {
        throw invalid_message("No client-hello cipher suites");
    }
    uint16_t length = endian::convert(endian::native(), endian_t::big,
                                      value_uint16);
    msg._cipher_suites.clear();
    for (int i = 0; i < length / 2; ++i) {
        if (!istr.read((char*)&value_uint16, 2)) {
            throw invalid_message("No client-hello cipher suite");
        }
        uint16_t suite = endian::convert(endian::native(), endian_t::big,
                                         value_uint16);
        msg._cipher_suites.push_back(static_cast<cipher_suite_t>(suite));
    }

    /* Compression methods */
    ch = istr.get();
    if (ch == std::istream::traits_type::eof()) {
        throw invalid_message("No client-hello compression methods");
    }
    length = ch;
    std::cout << "compression methods = " << length << std::endl;
    msg._legacy_compression_methods.clear();
    for (int i = 0; i < length; ++i) {
        ch = istr.get();
        if (ch == std::istream::traits_type::eof()) {
            throw invalid_message("No client-hello compression methods");
        }
        msg._legacy_compression_methods.push_back(
                static_cast<compression_method_t>(ch));
    }

    return istr;
}

std::istream&
operator>>(std::istream& istr, server_hello& msg)
{
    return istr;
}

std::istream&
operator>>(std::istream& istr, std::shared_ptr<record>& msg)
{
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const record& msg)
{
    return ostr;
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
