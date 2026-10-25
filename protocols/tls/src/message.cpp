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

#include "mud/core/bufferstream.h"
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

const size_t record_header_size = 5;

record::record()
    : _content_type(record_content_type_t::invalid)
    , _legacy_record_version(protocol_version_t::tls_10)
{
}

record::record(record_content_type_t type, protocol_version_t legacy_version)
    : _content_type(type)
    , _legacy_record_version(legacy_version)
{
}

std::istream&
record::read(std::istream& istr)
{
    /* The record header is read by the derived type (e.g. handshake or
       application data. */
    return istr;
}

std::ostream&
record::write(std::ostream& ostr) const
{
    uint16_t ui16;

    /* Write the five-byte header */
    ostr.write((const char*)&_content_type, 1);
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(_legacy_record_version));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(size() - record::size()));
    ostr.write((const char*)&ui16, 2);

    return ostr;
}

size_t
record::size() const
{
    return record_header_size;
}

std::ostream&
operator<<(std::ostream& ostr, const record& msg)
{
    msg.write(ostr);
    return ostr;
    /*
    switch (msg.content_type())
    {
        case record_content_type_t::change_cipher_spec:
            break;
        case record_content_type_t::alert:
            break;
        case record_content_type_t::handshake:
            ostr << static_cast<const handshake&>(msg);
            break;
        case record_content_type_t::application_data:
            break;
        default:
            throw invalid_message("unknown record content-type");
    }
    return ostr;
    */
}

std::istream&
operator>>(std::istream& istr, record& msg)
{
    msg.read(istr);
    return istr;
}

std::istream&
operator>>(std::istream& istr, record::ptr& msg)
{
    return istr;
}

/* ====================================================================== 
 * Handshake
 * ====================================================================== */

const size_t handshake_header_size = 4;

handshake::handshake()
    : _msg_type(handshake_type_t::unassigned)
{
}

handshake::handshake(handshake_type_t type)
    : record(record_content_type_t::handshake, protocol_version_t::tls_12)
    , _msg_type(type)
{
}

size_t
handshake::size() const
{
    return record::size() + handshake_header_size;
}

std::istream&
handshake::read(std::istream& istr)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Read the fixed five-byte header */
    record::read(istr);

    return istr;
}

std::ostream&
handshake::write(std::ostream& ostr) const
{
    uint16_t ui16;
    uint32_t ui32;

    /* Write the record header */
    record::write(ostr);

    /* Write the handshake header */
    ostr.write((const char*)&_msg_type, 1);
    ui32 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint32_t>(size() - handshake::size()));
    ostr.write(((const char*)&ui32)+1, 3);

    return ostr;
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
#if 0
    auto ext = extension_factory::instance().create(
            { msg_type(), extension_type_t::supported_versions });
    _extensions.push_back(std::move(ext));
#endif
}

client_hello::client_hello(const mud::core::buffer_view& msg)
    : handshake(handshake_type_t::client_hello)
{
    /* The type */
    auto iter = msg.cbegin();
    auto type = msg.get<uint8_t>(iter, endian_t::big);
    iter += 1;

    /* The length */
    uint32_t length = 0;
    length |= msg.get<uint8_t>(iter, endian_t::big) << 16;
    length |= msg.get<uint16_t>(iter+1, endian_t::big);
    iter += 3;
    if (length != (msg.cend() - iter)) {
        throw invalid_message("Invalid client-hello message");
    }

    /* The legacy version */
    _legacy_version = static_cast<protocol_version_t>(
            msg.get<uint16_t>(iter, endian_t::big));
    iter += 2;

    /* The 32-byte random */
    _random = msg.view(iter, iter+32);
    iter += 32;

    /* The 32-byte session ID and its length */
    uint8_t session_length = msg.get<uint8_t>(iter, endian_t::big);
    ++iter;
    _legacy_session_id = msg.view(iter, iter + session_length);
    iter += session_length;

    /* Cipher suites */
    uint16_t nr_cipher_suites = msg.get<uint16_t>(iter, endian_t::big) / 2;
    iter += 2;
    for (int i = 0; i < nr_cipher_suites; ++i) {
        _cipher_suites.push_back(static_cast<cipher_suite_t>(
                msg.get<uint16_t>(iter, endian_t::big)));
        iter += 2;
    }

    /* Compression methods */
    uint8_t nr_compr_methods = msg.get<uint8_t>(iter, endian_t::big);
    ++iter;
    for (int i = 0; i < nr_compr_methods; ++i) {
        _legacy_compression_methods.push_back(static_cast<compression_method_t>(
            msg.get<uint8_t>(iter, endian_t::big)));
        ++iter;
    }

    /* Extensions */
    uint16_t extensions_length = msg.get<uint16_t>(iter, endian_t::big);
    iter += 2;
    if (extensions_length != (msg.cend() - iter)) {
        throw invalid_message("Invalid client-hello message");
    }
    while (iter < msg.cend()) {
        /* Get the extension type and length */
        uint16_t extension_type = msg.get<uint16_t>(iter, endian_t::big);
        uint16_t extension_length = msg.get<uint16_t>(iter + 2, endian_t::big);

        /* Get the extension */
        try {
            auto ext = extension_factory::instance().create(
                    { msg_type(),
                      static_cast<extension_type_t>(extension_type) },
                    msg.view(iter, iter + 4 + extension_length));
            _extensions.push_back(std::move(ext));
        }
        catch (...) {
            /* Unsupported extension type, skip it */
        }
        iter += 4 + extension_length;
    }
    std::cout << "Extensions found that are supported: " << _extensions.size() << std::endl;

#if 0
    /* Extensions */
    istr.read((char*)&ui16, 2);
    length = endian::convert(endian_t::big, endian_t::native, ui16);
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
            uint16_t length = endian::convert(endian_t::big, endian_t::native,
                                              ui16);
            istr.ignore(length);
        }
    }
    return istr;
#endif
}

size_t
client_hello::size() const
{
    size_t size = handshake::size() + 2 + _random.size() +
                  1 + _legacy_session_id.size() +
                  2 + _cipher_suites.size() * 2 +
                  1 + _legacy_compression_methods.size();
    size += 2;
    for (const auto& ext: _extensions) {
        size += ext->size();
    }
    return size;
}

std::istream&
client_hello::read(std::istream& istr)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    handshake::read(istr);

    return istr;
}

std::ostream&
client_hello::write(std::ostream& ostr) const
{
    uint8_t ui8;
    uint16_t ui16;

    /* Write the message to a buffer-stream, so we can set various length fields
     * afterwards. */
    mud::core::obufferstream bstr;

    /* Write the record and handshake headers */
    handshake::write(bstr);

    /* The legacy version */
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(_legacy_version));
    bstr.write((const char*)&ui16, 2);

    /* The random data */
    bstr.write((const char*)_random.data(), _random.size());

    /* The session ID */
    ui8 = _legacy_session_id.size();
    bstr.write((const char*)&ui8, 1);
    bstr.write((const char*)_legacy_session_id.data(), ui8);

    /* Cipher suites */
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(_cipher_suites.size() * 2));
    bstr.write((const char*)&ui16, 2);
    for (auto cipher: _cipher_suites) {
        ui16 = endian::convert(endian_t::native, endian_t::big,
                static_cast<uint16_t>(cipher));
        bstr.write((const char*)&ui16, 2);
    }

    /* Compression methods */
    ui8 = _legacy_compression_methods.size();
    bstr.write((const char*)&ui8, 1);
    for (auto method: _legacy_compression_methods) {
        bstr.write((const char*)&method, 1);
    }

    /* Extensions with a placeholder total extension length */
    ui16 = 0;
    for (const auto& ext: _extensions) {
        ui16 += ext->size();
    }
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(ui16));
    bstr.write((const char*)&ui16, 2);
    for (const auto& ext: _extensions) {
        bstr << *ext;
    }

    /* Copy the finalised message to the output stream */
    ostr.write((const char*)bstr.buf().data(), bstr.buf().size());

    return ostr;
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

size_t
server_hello::size() const
{
    // TBD
    return 0;
}

std::istream&
server_hello::read(std::istream& istr)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    handshake::read(istr);

    return istr;
}

std::ostream&
server_hello::write(std::ostream& ostr) const
{
    record::write(ostr);
    handshake::write(ostr);
    return ostr;
}

/* ====================================================================== 
 * Streaming Operators
 * ====================================================================== */

#if 0
std::istream&
operator>>(std::istream& istr, record::ptr& rec)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Read te record header */
    mud::core::buffer buffer(5);
    istr.read((char*)buffer.data(), buffer.size());
    if (istr.gcount() != buffer.size()) {
        throw invalid_message("unable to read TLS record header");
    }

    /* Read the remainder of the record */
    auto length = buffer.get<uint16_t>(3, endian_t::big);
    buffer.resize(5 + length);
    istr.read((char*)buffer.data() + 5, length);
    if (istr.gcount() != length) {
        throw invalid_message("unable to read TLS record body");
    }

    /* Create an instance of the message type */
    switch (static_cast<record_content_type_t>(buffer[0])) {
        case record_content_type_t::change_cipher_spec:
            break;
        case record_content_type_t::alert:
            break;
        case record_content_type_t::handshake:
            switch (static_cast<handshake_type_t>(buffer[5])) {
                case handshake_type_t::client_hello:
                    rec = std::make_shared<client_hello>(
                        buffer.view(buffer.cbegin()+5, buffer.cend()));
                    break;
                case handshake_type_t::server_hello:
                case handshake_type_t::new_session_ticket:
                case handshake_type_t::end_of_early_data:
                case handshake_type_t::encrypted_extensions:
                case handshake_type_t::certificate:
                case handshake_type_t::certificate_request:
                case handshake_type_t::finished:
                case handshake_type_t::key_update:
                case handshake_type_t::message_hash:
                    break;
                default:
                    throw invalid_message("unknown handshake message-type");
            }
        case record_content_type_t::application_data:
            break;
        default:
            throw invalid_message("unknown record content-type");
    }

    /* Parse the buffer */
    // rec->parse(buffer);

    return istr;
#if 0 
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
            endian::convert(endian_t::big, endian_t::native, ui16));

    /* Get the length */
    istr.read((char*)&ui16, 2);
    rec._length = endian::convert(endian_t::big, endian_t::native, ui16);
#endif
}

#if 0
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
            endian::convert(endian_t::big, endian_t::native, ui16));

    /* Get the length */
    istr.read((char*)&ui16, 2);
    rec._length = endian::convert(endian_t::big, endian_t::native, ui16);
    return istr;
}
#endif

std::ostream&
operator<<(std::ostream& ostr, const handshake& msg)
{
    switch (msg.msg_type()) {
        case handshake_type_t::client_hello:
            ostr << static_cast<const client_hello&>(msg);
            break;
        case handshake_type_t::server_hello:
            ostr << static_cast<const server_hello&>(msg);
            break;
        case handshake_type_t::new_session_ticket:
        case handshake_type_t::end_of_early_data:
        case handshake_type_t::encrypted_extensions:
        case handshake_type_t::certificate:
        case handshake_type_t::certificate_request:
        case handshake_type_t::finished:
        case handshake_type_t::key_update:
        case handshake_type_t::message_hash:
            break;
        default:
            throw invalid_message("Unrecognized handshake message-type");
    }
    return ostr;
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
    msg._length = endian::convert(endian_t::big, endian_t::native, ui32);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const client_hello& msg)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* Write the legacy version */
    uint16_t ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(msg.legacy_version()));
    ostr.write((const char*)&ui16, 2);

    /* The 32-byte random */
    ostr.write((const char*)msg.random().data(), msg.random().size());

    /* The session ID random */
    ostr.put(msg.legacy_session_id().size());
    ostr.write((const char*)msg.legacy_session_id().data(),
               msg.legacy_session_id().size());

    /* Cipher suites */
    ui16 = endian::convert(endian_t::native, endian_t::big,
            static_cast<uint16_t>(msg.cipher_suites().size() * 2));
    ostr.write((const char*)&ui16, 2);
    for (auto suite: msg.cipher_suites()) {
        ui16 = endian::convert(endian_t::native, endian_t::big,
                static_cast<uint16_t>(suite));
        ostr.write((const char*)&ui16, 2);
    }

    /* Compression methods */
    ostr.put(msg.legacy_compression_methods().size());
    for (auto method: msg.legacy_compression_methods()) {
        ostr.put(static_cast<uint8_t>(method));
    }

    /* Extensions */
    ui16 = 0;
    for (auto ext: msg.extensions()) {
        ui16 += ext->size();
    }
    ui16 = endian::convert(endian_t::native, endian_t::big, ui16);
    ostr.write((const char*)&ui16, 2);
    for (auto ext: msg.extensions()) {
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
            endian::convert(endian_t::big, endian_t::native, ui16));

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
    uint16_t length = endian::convert(endian_t::big, endian_t::native, ui16);
    msg._cipher_suites.clear();
    for (int i = 0; i < length / 2; ++i) {
        istr.read((char*)&ui16, 2);
        uint16_t suite = endian::convert(endian_t::big, endian_t::native, ui16);
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
    length = endian::convert(endian_t::big, endian_t::native, ui16);
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
#if 0
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
            uint16_t length = endian::convert(endian_t::big, endian_t::native,
                                              ui16);
            istr.ignore(length);
        }
#endif
    }
    return istr;
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
            endian::convert(endian_t::big, endian_t::native, ui16));

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
    uint16_t suite = endian::convert(endian_t::big, endian_t::native,
                                     ui16);
    msg._cipher_suite = static_cast<cipher_suite_t>(suite);

    /* Compression method */
    ch = istr.get();
    msg._legacy_compression_method = static_cast<compression_method_t>(ch);
    
    /* Extensions */
    uint16_t length;
    istr.read((char*)&ui16, 2);
    length = endian::convert(endian_t::big, endian_t::native, ui16);
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
#if 0
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
                                endian_t::native, ui16);
            istr.ignore(length);
        }
#endif
    }
    return istr;
}

#endif

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
