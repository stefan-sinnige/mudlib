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

/* ====================================================================== 
 * Record
 * ====================================================================== */

record::record()
    : _content_type(record_content_type_t::invalid)
    , _legacy_record_version(protocol_version_t::tls_10)
    , _length(0)
    , _encrypted(true)
{
}

record::record(record_content_type_t type, protocol_version_t legacy_version)
    : _content_type(type)
    , _legacy_record_version(legacy_version)
    , _encrypted(true)
{
}

/* ====================================================================== 
 * Handshake
 * ====================================================================== */

handshake::handshake()
    : _msg_type(handshake_type_t::unassigned)
    , _length(0)
{
}

handshake::handshake(handshake_type_t type)
    : record(record_content_type_t::handshake, protocol_version_t::tls_12)
    , _msg_type(type)
    , _length(0)
{
}

/* ====================================================================== 
 * Handshake - Client Hello
 * ====================================================================== */

client_hello::client_hello()
    : handshake(handshake_type_t::client_hello)
    , _legacy_version(protocol_version_t::tls_12)
    , _random(32)
{
    /* For compatibility reasons, it is common for the Client Hello message
     * to define TLS 1.0 for the record lgeacy version. */
    legacy_record_version(protocol_version_t::tls_10);

    /* Supported cipher suites */
    _cipher_suites.push_back(cipher_suite_t::tls_aes_128_gcm_sha256);
    _cipher_suites.push_back(cipher_suite_t::tls_aes_256_gcm_sha384);
    _cipher_suites.push_back(cipher_suite_t::tls_aes_128_ccm_sha256);

    /* Legacy compression methods */
    _legacy_compression_methods.push_back(compression_method_t::null);

    /* Mandatory extensions */
    auto ext = extension_factory::instance().create(
            { msg_type(), extension_type_t::supported_versions });
    _extensions.push_back(std::move(ext));
}

std::ostream&
operator<<(std::ostream& ostr, const client_hello& msg)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* Write the legacy version */
    uint16_t ui16 = endian::convert(endian::native(), endian_t::big,
            static_cast<uint16_t>(msg._legacy_version));
    ostr.write((const char*)&ui16, 2);

    /* The 32-byte random */
    ostr.write((const char*)msg._random.data(), msg._random.size());

    /* The session ID random */
    ostr.put(msg._legacy_session_id.size());
    ostr.write((const char*)msg._legacy_session_id.data(),
               msg._legacy_session_id.size());

    /* Cipher suites */
    ui16 = endian::convert(endian::native(), endian_t::big,
            static_cast<uint16_t>(msg._cipher_suites.size() * 2));
    ostr.write((const char*)&ui16, 2);
    for (auto suite: msg._cipher_suites) {
        ui16 = endian::convert(endian::native(), endian_t::big,
                static_cast<uint16_t>(suite));
        ostr.write((const char*)&ui16, 2);
    }

    /* Compression methods */
    ostr.put(msg._legacy_compression_methods.size());
    for (auto method: msg._legacy_compression_methods) {
        ostr.put(static_cast<uint8_t>(method));
    }

    /* Extensions */
    ui16 = 0;
    for (auto ext: msg._extensions) {
        ui16 += ext->size();
    }
    ui16 = endian::convert(endian::native(), endian_t::big, ui16);
    ostr.write((const char*)&ui16, 2);
    for (auto ext: msg._extensions) {
        ext->write(ostr);
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, client_hello& msg)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* The legacy version */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    msg._legacy_version = static_cast<protocol_version_t>(
            endian::convert(endian_t::big, endian::native(), ui16));

    /* The 32-byte random */
    mud::core::buffer random(32);
    istr.read((char*)random.data(), 32);
    msg._random = random;

    /* The legacy session ID */
    int ch = istr.get();
    if (ch > 32) {
        throw invalid_message("Invalid client-hello session ID length");
    }
    mud::core::buffer legacy_session_id(ch);
    istr.read((char*)legacy_session_id.data(), 32);
    msg._legacy_session_id = legacy_session_id;

    /* Cipher suites */
    istr.read((char*)&ui16, 2);
    uint16_t length = endian::convert(endian_t::big, endian::native(), ui16);
    msg._cipher_suites.clear();
    for (int i = 0; i < length / 2; ++i) {
        istr.read((char*)&ui16, 2);
        uint16_t suite = endian::convert(endian_t::big, endian::native(), ui16);
        msg._cipher_suites.push_back(static_cast<cipher_suite_t>(suite));
    }

    /* Compression methods */
    ch = istr.get();
    length = ch;
    msg._legacy_compression_methods.clear();
    for (int i = 0; i < length; ++i) {
        ch = istr.get();
        msg._legacy_compression_methods.push_back(
                static_cast<compression_method_t>(ch));
    }

    /* Extensions */
    istr.read((char*)&ui16, 2);
    length = endian::convert(endian_t::big, endian::native(), ui16);
    msg._extensions.clear();
    auto end = (uint16_t)istr.tellg() + length;
    while (istr.tellg() < end) {
        /* Get the first two bytes to retrieve the extension type and use their
         * streaming operator to read it. Put the two bytes back as the
         * extension will read it in. */
        int hi = istr.get();
        int lo = istr.get();
        istr.putback(lo);
        istr.putback(hi);
        uint16_t ext_type = ((hi & 0xFF) << 8) | (lo & 0xFF);

        /* Read the extension */
        try {
            auto ext = extension_factory::instance().create(
                { msg.msg_type(),
                  static_cast<extension_type_t>(ext_type) });
            ext->read(istr);
            msg._extensions.push_back(std::move(ext));
        }
        catch (...) {
            /* Unsupported extension type, skip it */
            istr.read((char*)&ui16, 2);
            istr.read((char*)&ui16, 2);
            uint16_t length = endian::convert(endian_t::big, endian::native(),
                                              ui16);
            istr.ignore(length);
        }
    }
    return istr;
}

/* ====================================================================== 
 * Handshake - Server Hello
 * ====================================================================== */

server_hello::server_hello()
    : handshake(handshake_type_t::server_hello)
    , _legacy_version(protocol_version_t::tls_12)
    , _random(32)
{
}

std::ostream&
operator>>(std::ostream& ostr, const server_hello& msg)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    return ostr;
}

std::istream&
operator>>(std::istream& istr, server_hello& msg)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the legacy version */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    msg._legacy_version = static_cast<protocol_version_t>(
            endian::convert(endian_t::big, endian::native(), ui16));

    /* The 32-byte random */
    mud::core::buffer random(32);
    istr.read((char*)random.data(), 32);
    msg._random = random;

    /* The legacy session ID */
    int ch = istr.get();
    if (ch > 32) {
        throw invalid_message("Invalid server-hello session ID length");
    }
    mud::core::buffer legacy_session_id(ch);
    istr.read((char*)legacy_session_id.data(), 32);
    msg._legacy_session_id = legacy_session_id;

    /* Cipher suite */
    istr.read((char*)&ui16, 2);
    uint16_t suite = endian::convert(endian_t::big, endian::native(),
                                     ui16);
    msg._cipher_suite = static_cast<cipher_suite_t>(suite);

    /* Compression method */
    ch = istr.get();
    msg._legacy_compression_method = static_cast<compression_method_t>(ch);
    
    /* Extensions */
    uint16_t length;
    istr.read((char*)&ui16, 2);
    length = endian::convert(endian_t::big, endian::native(), ui16);
    msg._extensions.clear();
    auto end = (uint16_t)istr.tellg() + length;
    while (istr.tellg() < end) {
        /* Get the first two bytes to retrieve the extension type and use their
         * streaming operator to read it. Put the two bytes back as the
         * extension will read it in. */
        int hi = istr.get();
        int lo = istr.get();
        istr.putback(lo);
        istr.putback(hi);
        uint16_t ext_type = ((hi & 0xFF) << 8) | (lo & 0xFF);

        /* Read the extension */
        try {
            auto ext = extension_factory::instance().create(
                { msg.msg_type(),
                  static_cast<extension_type_t>(ext_type) });
            ext->read(istr);
            msg._extensions.push_back(std::move(ext));
        }
        catch (...) {
            /* Unsupported extension type, skip it */
            istr.read((char*)&ui16, 2);
            istr.read((char*)&ui16, 2);
            uint16_t length = endian::convert(endian_t::big,
                                endian::native(), ui16);
            istr.ignore(length);
        }
    }
    return istr;
}

/* ====================================================================== 
 * Streaming operators
 * ====================================================================== */

std::istream&
operator>>(std::istream& istr, record& rec)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the content type byte */
    int ch = istr.get();
    switch (static_cast<record_content_type_t>(ch)) {
        case record_content_type_t::change_cipher_spec:
        case record_content_type_t::alert:
        case record_content_type_t::handshake:
        case record_content_type_t::application_data:
            rec._content_type = static_cast<record_content_type_t>(ch);
            break;
        default:
            throw invalid_message("Unrecognized record content-type");
    }

    /* Get the legacy version */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    rec._legacy_record_version = static_cast<protocol_version_t>(
            endian::convert(endian_t::big, endian::native(), ui16));

    /* Get the length */
    istr.read((char*)&ui16, 2);
    rec._length = endian::convert(endian_t::big, endian::native(), ui16);
    return istr;
}

std::istream&
operator>>(std::istream& istr, handshake& msg)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the message handshake type byte */
    int ch = istr.get();
    switch (static_cast<handshake_type_t>(ch)) {
        case handshake_type_t::client_hello:
        case handshake_type_t::server_hello:
        case handshake_type_t::new_session_ticket:
        case handshake_type_t::end_of_early_data:
        case handshake_type_t::encrypted_extensions:
        case handshake_type_t::certificate:
        case handshake_type_t::certificate_request:
        case handshake_type_t::finished:
        case handshake_type_t::key_update:
        case handshake_type_t::message_hash:
            msg._msg_type = static_cast<handshake_type_t>(ch);
            break;
        default:
            throw invalid_message("Unrecognized handshake message-type");
    }

    /* Get the length */
    uint32_t ui32 = 0;
    istr.read(((char*)&ui32)+1, 3);
    msg._length = endian::convert(endian_t::big, endian::native(), ui32);
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
