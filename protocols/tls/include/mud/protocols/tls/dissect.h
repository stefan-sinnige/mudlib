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

#ifndef _MUDLIB_TLS_DECODE_H_
#define _MUDLIB_TLS_DECODE_H_

#include <iostream>
#include <mud/core/buffer.h>
#include <mud/protocols/tls/ns.h>
#include <mud/protocols/tls/message.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief Dissecting a TLS message into human-readable form.
 * @details
 * The binary TLS messages can be presented into a human readable form by
 * dissecting their contents. Only valid and supported TLS messages can be
 * successfully dissected.
 *
 * For example, to dissect a TLS client-hello message:
 * @code
 * mud::tls::client_hello msg;
 * std::cout << dissect(msg) << std::endl;
 * @endcode
 */
class MUDLIB_TLS_API dissect
{
public:
    /** 
     * @brief Constructor to dissect a TLS message.
     * @param msg The TLS message to dissect.
     */
    explicit dissect(const record& msg);

    /** 
     * @brief Constructor to dissect a TLS message.
     * @param msg The TLS message to dissect.
     */
    explicit dissect(const mud::core::buffer_view& msg);

private:
    /**
     * Decode different layers and message types. Any invalid message will
     * be raised as an exception.
     */
    void record_layer(
            std::ostream& ostr, size_t idx, size_t ind) const;
    void handshake_protocol(
            std::ostream& ostr, size_t idx, size_t ind) const;
    void client_hello(
            std::ostream& ostr, size_t idx, size_t ind) const;
    void server_hello(
            std::ostream& ostr, size_t idx, size_t ind) const;

    /**
     * Decode particular fields.
     */
    void protocol_version(
            std::ostream& ostr, uint16_t value) const;
    void cipher_suite(
            std::ostream& ostr, uint16_t value) const;
    void compression_method(
            std::ostream& ostr, uint8_t value) const;
    void key_exchange_group(
            std::ostream& ostr, uint16_t value) const;

    /**
     * Decode extensions
     */
    void server_name(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void max_fragment_length(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void status_request(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void supported_groups(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void signature_algorithms(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void use_srtp(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void heartbeat(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void application_layer_protocol_negotiation(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void signed_certificate_timestamp(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void client_certificate_type(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void server_certificate_type(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void padding(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void pre_shared_key(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void early_data(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void supported_versions(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void cookie(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void psk_key_exchange_modes(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void certificate_authorities(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void oid_filters(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void post_handshake_auth(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void signature_algorithms_cert(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;
    void key_share(std::ostream& ostr, size_t ind,
            uint16_t type, uint16_t length,
            const mud::core::buffer_view& data) const;

    /**
     * The message to dissect.
     */
    mud::core::buffer_view _view;

    /**
     * The message buffer, if required
     */
    mud::core::buffer _binary;

    /** Friend */
    friend std::ostream& operator<<(std::ostream&, const dissect& msg);
};

/**
 * @brief Write a dissected TLS message to an output stream.
 * @param ostr The output stream.
 * @param msg The message to dissect.
 */
std::ostream&
operator<<(std::ostream&, const dissect& msg);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_DECODE_H_ */
