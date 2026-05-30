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

extension::extension(extension::type_t extension_type)
    : _type(extension_type)
{
}

/* ====================================================================== 
 * Server name
 * ====================================================================== */

server_name::server_name()
    : extension(extension::type_t::server_name)
{
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
    ext.assert(ext_type, extension::type_t::server_name);

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
    return ostr;
}

/* ====================================================================== 
 * Maximum fragment length
 * ====================================================================== */

max_fragment_length::max_fragment_length()
    : extension(extension::type_t::max_fragment_length)
    , _length(4096)
{
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
    ext.assert(ext_type, extension::type_t::max_fragment_length);
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
    return ostr;
}

/* ====================================================================== 
 * Status request
 * ====================================================================== */

status_request::status_request()
    : extension(extension::type_t::status_request)
{
    /* Not supported until we can handle DER encoding */
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
    ext.assert(ext_type, extension::type_t::status_request);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const status_request& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Supported groups
 * ====================================================================== */

supported_groups::supported_groups()
    : extension(extension::type_t::supported_groups)
{
    _groups.push_back(key_exchange_group_t::x25519);
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
    ext.assert(ext_type, extension::type_t::supported_groups);

    /* Get the group length  */
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
    return ostr;
}

/* ====================================================================== 
 * Supported groups
 * ====================================================================== */

signature_algorithms::signature_algorithms()
    : extension(extension::type_t::signature_algorithms)
{
    _algos.push_back(signature_algorithm_t::rsa_pkcs1_sha256);
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
    ext.assert(ext_type, extension::type_t::signature_algorithms);

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
    return ostr;
}

/* ====================================================================== 
 * Heartbeat
 * ====================================================================== */

heartbeat::heartbeat()
    : extension(extension::type_t::signature_algorithms)
    , _mode(mode_t::peer_allowed_to_send)
{
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
    ext.assert(ext_type, extension::type_t::heartbeat);
    ext.assert(ext_length, 1);

    /* Get the mode */
    int mode = istr.get();
    ext._mode = static_cast<heartbeat::mode_t>(mode);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const heartbeat& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Application layer protocol negotiation
 * ====================================================================== */

application_layer_protocol_negotiation::application_layer_protocol_negotiation()
    : extension(extension::type_t::application_layer_protocol_negotiation)
{
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
               extension::type_t::application_layer_protocol_negotiation);

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
    return ostr;
}

/* ====================================================================== 
 * Client certificate type
 * ====================================================================== */

client_certificate_type::client_certificate_type()
    : extension(extension::type_t::client_certificate_type)
{
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
    ext.assert(ext_type, extension::type_t::client_certificate_type);

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
    return ostr;
}

/* ====================================================================== 
 * Server certificate type
 * ====================================================================== */

server_certificate_type::server_certificate_type()
    : extension(extension::type_t::server_certificate_type)
{
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
    ext.assert(ext_type, extension::type_t::client_certificate_type);

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
    return ostr;
}

/* ====================================================================== 
 * Padding
 * ====================================================================== */

padding::padding()
    : extension(extension::type_t::padding)
    , _length(0)
{
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
    ext.assert(ext_type, extension::type_t::padding);

    /* Ignore the padding */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const padding& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Pre-shared key
 * ====================================================================== */

pre_shared_key::pre_shared_key()
    : extension(extension::type_t::pre_shared_key)
{
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
    ext.assert(ext_type, extension::type_t::pre_shared_key);

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
    return ostr;
}

/* ====================================================================== 
 * Early-data
 * ====================================================================== */

early_data::early_data()
    : extension(extension::type_t::early_data)
{
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
    ext.assert(ext_type, extension::type_t::early_data);

    /* Read the data  */
    mud::crypto::data_t data(ext_length);
    istr.read((char*)data.data(), ext_length);
    ext._data = data;

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const early_data& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Supported versions
 * ====================================================================== */

supported_versions::supported_versions()
    : extension(extension::type_t::supported_versions)
{
    _versions.push_back(protocol_version_t::tls_13);
}

std::istream&
operator>>(std::istream& istr, supported_versions& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension::type_t::supported_versions);

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
operator<<(std::ostream& ostr, const supported_versions& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Cookie
 * ====================================================================== */

cookie::cookie()
    : extension(extension::type_t::cookie)
{
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
    ext.assert(ext_type, extension::type_t::cookie);

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
    return ostr;
}

/* ====================================================================== 
 * PSK key exchange modes
 * ====================================================================== */

psk_key_exchange_modes::psk_key_exchange_modes()
    : extension(extension::type_t::psk_key_exchange_modes)
{
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
    ext.assert(ext_type, extension::type_t::psk_key_exchange_modes);

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
    return ostr;
}

/* ====================================================================== 
 * Certificate Authorities
 * ====================================================================== */

certificate_authorities::certificate_authorities()
    : extension(extension::type_t::certificate_authorities)
{
    /* Not supported until we can handle DER encoding */
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
    ext.assert(ext_type, extension::type_t::certificate_authorities);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const certificate_authorities& ext)
{
    return ostr;
}

/* ====================================================================== 
 * OID Filters
 * ====================================================================== */

oid_filters::oid_filters()
    : extension(extension::type_t::oid_filters)
{
    /* Not supported until we can handle DER encoding */
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
    ext.assert(ext_type, extension::type_t::oid_filters);

    /* Unsupported extension type, skip it */
    istr.ignore(ext_length);

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const oid_filters& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Post-handshake auth
 * ====================================================================== */

post_handshake_auth::post_handshake_auth()
    : extension(extension::type_t::post_handshake_auth)
{
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

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const post_handshake_auth& ext)
{
    return ostr;
}

/* ====================================================================== 
 * Signature algorithms certs extension
 * ====================================================================== */

signature_algorithms_cert::signature_algorithms_cert()
    : extension(extension::type_t::signature_algorithms_cert)
{
    _algos.push_back(signature_algorithm_t::rsa_pkcs1_sha256);
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
    ext.assert(ext_type, extension::type_t::signature_algorithms_cert);

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
    return ostr;
}

/* ====================================================================== 
 * Key-share
 * ====================================================================== */

key_share::key_share()
    : extension(extension::type_t::key_share)
{
}

std::istream&
operator>>(std::istream& istr, key_share& ext)
{
    /* Set up to throw exceptions */
    istr.exceptions(std::istream::failbit);

    /* Get the extension type and total extension length */
    uint16_t ui16;
    istr.read((char*)&ui16, 2);
    auto ext_type = endian::convert(endian_t::big, endian::native(), ui16);
    istr.read((char*)&ui16, 2);
    auto ext_length = endian::convert(endian_t::big, endian::native(), ui16);
    ext.assert(ext_type, extension::type_t::key_share);

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
operator<<(std::ostream& ostr, const key_share& ext)
{
    return ostr;
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
