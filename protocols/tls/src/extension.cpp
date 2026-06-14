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
#include "mud/protocols/tls/extension.h"

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

BEGIN_MUDLIB_TLS_NS

/* ====================================================================== 
 * Extension
 * ====================================================================== */

extension::extension(extension_type_t extension_type)
    : _type(extension_type)
{
}

/* ====================================================================== 
 * Server name
 * ====================================================================== */

server_name::server_name()
    : extension(extension_type_t::server_name)
{
}

size_t
server_name::size() const
{
    size_t sz = 6;
    for (auto& name: _names) {
        sz += 1 + name.size();
    }
    return sz;
}

std::istream&
operator>>(std::istream& istr, server_name& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::server_name);

    /* The length of the list */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);
        
    /* Get the server names */
    auto end = (uint16_t)istr.tellg() + length;
    ext._names.clear();
    while (istr.tellg() < end) {
        /* The type - should be host_name (0), the only allowable value */
        int ch = istr.get();

        /* The length of the name */
        istr.read((char*)&ui16, 2);
        length = endian::convert(endian_t::big, endian::native(), ui16);
        
        /* Read name into a string */
        std::string name;
        name.resize(length);
        istr.read(&name[0], length);
        ext._names.push_back(name);
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const server_name& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The length of the list */
    ui16 = endian::convert(endian::native(), endian_t::big,
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The server names */
    for (auto& name: ext._names) {
        /* The type - host_name(0) */
        ostr.put(0);

        /* The length of the name */
        ui16 = endian::convert(endian::native(), endian_t::big,
                static_cast<uint16_t>(name.size()));
        ostr.write((const char*)&ui16, 2);

        /* The name */
        ostr.write((const char*)name.data(), name.size());
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::server_name },
            server_name>
    _ch_server_name_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::server_name },
            server_name>
    _ee_server_name_registrar;

/* ====================================================================== 
 * Maximum fragment length
 * ====================================================================== */

max_fragment_length::max_fragment_length()
    : extension(extension_type_t::max_fragment_length)
    , _length(4096)
{
}

size_t
max_fragment_length::size() const
{
    return 5;
}

std::istream&
operator>>(std::istream& istr, max_fragment_length& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::max_fragment_length);
    ext.assert(ext_length, 1);

    /* Get the length specifier */
    switch (istr.get()) {
        case 0x01:
            ext._length = 512;
            break;
        case 0x02:
            ext._length = 1024;
            break;
        case 0x03:
            ext._length = 2048;
            break;
        case 0x04:
        default:
            ext._length = 4096;
            break;
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const max_fragment_length& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The length specifier */
    switch (ext._length) {
        case 512:
            ostr.put(0x01);
            break;
        case 1024:
            ostr.put(0x02);
            break;
        case 2048:
            ostr.put(0x03);
            break;
        case 4096:
        default:
            ostr.put(0x04);
            break;
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::max_fragment_length },
            max_fragment_length>
    _ch_max_fragment_length_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::max_fragment_length },
            max_fragment_length>
    _ee_max_fragment_length_registrar;

/* ====================================================================== 
 * Status request
 * ====================================================================== */

status_request::status_request()
    : extension(extension_type_t::status_request)
{
    /* Not supported until we can handle DER encoding */
}

size_t
status_request::size() const
{
    return 9;
}

std::istream&
operator>>(std::istream& istr, status_request& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::status_request);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const status_request& ext)
{
    throw invalid_message("Not supported yet");
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::status_request },
            status_request>
    _ch_status_request_registrar;

extension_factory::registrar<
            { handshake_type_t::certificate_request,
              extension_type_t::status_request },
            status_request>
    _cr_status_request_registrar;

extension_factory::registrar<
            { handshake_type_t::certificate,
              extension_type_t::status_request },
            status_request>
    _ct_status_request_registrar;

/* ====================================================================== 
 * Supported groups
 * ====================================================================== */

supported_groups::supported_groups()
    : extension(extension_type_t::supported_groups)
{
    _groups.push_back(key_exchange_group_t::x25519);
}

size_t
supported_groups::size() const
{
    return 6 + 2 * _groups.size();
}

std::istream&
operator>>(std::istream& istr, supported_groups& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::supported_groups);

    /* Get the group list length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the groups */
    auto end = (uint16_t)istr.tellg() + length;
    ext._groups.clear();
    while (istr.tellg() < end) {
        /* Get a group type */
        istr.read((char*)&ui16, 2);
        uint16_t type = endian::convert(endian_t::big, endian::native(), ui16);
        ext._groups.push_back(static_cast<key_exchange_group_t>(type));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const supported_groups& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The group list length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The groups */
    for (auto group: ext._groups) {
        ui16 = endian::convert(endian::native(), endian_t::big, 
                static_cast<uint16_t>(group));
        ostr.write((const char*)&ui16, 2);
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::supported_groups },
            supported_groups>
    _ch_supported_groups_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::supported_groups },
            supported_groups>
    _ee_supported_groups_registrar;

/* ====================================================================== 
 * Signature slgorithms
 * ====================================================================== */

signature_algorithms::signature_algorithms()
    : extension(extension_type_t::signature_algorithms)
{
    _algos.push_back(signature_algorithm_t::rsa_pkcs1_sha256);
}

size_t
signature_algorithms::size() const
{
    return 6 + 2 * _algos.size();
}

std::istream&
operator>>(std::istream& istr, signature_algorithms& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::signature_algorithms);

    /* Get the algorithms length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the algorithms */
    auto end = (uint16_t)istr.tellg() + length;
    ext._algos.clear();
    while (istr.tellg() < end) {
        /* Get a algorithm type */
        istr.read((char*)&ui16, 2);
        uint16_t type = endian::convert(endian_t::big, endian::native(), ui16);
        ext._algos.push_back(static_cast<signature_algorithm_t>(type));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const signature_algorithms& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The algorithm list length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The algorithms */
    for (auto algorithm: ext._algos) {
        ui16 = endian::convert(endian::native(), endian_t::big, 
                static_cast<uint16_t>(algorithm));
        ostr.write((const char*)&ui16, 2);
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::signature_algorithms },
            signature_algorithms>
    _ch_signature_algorithms_registrar;

extension_factory::registrar<
            { handshake_type_t::certificate_request,
              extension_type_t::signature_algorithms },
            signature_algorithms>
    _cr_signature_algorithms_registrar;

/* ====================================================================== 
 * Heartbeat
 * ====================================================================== */

heartbeat::heartbeat()
    : extension(extension_type_t::signature_algorithms)
    , _mode(mode_t::peer_allowed_to_send)
{
}

size_t
heartbeat::size() const
{
    return 5;
}

std::istream&
operator>>(std::istream& istr, heartbeat& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::heartbeat);
    ext.assert(ext_length, 1);

    /* Get the mode */
    int mode = istr.get();
    ext._mode = static_cast<heartbeat::mode_t>(mode);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const heartbeat& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The mode */
    ostr.put(static_cast<uint8_t>(ext._mode));

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::heartbeat },
            heartbeat>
    _ch_heartbeat_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::heartbeat },
            heartbeat>
    _ee_heartbeat_registrar;

/* ====================================================================== 
 * Application layer protocol negotiation
 * ====================================================================== */

application_layer_protocol_negotiation::application_layer_protocol_negotiation()
    : extension(extension_type_t::application_layer_protocol_negotiation)
{
}

size_t
application_layer_protocol_negotiation::size() const
{
    size_t sz = 6;
    for (auto& protocol: _protocols) {
        sz += 1 + protocol.size();
    }
    return sz;
}

std::istream&
operator>>(std::istream& istr, application_layer_protocol_negotiation& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type,
               extension_type_t::application_layer_protocol_negotiation);

    /* Get the protocols length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the protocols */
    auto end = (uint16_t)istr.tellg() + length;
    ext._protocols.clear();
    while (istr.tellg() < end) {
        /* The length of the protocol */
        int length = istr.get();

        /* Read name into a string */
        std::string name;
        name.resize(length);
        istr.read(&name[0], length);
        ext._protocols.push_back(name);
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr,
        const application_layer_protocol_negotiation& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The protocol list length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The protocols */
    for (auto protocol: ext._protocols) {
        /* The length */
        ostr.put(protocol.size());

        /* The name */
        ostr.write((const char*)protocol.data(), protocol.size());
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::application_layer_protocol_negotiation },
            application_layer_protocol_negotiation>
    _ch_application_layer_protocol_negotiation_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::application_layer_protocol_negotiation },
            application_layer_protocol_negotiation>
    _ee_application_layer_protocol_negotiation_registrar;

/* ====================================================================== 
 * Client certificate type
 * ====================================================================== */

client_certificate_type::client_certificate_type()
    : extension(extension_type_t::client_certificate_type)
{
}

size_t
client_certificate_type::size() const
{
    return 4 + _types.size();
}

std::istream&
operator>>(std::istream& istr, client_certificate_type& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::client_certificate_type);

    /* Get the types length  */
    auto length = istr.get();
    ext.assert(length, ext_length - 1);

    /* Get the types */
    auto end = (uint16_t)istr.tellg() + length;
    ext._types.clear();
    while (istr.tellg() < end) {
        /* Get a certificate type */
        int type = istr.get();
        ext._types.push_back(static_cast<certificate_type_t>(type));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const client_certificate_type& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The type list length */
    ostr.put(ext.size() - 5);

    /* The types */
    for (auto type: ext._types) {
        ostr.put(static_cast<uint8_t>(type));
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::client_certificate_type },
            client_certificate_type>
    _ch_client_certificate_type_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::client_certificate_type },
            client_certificate_type>
    _ee_client_certificate_type_registrar;

/* ====================================================================== 
 * Server certificate type
 * ====================================================================== */

server_certificate_type::server_certificate_type()
    : extension(extension_type_t::server_certificate_type)
{
}

size_t
server_certificate_type::size() const
{
    return 4 + _types.size();
}

std::istream&
operator>>(std::istream& istr, server_certificate_type& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::client_certificate_type);

    /* Get the types length  */
    auto length = istr.get();
    ext.assert(length, ext_length - 1);

    /* Get the types */
    auto end = (uint16_t)istr.tellg() + length;
    ext._types.clear();
    while (istr.tellg() < end) {
        /* Get a certificate type */
        int type = istr.get();
        ext._types.push_back(static_cast<certificate_type_t>(type));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const server_certificate_type& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The type list length */
    ostr.put(ext.size() - 5);

    /* The types */
    for (auto type: ext._types) {
        ostr.put(static_cast<uint8_t>(type));
    }

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::server_certificate_type },
            server_certificate_type>
    _ch_server_certificate_type_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::server_certificate_type },
            server_certificate_type>
    _ee_server_certificate_type_registrar;

/* ====================================================================== 
 * Padding
 * ====================================================================== */

padding::padding()
    : extension(extension_type_t::padding)
    , _length(0)
{
}

size_t
padding::size() const
{
    return 4 + _length;
}

std::istream&
operator>>(std::istream& istr, padding& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::padding);
    ext._length = ext_length;

    /* Ignore the padding */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const padding& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The padding */
    for (int i = 0; i < ext._length; ++i) {
        ostr.put(0);
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::padding },
            padding>
    _ch_padding_registrar;

/* ====================================================================== 
 * Pre-shared key
 * ====================================================================== */

pre_shared_key::pre_shared_key()
    : extension(extension_type_t::pre_shared_key)
{
}

size_t
pre_shared_key::size() const
{
    size_t sz = 6;
    for (auto& offering: _offerings) {
        sz += 2 + offering.identity().size() +
              4 +
              2 + offering.binder().size();
    }
    return sz;
}

std::istream&
operator>>(std::istream& istr, pre_shared_key& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::pre_shared_key);

    /* Get the list length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the PSK offerings */
    auto end = (uint16_t)istr.tellg() + length;
    ext._offerings.clear();
    while (istr.tellg() < end) {
        /* The length of the identity */
        istr.read((char*)&ui16, 2);
        length = endian::convert(endian_t::big, endian::native(), ui16);

        /* Read the identity  */
        mud::crypto::data_t identity(length);
        istr.read((char*)identity.data(), length);

        /* Get the age (zero for external PSKs) */
        uint32_t value_uint32;
        istr.read((char*)&value_uint32, 4);
        uint32_t age = endian::convert(endian_t::big, endian::native(),
                value_uint32);

        /* The length of the binder */
        istr.read((char*)&ui16, 2);
        length = endian::convert(endian_t::big, endian::native(), ui16);

        /* Read the binder  */
        mud::crypto::data_t binder(length);
        istr.read((char*)binder.data(), length);

        /* Save the offering */
        psk_identity offering;
        offering.obfuscated_ticket_age(age);
        offering.identity(identity);
        offering.binder(binder);
        ext._offerings.push_back(offering);
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const pre_shared_key& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The list length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The list of offerings */
    for (auto& offering: ext._offerings) {
        /* Identity size */
        ui16 = endian::convert(endian::native(), endian_t::big, 
                static_cast<uint16_t>(offering.identity().size()));
        ostr.write((const char*)&ui16, 2);

        /* The identity */
        ostr.write((const char*)offering.identity().data(),
                   offering.identity().size());

        /* The age */
        uint32_t ui32 = endian::convert(endian::native(), endian_t::big,
                offering.obfuscated_ticket_age());
        ostr.write((const char*)&ui32, 4);

        /* Binder size */
        ui16 = endian::convert(endian::native(), endian_t::big, 
                static_cast<uint16_t>(offering.binder().size()));
        ostr.write((const char*)&ui16, 2);

        /* The binder */
        ostr.write((const char*)offering.binder().data(),
                   offering.binder().size());
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::pre_shared_key },
            pre_shared_key>
    _ch_pre_shared_key_registrar;

extension_factory::registrar<
            { handshake_type_t::server_hello,
              extension_type_t::pre_shared_key },
            pre_shared_key>
    _ee_pre_shared_key_registrar;

/* ====================================================================== 
 * Early-data
 * ====================================================================== */

early_data::early_data()
    : extension(extension_type_t::early_data)
{
}

size_t
early_data::size() const
{
    return 4 + _data.size();
}

std::istream&
operator>>(std::istream& istr, early_data& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::early_data);

    /* Read the data  */
    mud::crypto::data_t data(ext_length);
    istr.read((char*)data.data(), ext_length);
    ext._data = data;

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const early_data& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The data */
    ostr.write((const char*)ext._data.data(), ext._data.size());
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::early_data },
            early_data>
    _ch_early_data_registrar;

extension_factory::registrar<
            { handshake_type_t::encrypted_extensions,
              extension_type_t::early_data },
            early_data>
    _ee_early_data_registrar;

extension_factory::registrar<
            { handshake_type_t::new_session_ticket,
              extension_type_t::early_data },
            early_data>
    _nst_early_data_registrar;

/* ====================================================================== 
 * Supported versions (client hello)
 * ====================================================================== */

client_supported_versions::client_supported_versions()
    : extension(extension_type_t::supported_versions)
{
    _versions.push_back(protocol_version_t::tls_13);
}

size_t
client_supported_versions::size() const
{
    return 5 + 2 * _versions.size();
}

std::istream&
operator>>(std::istream& istr, client_supported_versions& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::supported_versions);

    /* Get the list length  */
    auto length = istr.get();
    ext.assert(length, ext_length - 1);

    /* Get the versions */
    auto end = (uint16_t)istr.tellg() + length;
    ext._versions.clear();
    while (istr.tellg() < end) {
        /* Read and save the version */
        istr.read((char*)&ui16, 2);
        ext._versions.push_back(static_cast<protocol_version_t>(
            endian::convert(endian_t::big, endian::native(), ui16)));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const client_supported_versions& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The list size in bytes */
    ostr.put(ext._versions.size() * 2);

    /* The versions */
    for (auto version: ext._versions) {
        ui16 = endian::convert(endian::native(), endian_t::big,
                static_cast<uint16_t>(version));
        ostr.write((const char*)&ui16, 2);
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::supported_versions },
            client_supported_versions>
    _ch_supported_versions_registrar;

/* ====================================================================== 
 * Supported versions (server hello)
 * ====================================================================== */

server_supported_versions::server_supported_versions()
    : extension(extension_type_t::supported_versions)
{
    _version = protocol_version_t::tls_13;
}

size_t
server_supported_versions::size() const
{
    return 6;
}

std::istream&
operator>>(std::istream& istr, server_supported_versions& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::supported_versions);

    /* Read and save the version */
    istr.read((char*)&ui16, 2);
    ext._version = static_cast<protocol_version_t>(
            endian::convert(endian_t::big, endian::native(), ui16));
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const server_supported_versions& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The version */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext._version));
    ostr.write((const char*)&ui16, 2);

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::server_hello,
              extension_type_t::supported_versions },
            server_supported_versions>
    _sh_supported_versions_registrar;

/* ====================================================================== 
 * Cookie
 * ====================================================================== */

cookie::cookie()
    : extension(extension_type_t::cookie)
{
}

size_t
cookie::size() const
{
    return 6 + _data.size();
}

std::istream&
operator>>(std::istream& istr, cookie& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::cookie);

    /* Get the cookie data length  */
    auto length = istr.get();
    ext.assert(length, ext_length - 1);

    /* Read the data  */
    mud::crypto::data_t data(length);
    istr.read((char*)data.data(), length);
    ext._data = data;

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const cookie& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The cookie length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The cookie */
    ostr.write((const char*)ext._data.data(), ext._data.size());

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::cookie },
            cookie>
    _ch_cookie_registrar;

/* ====================================================================== 
 * PSK key exchange modes
 * ====================================================================== */

psk_key_exchange_modes::psk_key_exchange_modes()
    : extension(extension_type_t::psk_key_exchange_modes)
{
}

size_t
psk_key_exchange_modes::size() const
{
    return 5 + _modes.size();
}

std::istream&
operator>>(std::istream& istr, psk_key_exchange_modes& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::psk_key_exchange_modes);

    /* Get the list length  */
    auto length = istr.get();
    ext.assert(length, ext_length - 1);

    /* Get the modes */
    auto end = (uint16_t)istr.tellg() + length;
    ext._modes.clear();
    while (istr.tellg() < end) {
        /* Read and save the mode */
        auto mode = istr.get();
        ext._modes.push_back(static_cast<key_exchange_mode_t>(mode));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const psk_key_exchange_modes& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The list length */
    ostr.put(ext._modes.size());

    /* The modes */
    for (auto mode: ext._modes) {
        ostr.put(static_cast<uint8_t>(mode));
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::psk_key_exchange_modes },
            psk_key_exchange_modes>
    _ch_psk_key_exchange_modes_registrar;

/* ====================================================================== 
 * Certificate Authorities
 * ====================================================================== */

certificate_authorities::certificate_authorities()
    : extension(extension_type_t::certificate_authorities)
{
    /* Not supported until we can handle DER encoding */
}

size_t
certificate_authorities::size() const
{
    return 6 /* + TBD */;
}

std::istream&
operator>>(std::istream& istr, certificate_authorities& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::certificate_authorities);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const certificate_authorities& ext)
{
    throw invalid_message("Not supported yet");
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::certificate_authorities },
            certificate_authorities>
    _ch_certificate_authorities_registrar;

extension_factory::registrar<
            { handshake_type_t::certificate_request,
              extension_type_t::certificate_authorities },
            certificate_authorities>
    _cr_certificate_authorities_registrar;

/* ====================================================================== 
 * OID Filters
 * ====================================================================== */

oid_filters::oid_filters()
    : extension(extension_type_t::oid_filters)
{
    /* Not supported until we can handle DER encoding */
}

size_t
oid_filters::size() const
{
    return 4 /* + TBD */;
}

std::istream&
operator>>(std::istream& istr, oid_filters& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::oid_filters);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const oid_filters& ext)
{
    throw invalid_message("Not supported yet");
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::certificate_request,
              extension_type_t::oid_filters },
            oid_filters>
    _cr_oid_filters_registrar;

/* ====================================================================== 
 * Post-handshake auth
 * ====================================================================== */

post_handshake_auth::post_handshake_auth()
    : extension(extension_type_t::post_handshake_auth)
{
}

size_t
post_handshake_auth::size() const
{
    return 4;
}

std::istream&
operator>>(std::istream& istr, post_handshake_auth& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::post_handshake_auth);
    ext.assert(ext_length, 0);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const post_handshake_auth& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::post_handshake_auth },
            post_handshake_auth>
    _ch_post_handshake_auth_registrar;

/* ====================================================================== 
 * Signature algorithms certs extension
 * ====================================================================== */

signature_algorithms_cert::signature_algorithms_cert()
    : extension(extension_type_t::signature_algorithms_cert)
{
    _algos.push_back(signature_algorithm_t::rsa_pkcs1_sha256);
}

size_t
signature_algorithms_cert::size() const
{
    return 6 + 2 * _algos.size();
}

std::istream&
operator>>(std::istream& istr, signature_algorithms_cert& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::signature_algorithms_cert);

    /* Get the algorithms length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the algorithms */
    auto end = (uint16_t)istr.tellg() + length;
    ext._algos.clear();
    while (istr.tellg() < end) {
        /* Get a algorithm type */
        istr.read((char*)&ui16, 2);
        uint16_t type = endian::convert(endian_t::big, endian::native(),
                                        ui16);
        ext._algos.push_back(static_cast<signature_algorithm_t>(type));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const signature_algorithms_cert& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The list length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The algorithms */
    for (auto algorithm: ext._algos) {
        ui16 = endian::convert(endian::native(), endian_t::big, 
                static_cast<uint16_t>(algorithm));
        ostr.write((const char*)&ui16, 2);
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::signature_algorithms_cert },
            signature_algorithms_cert>
    _ch_signature_algorithms_cert_registrar;

extension_factory::registrar<
            { handshake_type_t::certificate_request,
              extension_type_t::signature_algorithms_cert },
            signature_algorithms_cert>
    _cr_signature_algorithms_cert_registrar;

/* ====================================================================== 
 * Key-share for client hello
 * ====================================================================== */

client_key_share::client_key_share()
    : extension(extension_type_t::key_share)
{
}

size_t
client_key_share::size() const
{
    size_t sz = 6;
    for (auto& share: _shares) {
        sz += 4 + share.second.size();
    }
    return sz;
}

std::istream&
operator>>(std::istream& istr, client_key_share& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::key_share);

    /* Get the key data length  */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(length, ext_length - 2);

    /* Get the key data */
    auto end = (uint16_t)istr.tellg() + length;
    ext._shares.clear();
    while (istr.tellg() < end) {
        /* The key exchange group */
        istr.read((char*)&ui16, 2);
        auto group = static_cast<key_exchange_group_t>(
            endian::convert(endian_t::big, endian::native(), ui16));

        /* The length of the key */
        istr.read((char*)&ui16, 2);
        length = endian::convert(endian_t::big, endian::native(), ui16);

        /* Read the key */
        mud::crypto::key_t key(length);
        istr.read((char*)key.data(), length);

        /* Save the entry */
        ext._shares.push_back(std::make_pair(group, key));
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const client_key_share& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* The list size in bytes */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 6));
    ostr.write((const char*)&ui16, 2);

    /* The key shares */
    for (auto share: ext._shares) {
        /* The key exhcnage group */
        ui16 = endian::convert(endian::native(), endian_t::big,
                static_cast<uint16_t>(share.first));
        ostr.write((const char*)&ui16, 2);

        /* The length of the key */
        ui16 = endian::convert(endian::native(), endian_t::big,
                static_cast<uint16_t>(share.second.size()));
        ostr.write((const char*)&ui16, 2);

        /* The key */
        ostr.write((const char*)share.second.data(), share.second.size());
    }
    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::client_hello,
              extension_type_t::key_share },
            client_key_share>
    _ch_key_share_registrar;

/* ====================================================================== 
 * Key-share for server hello
 * ====================================================================== */

server_key_share::server_key_share()
    : extension(extension_type_t::key_share)
{
}

size_t
server_key_share::size() const
{
    return 8 + _share.second.size();
}

std::istream&
operator>>(std::istream& istr, server_key_share& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension_type_t::key_share);

    /* The key exchange group */
    istr.read((char*)&ui16, 2);
    auto group = static_cast<key_exchange_group_t>(
        endian::convert(endian_t::big, endian::native(), ui16));

    /* The length of the key */
    istr.read((char*)&ui16, 2);
    auto length = endian::convert(endian_t::big, endian::native(), ui16);

    /* Read the key */
    mud::crypto::key_t key(length);
    istr.read((char*)key.data(), length);

    /* Save the entry */
    ext._share = std::make_pair(group, key);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const server_key_share& ext)
{
    /* Set up to throw exceptions */
    ostr.exceptions(std::istream::failbit);

    /* The extension type and length */
    uint16_t ui16;
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.extension_type()));
    ostr.write((const char*)&ui16, 2);
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext.size() - 4));
    ostr.write((const char*)&ui16, 2);

    /* Key exchange group */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext._share.first));
    ostr.write((const char*)&ui16, 2);

    /* Key length */
    ui16 = endian::convert(endian::native(), endian_t::big, 
            static_cast<uint16_t>(ext._share.second.size()));
    ostr.write((const char*)&ui16, 2);

    /* The key */
    ostr.write((const char*)ext._share.second.data(), ext._share.second.size());

    return ostr;
}

extension_factory::registrar<
            { handshake_type_t::server_hello,
              extension_type_t::key_share },
            server_key_share>
    _sh_key_share_registrar;

END_MUDLIB_TLS_NS

/* ====================================================================== 
 * Extension factory
 * ====================================================================== */

template<>
mud::tls::extension_factory&
mud::tls::extension_factory::instance()
{   
    static mud::tls::extension_factory _instance;
    return _instance;
}   

/* vi: set ai ts=4 expandtab: */
