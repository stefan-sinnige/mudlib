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

#ifndef _MUDLIB_TLS_COMMON_H_
#define _MUDLIB_TLS_COMMON_H_

#include <iostream>
#include <memory>
#include <vector>
#include <mud/protocols/tls/ns.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The TLS message record content type
 * @details
 * The TLS record type defines the content of the message it wraps.
 */
enum class record_content_type_t : uint8_t
{
    invalid = 0,
    change_cipher_spec = 20,
    alert = 21,
    handshake = 22,
    application_data = 23,
    unassigned = 255
};

/**
 * @brief The TLS message handshake type
 * @details
 * The TLS handshake type defines the supported protocol messages. Other
 * values that are not listed are reserved. They may be compatible with
 * older TLS versions but deprecated
 */
enum class handshake_type_t : uint8_t
{
    client_hello = 1,
    server_hello = 2,
    new_session_ticket = 4,
    end_of_early_data = 5,
    encrypted_extensions = 8,
    certificate = 11,
    certificate_request = 13,
    certificate_verify = 15,
    finished = 20,
    key_update = 24,
    message_hash = 254,
    unassigned = 255
};

/**
 * @brief The TLS message extension type
 * @details
 */
enum class extension_type_t : uint16_t
{
    server_name = 0,
    max_fragment_length = 1,
    status_request = 5,
    supported_groups = 10,
    signature_algorithms = 13,
    use_srtp = 14,
    heartbeat = 15,
    application_layer_protocol_negotiation = 16,
    signed_certificate_timestamp = 18,
    client_certificate_type = 19,
    server_certificate_type = 20,
    padding = 21,
    pre_shared_key = 41,
    early_data = 42,
    supported_versions = 43,
    cookie = 44,
    psk_key_exchange_modes = 45,
    certificate_authorities = 47,
    oid_filters = 48,
    post_handshake_auth = 49,
    signature_algorithms_cert = 50,
    key_share = 51,
    unassigned = 0xffff
};

/**
 * @brief Protocol version specifications.
 * @details
 * SSL 3.0 was given version 0x0300, and TLS 3.0 started off as SSL 3.1, was
 * given the next one up. Note that the legacy version number in the message are
 * deprecated in favour of the 'Supported Version' extension.
 */
enum class protocol_version_t : uint16_t
{
    tls_10 = 0x0301,
    tls_11 = 0x0302,
    tls_12 = 0x0303,
    tls_13 = 0x0304,
};

/**
 * @brief Allowable cipher suites for TLS v1.3
 */
enum class cipher_suite_t : uint16_t
{
    tls_aes_128_gcm_sha256       = 0x1301,  /* RFC 5116 */
    tls_aes_256_gcm_sha384       = 0x1302,  /* RFC 5116 */
    tls_chacha20_poly1305_sha256 = 0x1303,  /* RFC 8439 */
    tls_aes_128_ccm_sha256       = 0x1304,  /* RFC 5116 */
    tls_aes_128_ccm_8_sha256     = 0x1305,  /* RFC 6655 */
};

/**
 * @brief Allowable compression methods for TLS v1.3
 */
enum class compression_method_t : uint8_t
{
    null = 0x00
};

/**
 * @brief Supported signature algorithms.
 */
enum class signature_algorithm_t: uint16_t
{
    /* RSASSA-PKCS1-v1_5 algorithms */
    rsa_pkcs1_sha256 = 0x0401,
    // rsa_pkcs1_sha384 = 0x0501,
    // rsa_pkcs1_sha512 = 0x0601,

    /* ECDSA algorithms */
    // ecdsa_secp256r1_sha256 = 0x0403,
    // ecdsa_secp384r1_sha384 = 0x0503,
    // ecdsa_secp521r1_sha512 = 0x0603,

    /* RSASSA-PSS algorithms with public key OID rsaEncryption */
    // rsa_pss_rsae_sha256 = 0x0804,
    // rsa_pss_rsae_sha384 = 0x0805,
    // rsa_pss_rsae_sha512 = 0x0806,

    /* EdDSA algorithms */ 
    // ed25519 = 0x0807,
    // ed448 = 0x0808,

    /* RSASSA-PSS algorithms with public key OID RSASSA-PSS */
    // rsa_pss_pss_sha256 = 0x0809,
    // rsa_pss_pss_sha384 = 0x080a,
    // rsa_pss_pss_sha512 = 0x080b,

    /* Legacy algorithms */
    // rsa_pkcs1_sha1 = 0x0201,
    // ecdsa_sha1 = 0x0203,
};

/**
 * @brief Supported key-exchange groups
 */
enum class key_exchange_group_t : uint16_t
{
    // secp256r1 = 23,
    // secp384r1 = 24,
    x25519 = 29,
    // x448 = 30,
};

/**
 * @brief Supported key-exchange modes
 */
enum class key_exchange_mode_t : uint16_t
{
    psk_ke = 0,
    psk_dhe_ke = 1
};

/**
 * @brief Supported certificate types
 */
enum class certificate_type_t: uint8_t
{
    x509 = 0,
    // open_pgp = 1,
    raw_public_key = 2
};

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_COMMON_H_ */
