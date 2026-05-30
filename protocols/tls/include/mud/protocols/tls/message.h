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

#ifndef _MUDLIB_TLS_MESSAGE_H_
#define _MUDLIB_TLS_MESSAGE_H_

#include <iostream>
#include <memory>
#include <vector>
#include <mud/protocols/tls/ns.h>
#include <mud/protocols/tls/common.h>
#include <mud/protocols/tls/extension.h>
#include <mud/crypto/types.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The definition of an TLS record message.
 * @details
 * The TLS record message (as part of the record layer) encapsulates all the
 * messages. Apart from the initial security set-up messages, all the content
 * of the record message fragment is encrypted. The content can contain messages
 * that are specific to TLS negotiation, or it can contain application data that
 * is processed by the layer that TLS itself encapsulates. In encrypted form,
 * the record header is the only part that is transmitted in plain-text. 
 *
 * A TLS record (header-only) can be read from an input stream using the @c
 * operator>> streaming operator. Any subsequent part of the remaining message
 * can be read using their associated streaming operator.
 *
 * When a full TLS record is read from an input stream, a derived instance of
 * the record is created as a @c std::shared_ptr and is re-seated by an
 * appropriate instance of a derived specialisation during the streaming
 * operation.
 * @code
 * // Create an invalid record placeholder
 * std::shared_ptr<mud::tls::record> record;
 * istr >> record;
 *
 * // The placeholder is replaced with an instance of the specialisation.
 * auto msg = std::dynamic_pointer_cast<mud::tls::client_hello>(record);
 * if (msg) {
 *   std::cout << "ClientHello received" << std::endl;
 * }
 * @endcode
 *
 * @see RFC 8446: Section 5
 */
class MUDLIB_TLS_API record
{
public:
    /** 
     * @brief Create an invalid record.
     * @param encrypted Indication if the record content is encrypted.
     */
    record();

    /**
     * @brief Destructor.
     */
    virtual ~record() = default;

    /**
     * @brief Return the content type.
     */
    record_content_type_t content_type() const { return _content_type; }

    /**
     * @brief Set the content type.
     * @param value The content type to set.
     */
    void content_type(record_content_type_t value) { _content_type = value; }

    /**
     * @brief Get legacy record version
     */
    protocol_version_t legacy_record_version() const {
        return _legacy_record_version;
    }

    /**
     * @brief Set the legacy record version
     * @param version The version to set.
     */
    void legacy_record_version(protocol_version_t version) {
        _legacy_record_version = version; 
    }

    /**
     * @brief Return the record length
     */
    uint16_t length() const { return _length; }

    /**
     * @brief Get the encrypted status of the content.
     */
    bool encrypted() const { return _encrypted; }

    /**
     * @brief Set the encrypted status of the content.
     * @param value The statue to set.
     */
    void encrypted(bool value) { _encrypted = value; }

protected:
    /**
     * Constructor
     * @param content_type The record content type
     * @param version The legacy record version.
     */
    record(record_content_type_t content_type, protocol_version_t version);

private:
    /** The content type */
    record_content_type_t _content_type;

    /** The legacy record version */
    protocol_version_t _legacy_record_version;

    /** The record length */
    uint16_t _length;

    /** Indication if the content is encrypted. */
    bool _encrypted;

    /** Friends */
    friend std::istream& operator>>(std::istream& istr, record& rec);
};

/**
 * @brief The base definition of an TLS handshake message.
 * @see RFC 8446: Section 4
 */
class MUDLIB_TLS_API handshake : public record
{
public:
    /** 
     * @brief Create an invalid handshake.
     */
    handshake();

    /**
     * Copy constructor.
     */
    handshake(const handshake& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~handshake() = default;

    /**
     * Assign a TLS mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    handshake& operator=(const handshake& rhs) = default;

    /**
     * Get the handshake message type.
     */
    handshake_type_t msg_type() const { return _msg_type; }

    /**
     * Set the handshake message type.
     * @param value The handshake type to set.
     */
    void msg_type(handshake_type_t value) { _msg_type = value; }

    /**
     * @brief Return the handshake length
     */
    uint32_t length() const { return _length; }

protected:
    /**
     * @brief Construction of a message of a certain type.
     * @param type The handshake messgae type.
     */
    handshake(handshake_type_t type);

private:
    /** The message type */
    handshake_type_t _msg_type;

    /** The message length (24-bits) */
    uint32_t _length;

    /** Friends */
    friend std::istream& operator>>(std::istream& istr, handshake& rec);
};

/**
 * @brief TLS Client Hello message
 * @details
 */
class client_hello: public handshake
{
public:
    /**
     * @brief Construct a Client Hello message.
     */
    client_hello();

    /**
     * @brief Get legacy version
     */
    protocol_version_t legacy_version() const { return _legacy_version; }

    /**
     * @brief Set the legacy version
     * @param version The version to set.
     */
    void legacy_version(protocol_version_t version) {
        _legacy_version = version;
    }

    /**
     * @brief Get the 32-byte random value.
     */
    const mud::crypto::random_t& random() const { return _random; }

    /**
     * @brief Set the 32-byte random value.
     * @param version The random value to set.
     */
    void random(const mud::crypto::random_t& value) { _random = value; }

    /**
     * @brief Get the legacy session ID.
     */
    const mud::crypto::data_t& legacy_session_id() const {
        return _legacy_session_id;
    }

    /**
     * @brief Set the legacy session ID.
     * @param version The seesion ID value to set.
     */
    void legacy_session_id(const mud::crypto::data_t& value) {
        _legacy_session_id = value;
    }

    /**
     * @brief Get the cipher suites.
     */
    const std::vector<cipher_suite_t>& cipher_suites() const {
        return _cipher_suites;
    }

    /**
     * @brief Set the cipher suites.
     * @param version The cipher suites to set.
     */
    void cipher_suites(const std::vector<cipher_suite_t>& value) {
        _cipher_suites = value;
    }

    /**
     * @brief Get the legacy compression methods
     */
    const std::vector<compression_method_t>& legacy_compression_methods() const
    {
        return _legacy_compression_methods;
    }

    /**
     * @brief Set the legacy compression methods
     * @param version The compression methods to set.
     * @details
     * This field is no longer in use in TLS 1.3 and must be set to the single
     * null compression method.
     */
    void legacy_compression_methods(
            const std::vector<compression_method_t>& value)
    {
        _legacy_compression_methods = value;
    }

    /**
     * @brief Get the extensions
     */
    const std::vector<std::shared_ptr<extension>>& extensions() const {
        return _extensions;
    }

    /**
     * @brief Set the extensions
     * @param value The extensions
     */
    void extensions(const std::vector<std::shared_ptr<extension>>& value) {
        _extensions = value;
    }

private:
    /** The legacy protocol version */
    protocol_version_t _legacy_version;

    /** The 32-byte random value. */
    mud::crypto::random_t _random;

    /** The legacy session ID. */
    mud::crypto::data_t _legacy_session_id;

    /** The cipher suites */
    std::vector<cipher_suite_t> _cipher_suites;

    /* The legacy compression methods */
    std::vector<compression_method_t> _legacy_compression_methods;

    /* The extensions */
    std::vector<std::shared_ptr<extension>> _extensions;

    /** Friends */
    friend std::istream& operator>>(std::istream& istr, client_hello& rec);
};

/**
 * @brief TLS Server Hello message
 * @details
 */
class server_hello: public handshake
{
public:
    /**
     * @brief Construct a Server Hello message.
     */
    server_hello();

    /**
     * @brief Get legacy version
     */
    protocol_version_t legacy_version() const { return _legacy_version; }

    /**
     * @brief Set the legacy version
     * @param version The version to set.
     */
    void legacy_version(protocol_version_t version) {
        _legacy_version = version;
    }

    /**
     * @brief Get the random value.
     */
    const mud::crypto::random_t& random() const { return _random; }

    /**
     * @brief Set the random value.
     * @param version The random value to set.
     * @details
     * The default constructor already sets a random value. This method will
     * override this.
     */
    void random(const mud::crypto::random_t& value) { _random = value; }

    /**
     * @brief Get the legacy session ID.
     */
    const mud::crypto::data_t& legacy_session_id() const {
        return _legacy_session_id;
    }

    /**
     * @brief Set the legacy session ID.
     * @param version The seesion ID value to set.
     */
    void legacy_session_id(const mud::crypto::data_t& value) {
        _legacy_session_id = value;
    }

    /**
     * @brief Get the cipher suite.
     */
    cipher_suite_t cipher_suite() const { return _cipher_suite; }

    /**
     * @brief Set the cipher suite.
     * @param version The cipher suite to set.
     */
    void cipher_suite(const cipher_suite_t value) { _cipher_suite = value; }

    /**
     * @brief Get the legacy compression method
     */
    compression_method_t legacy_compression_method() const {
        return _legacy_compression_method;
    }

    /**
     * @brief Set the legacy compression method
     * @param version The compression method to set.
     * @details
     * This field is no longer in use in TLS 1.3 and must be set to the null
     * compression method.
     */
    void legacy_compression_method(compression_method_t value) {
        _legacy_compression_method = value;
    }

    /**
     * @brief Get the extensions
     */
    const std::vector<std::shared_ptr<extension>>& extensions() const {
        return _extensions;
    }

    /**
     * @brief Set the extensions
     * @param value The extensions
     */
    void extensions(const std::vector<std::shared_ptr<extension>>& value) {
        _extensions = value;
    }

private:
    /** The legacy protocol version */
    protocol_version_t _legacy_version;

    /** The random value. */
    mud::crypto::random_t _random;

    /** The legacy session ID. */
    mud::crypto::data_t _legacy_session_id;

    /** The cipher suite */
    cipher_suite_t _cipher_suite;

    /* The legacy compression method */
    compression_method_t _legacy_compression_method;

    /* The extensions */
    std::vector<std::shared_ptr<extension>> _extensions;

    /** Friends */
    friend std::istream& operator>>(std::istream& istr, server_hello& rec);
};

/**
 * @brief Read a TLS record (header only) from an input stream
 * @param istr The input stream to read from
 * @param rec The record header details.
 * @details
 * Read only the TLS record header from the input stream.
 */
std::istream&
operator>>(std::istream& istr, record& rec);

/**
 * @brief Read a TLS handshake (header only) from an input stream
 * @param istr The input stream to read from
 * @param msg The handshake header details.
 * @details
 * Read only the TLS handshake header from the input stream.
 */
std::istream&
operator>>(std::istream& istr, handshake& msg);

/**
 * @brief Read a TLS client-hello from an input stream
 * @param istr The input stream to read from
 * @param msg The message details.
 */
std::istream&
operator>>(std::istream& istr, client_hello& msg);

/**
 * @brief Read a TLS server-hello from an input stream
 * @param istr The input stream to read from
 * @param msg The message details.
 */
std::istream&
operator>>(std::istream& istr, server_hello& msg);

/**
 * @brief Write an TLS message to an output stream.
 * @param ostr The output stream
 * @param msg The message to write.
 */
std::ostream&
operator<<(std::ostream& ostr, const record& msg);

/**
 * @brief Read a TLS message from an input stream.
 * @param istr The input stream to read from
 * @param msg The placeholder to hold a new instance of the record received 
 * @details
 * The @c msg will be re-seated with a new instance of the associated record
 * specialisation. The @c msg can be an passed in as an uninitialised @c
 * shared_ptr .
 */
std::istream&
operator>>(std::istream& istr, std::shared_ptr<record>& rec);

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_MESSAGE_H_ */
