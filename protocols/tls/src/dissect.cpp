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

#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <mud/core/hexdump.h>
#include "mud/protocols/tls/dissect.h"

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

BEGIN_MUDLIB_TLS_NS

/**
 * Indentation streaming operator.
 */
class indent
{
public:
    /** Constructor to define the number of spaces to print. */
    explicit indent(size_t nr, size_t  mult = 2)
        : _spaces(nr), _multiplier(mult)
    {}

private:
    /** The number of spaces to print */
    size_t _spaces;

    /** The space multiplier */
    size_t _multiplier;

    /** Friend */
    friend std::ostream& operator<<(std::ostream& ostr, const indent& obj) {
        ostr << std::setfill(' ') << std::setw(obj._spaces * obj._multiplier)
             << "";
        return  ostr;
    }
};

dissect::dissect(const record& msg)
{
    std::stringstream sstr;
    sstr << msg;
    std::string raw = sstr.str();
    _binary = mud::core::buffer_view((uint8_t*)raw.data(), raw.size());
    _view = _binary.view();
}

dissect::dissect(const mud::core::buffer_view& msg)
    : _view(msg)
{
}

void
dissect::record_layer(std::ostream& ostr, size_t idx, size_t ind) const
{
    constexpr size_t width = 12;

    /* Decode the record layer header */
    ostr << indent(ind) << "Record Layer\n";
    ++ind;

    /* Content type */
    auto content_type = static_cast<record_content_type_t>(
            _view.get<uint8_t>(idx++, endian_t::big));
    ostr << indent(ind) << std::setw(width) << std::left
         << "Content Type" << ": ";
    switch (content_type) {
        case record_content_type_t::alert:
            ostr << "Alert Protocol";
            break;
        case record_content_type_t::application_data:
            ostr << "Application Data";
            break;
        case record_content_type_t::change_cipher_spec:
            ostr << "Change Cipher Protocol";
            break;
        case record_content_type_t::handshake:
            ostr << "Handshake Protocol";
            break;
        case record_content_type_t::invalid:
        case record_content_type_t::unassigned:
            /* Invalid */
            throw std::out_of_range("invalid content-type");
            break;
    }
    ostr << " (" << (int)content_type << ")\n";

    /* Legacy record version */
    ostr << indent(ind) << std::setw(width) << std::left
         << "Version" << ": ";
    protocol_version(ostr, _view.get<uint16_t>(idx, endian_t::big));
    ostr << "\n";
    idx += 2;

    /* Length */
    uint16_t length = _view.get<uint16_t>(idx, endian_t::big);
    idx += 2;
    ostr << indent(ind) << std::setw(width) << std::left
         << "Length" << ": " << std::dec << length << "\n";
    if (length != _view.size() - idx) {
        throw std::out_of_range("record-layer length mismatch");
    }

    /* Decode the content specific message */
    switch (content_type) {
        case record_content_type_t::alert:
        case record_content_type_t::application_data:
        case record_content_type_t::change_cipher_spec:
            /* Not yet supported */
            break;
        case record_content_type_t::handshake:
            handshake_protocol(ostr, idx, ind);
            break;
        case record_content_type_t::invalid:
        case record_content_type_t::unassigned:
            /* Invalid */
            break;
    }
    ostr << std::flush;
}

void
dissect::handshake_protocol(std::ostream& ostr, size_t idx, size_t ind) const
{
    constexpr size_t width = 12;

    /* Decode the record layer header */
    ostr << indent(ind) << "Handshake Protocol\n";
    ++ind;

    /* Message type */
    auto message_type = static_cast<handshake_type_t>(
            _view.get<uint8_t>(idx++, endian_t::big));
    ostr << indent(ind) << std::setw(width) << std::left
         << "Message Type" << ": ";
    switch (message_type) {
        case handshake_type_t::client_hello:
            ostr << "Client Hello";
            break;
        case handshake_type_t::server_hello:
            ostr << "Server Hello";
            break;
        case handshake_type_t::new_session_ticket:
            ostr << "New Session Ticket";
            break;
        case handshake_type_t::end_of_early_data:
            ostr << "End of Early Data";
            break;
        case handshake_type_t::encrypted_extensions:
            ostr << "Encrypted Extensions";
            break;
        case handshake_type_t::certificate:
            ostr << "Certificate";
            break;
        case handshake_type_t::certificate_request:
            ostr << "Certificate Request";
            break;
        case handshake_type_t::certificate_verify:
            ostr << "Certificate Verify";
            break;
        case handshake_type_t::finished:
            ostr << "Finished";
            break;
        case handshake_type_t::key_update:
            ostr << "Key Update";
            break;
        case handshake_type_t::message_hash:
            ostr << "Message Hash";
            break;
        case handshake_type_t::unassigned:
        default:
            /* Invalid */
            throw std::out_of_range("invalid handshake message-type");
            break;
    }
    ostr << " (" << (int)message_type << ")\n";

    /* Length */
    uint32_t length = 0;
    length |= _view.get<uint8_t>(idx, endian_t::big) << 16;
    length |= _view.get<uint16_t>(idx+1, endian_t::big);
    idx += 3;
    ostr << indent(ind) << std::setw(width) << std::left
         << "Length" << ": " << std::dec << length << "\n";
    if (length != _view.size() - idx) {
        throw std::out_of_range("handshake-layer length mismatch");
    }

    /* Decode the content specific message */
    switch (message_type) {
        case handshake_type_t::client_hello:
            client_hello(ostr, idx, ind);
            break;
        case handshake_type_t::server_hello:
            client_hello(ostr, idx, ind);
            break;
        case handshake_type_t::new_session_ticket:
        case handshake_type_t::end_of_early_data:
        case handshake_type_t::encrypted_extensions:
        case handshake_type_t::certificate:
        case handshake_type_t::certificate_request:
        case handshake_type_t::certificate_verify:
        case handshake_type_t::finished:
        case handshake_type_t::key_update:
        case handshake_type_t::message_hash:
            /* Not yet supported */
            break;
        case handshake_type_t::unassigned:
        default:
            /* Invalid */
            throw std::out_of_range("invalid handshake message-type");
            break;
    }
}

void
dissect::client_hello(std::ostream& ostr, size_t idx, size_t ind) const
{
    constexpr size_t width = 19;

    /* Decode the client hello */
    ostr << indent(ind) << "Client Hello\n";
    ++ind;

    /* Legacy version */
    ostr << indent(ind) << std::setw(width) << std::left
         << "Version" << ": ";
    protocol_version(ostr, _view.get<uint16_t>(idx, endian_t::big));
    ostr << "\n";
    idx += 2;

    /* Random */
    ostr << indent(ind) << std::setw(width) << std::left
         << "Random" << ": ";
    ostr << _view.view(_view.begin()+idx, _view.begin()+idx+32) << "\n";
    idx += 32;

    /* Legacy session ID */
    uint32_t length = _view.get<uint8_t>(idx, endian_t::big);
    ++idx;
    ostr << indent(ind) << std::setw(width) << std::left
         << "Session ID" << ": ";
    ostr << _view.view(_view.begin()+idx, _view.begin()+idx+length) << "\n";
    idx += length;

    /* Cipher suites */
    length = _view.get<uint16_t>(idx, endian_t::big);
    idx += 2;
    ostr << indent(ind) << "Cipher Suites\n";
    ++ind;
    for (int i = 0; i < length/2; ++i) {
        ostr << indent(ind);
        cipher_suite(ostr, _view.get<uint16_t>(idx, endian_t::big));
        idx += 2;
    }
    --ind;

    /* Compression methods */
    length = _view.get<uint8_t>(idx++, endian_t::big);
    ostr << indent(ind) << "Compression Methods\n";
    ++ind;
    for (int i = 0; i < length; ++i) {
        ostr << indent(ind);
        compression_method(ostr, _view.get<uint8_t>(idx++, endian_t::big));
    }
    --ind;

    /* Extensions */
    length = _view.get<uint16_t>(idx, endian_t::big);
    idx += 2;
    if (length != _view.size() - idx) {
        throw std::out_of_range("client-hello extension length mismatch");
    }
    ostr << indent(ind) << "Extensions\n";
    ++ind;
    while (idx < _view.size()) {
        /* Get the extentsion type and size */
        uint16_t ext_type = _view.get<uint16_t>(idx, endian_t::big);
        idx += 2;
        uint16_t ext_length = _view.get<uint16_t>(idx, endian_t::big);
        idx += 2;
        auto data = _view.view(_view.begin() + idx, 
                               _view.begin() + idx + ext_length);
        switch (ext_type) {
            case (uint16_t)extension_type_t::key_share:
                key_share(ostr, ind, ext_type, ext_length, data);
                break;
            case (uint16_t)extension_type_t::server_name:
            case (uint16_t)extension_type_t::max_fragment_length:
            case (uint16_t)extension_type_t::status_request:
            case (uint16_t)extension_type_t::supported_groups:
            case (uint16_t)extension_type_t::signature_algorithms:
            case (uint16_t)extension_type_t::use_srtp:
            case (uint16_t)extension_type_t::heartbeat:
            case (uint16_t)extension_type_t::application_layer_protocol_negotiation:
            case (uint16_t)extension_type_t::signed_certificate_timestamp:
            case (uint16_t)extension_type_t::client_certificate_type:
            case (uint16_t)extension_type_t::server_certificate_type:
            case (uint16_t)extension_type_t::padding:
            case (uint16_t)extension_type_t::pre_shared_key:
            case (uint16_t)extension_type_t::early_data:
            case (uint16_t)extension_type_t::supported_versions:
                supported_versions(ostr, ind, ext_type, ext_length, data);
                break;
            case (uint16_t)extension_type_t::cookie:
            case (uint16_t)extension_type_t::psk_key_exchange_modes:
            case (uint16_t)extension_type_t::certificate_authorities:
            case (uint16_t)extension_type_t::oid_filters:
            case (uint16_t)extension_type_t::post_handshake_auth:
            case (uint16_t)extension_type_t::signature_algorithms_cert:
                ostr << indent(ind) << "TBD" << ": Type (" << ext_type
                     << ")\n" << indent(ind+1) << data << "\n";
                break;
            unassigned:
            default:
                ostr << indent(ind) << "Extension" << ": Type (" << ext_type
                     << ")\n" << indent(ind+1) << data << "\n";
                break;
        }
        idx += ext_length;
    }
    --ind;
}

void
dissect::server_hello(std::ostream& ostr, size_t idx, size_t ind) const
{
    constexpr size_t width = 12;

    /* Decode the server hello */
    ostr << indent(ind) << "Server Hello\n";
    ++ind;

}

void
dissect::protocol_version(std::ostream& ostr, uint16_t value) const
{
    switch (value) {
        case 0x0300:
            ostr << "SSL 3.0";
            break;
        case (uint16_t)protocol_version_t::tls_10:
            ostr << "TLS 1.0";
            break;
        case (uint16_t)protocol_version_t::tls_11:
            ostr << "TLS 1.1";
            break;
        case (uint16_t)protocol_version_t::tls_12:
            ostr << "TLS 1.2";
            break;
        case (uint16_t)protocol_version_t::tls_13:
            ostr << "TLS 1.3";
            break;
        default:
            ostr << "Unknown version 0x" << std::hex << std::setw(4)
                 << std::setfill('0') << value;
            break;
    }
}

void
dissect::cipher_suite(std::ostream& ostr, uint16_t value) const
{
    switch (value)
    {
        case (uint16_t)cipher_suite_t::tls_aes_128_gcm_sha256:   
            ostr << "AES-128-GCM / SHA256";
            break;
        case (uint16_t)cipher_suite_t::tls_aes_256_gcm_sha384:
            ostr << "AES-256-GCM / SHA384";
            break;
        case (uint16_t)cipher_suite_t::tls_chacha20_poly1305_sha256:
            ostr << "ChaCha-20 Poly-1305 / SHA256";
            break;
        case (uint16_t)cipher_suite_t::tls_aes_128_ccm_sha256:
            ostr << "AES-128 CCM / SHA256";
            break;
        case (uint16_t)cipher_suite_t::tls_aes_128_ccm_8_sha256:
            ostr << "AES-128 CCM-8 / SHA256";
            break;
        default:
            ostr << "Discouraged: 0x" << std::hex << std::setw(4)
                 << std::setfill('0') << value;
            break;
    };
    ostr << "\n";
}

void
dissect::compression_method(std::ostream& ostr, uint8_t value) const
{
    switch (value)
    {
        case (uint8_t)compression_method_t::null:   
            ostr << "None";
            break;
        default:
            ostr << "Discouraged: 0x" << std::hex << std::setw(2)
                 << std::setfill('0') << value;
            break;
    }
    ostr << "\n";
}

void
dissect::key_exchange_group(std::ostream& ostr, uint16_t value) const
{
    switch (value)
    {
        case (uint16_t)key_exchange_group_t::x25519:
            ostr << "X25519";
            break;
        default:
            ostr << "Key exchange group: " << std::hex << std::setw(2)
                 << std::setfill('0') << value;
            break;
    }
    ostr << "\n";
}

void
dissect::server_name(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::max_fragment_length(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::status_request(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::supported_groups(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::signature_algorithms(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::use_srtp(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::heartbeat(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::application_layer_protocol_negotiation(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::signed_certificate_timestamp(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::client_certificate_type(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::server_certificate_type(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::padding(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::pre_shared_key(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::early_data(std::ostream& ostr, size_t ind, 
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::supported_versions(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
    constexpr size_t width = 7;

    /* The extension name */
    ostr << indent(ind) << "Supported Versions\n";
    ++ind;

    /* Get the number of versions */
    size_t idx = 0;
    auto length = data.get<uint8_t>(idx, endian_t::big);
    ++idx;
    while (idx < ext_length) {
        auto version = data.get<uint16_t>(idx, endian_t::big);
        idx += 2;
        ostr << indent(ind);
        protocol_version(ostr, version);
        ostr<< "\n";
    }
}

void
dissect::cookie(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::psk_key_exchange_modes(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::certificate_authorities(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::oid_filters(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::post_handshake_auth(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::signature_algorithms_cert(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
}

void
dissect::key_share(std::ostream& ostr, size_t ind,
        uint16_t ext_type, uint16_t ext_length,
        const mud::core::buffer_view& data) const
{
    constexpr size_t width = 5;

    /* The extension name */
    ostr << indent(ind) << "Key Shares\n";
    ++ind;

    /* Get the overall key-share length */
    size_t idx = 0;
    auto length = data.get<uint16_t>(idx, endian_t::big);
    idx += 2;

    while (idx < ext_length) {
        auto group = data.get<uint16_t>(idx, endian_t::big);
        idx += 2;
        auto key_length = data.get<uint16_t>(idx, endian_t::big);
        idx += 2;
        ostr << indent(ind) << std::setw(width) << std::left
             << "Group" << ": ";
        key_exchange_group(ostr, group);
        ostr << indent(ind) << std::setw(width) << std::left
             << "Key" << ": "
             << data.view(data.begin() + idx, data.begin() + idx + key_length)
             << "\n";;
        idx += key_length;
    }
}

std::ostream&
operator<<(std::ostream& ostr, const dissect& msg)
{
    /* Dump the entire message buffer as a binary view */
    ostr << mud::core::hexdump(msg._binary) << std::endl;

    /* Decode it, starting from the record-layer. This will disect the rest of
     * the message accordingly. */
    try {
        msg.record_layer(ostr, 0, 0);
    }
    catch (const std::exception& ex) {
        /* Ignore. */
        ostr << "Invalid TLS message: " << ex.what() << std::endl;
    }
    return ostr;
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
