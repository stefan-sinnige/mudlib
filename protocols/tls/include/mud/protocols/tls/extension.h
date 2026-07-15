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

#ifndef _MUDLIB_TLS_EXTENSION_H_
#define _MUDLIB_TLS_EXTENSION_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <mud/core/buffer.h>
#include <mud/protocols/tls/ns.h>
#include <mud/protocols/tls/common.h>
#include <mud/protocols/tls/exception.h>
#include <mud/protocols/tls/session.h>
#include <mud/crypto/types.h>
#include <mud/core/factory.h>

BEGIN_MUDLIB_TLS_NS

/**
 * @brief The base definition of an TLS extension
 * @see RFC 8446: Section 4.2
 */
class MUDLIB_TLS_API extension
{
public:
    /**
     * @brief Definition of a pointer to a record.
     */
    typedef std::shared_ptr<extension> ptr;

    /**
     * @brief Destructor.
     */
    virtual ~extension() = default;

    /**
     * @brief Return the extension type.
     */
    extension_type_t extension_type() const { return _type; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const = 0;

protected:
    /**
     * Constructor
     * @param extension_type The extension type
     */
    extension(extension_type_t extension_type);

    /**
     * Assert a value and throw an @c invalid_message exception if they are not
     * equal.
     * @param v1 Value to check against.
     * @param v2 Value to check against.
     */
    template<typename T1, typename T2>
    void assert(T1 v1, T2 v2) const {
        if (v1 != v2) {
            throw invalid_message();
        }
    }
    template<>
    void assert<uint16_t, extension_type_t>(
            uint16_t v1,
            extension_type_t v2) const
    {
        if (v1 != static_cast<uint16_t>(v2)) {
            throw invalid_message();
        }
    }

    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) = 0;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const = 0;

private:
    /** The extension type */
    extension_type_t _type;

    /* Friends */
    friend std::ostream& operator<<(std::ostream&, const extension&);
    friend std::istream& operator>>(std::istream&, extension&);
    friend std::istream& operator>>(std::istream&, extension::ptr&);
};

/**
 * @brief Write a TLS extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const extension& ext);

/**
 * @brief Read a TLS extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, extension& ext);

/**
 * @brief Read a TLS extension from an input stream.
 * @param istr The input stream to read from
 * @param ext The placeholder to hold a new instance of the extension received
 * @details
 * The @c ext will be re-seated with a new instance of the associated extension
 * specialisation. The @c ext can be an passed in as an uninitialised @c
 * shared_ptr .
 */
std::istream&
operator>>(std::istream& istr, extension::ptr& ext);

/**
 * @brief The TLS server-name extension.
 */
class server_name: public extension
{
public:
    /**
     * @brief Create an empty server-name extension.
     */
    server_name();

    /**
     * @brief Construct a server-name extension from a binary buffer.
     * @param ext The extension data
     */
    server_name(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of names.
     */
    const std::vector<std::string>& names() const { return _names; }

    /**
     * @brief Set the list of names.
     * @param value The list of names to set.
     */
    void names(const std::vector<std::string>& value) { _names = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The list of names */
    std::vector<std::string> _names;
};

/**
 * @brief The TLS server-name extension.
 */
class max_fragment_length: public extension
{
public:
    /**
     * @brief Create a maximum fragment length extension.
     */
    max_fragment_length();

    /**
     * @brief Construct a maximum fragment length extension from a binary
     * buffer.
     * @param ext The extension data
     */
    max_fragment_length(const mud::core::buffer_view& ext);

    /**
     * @brief Get the maximum fragment length.
     */
    uint16_t length() const { return _length; }

    /**
     * @brief Set the maximum fragment length.
     * @param value The length to set.
     * @details Only 512, 1024, 2048 and 4096 are allowed. If not defined, the
     * maximum length is set to 16348.
     */
    void length(uint16_t value) { _length = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The maximum fragment length */
    uint16_t _length;
    friend std::ostream& operator<<(std::ostream&, const max_fragment_length&);
};

/**
 * @brief The TLS certificate status request extension.
 * @note: Unsupported until DER encoding is implemented.
 */
class status_request: public extension
{
public:
    /**
     * @brief Create a status request extension.
     */
    status_request();

    /**
     * @brief Construct a status-request extension from a binary buffer.
     * @param ext The extension data
     */
    status_request(const mud::core::buffer_view& ext);

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;
};

/**
 * @brief The TLS supported-groups extension.
 * @details
 * Defines the supported key-exchange group (curves and finite fields), along
 * the @c key_share extension. Any discouraged and unsupported group may be
 * supplied but will not have an associated enumerated type (as defined
 * by the IANA recommendations).
 */
class supported_groups: public extension
{
public:
    /**
     * @brief Create an default supported-groups extension.
     */
    supported_groups();

    /**
     * @brief Construct a supported-groups extension from a binary buffer.
     * @param ext The extension data
     */
    supported_groups(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of groups.
     */
    const std::vector<key_exchange_group_t>& groups() const { return _groups; }

    /**
     * @brief Set the list of groups.
     * @param value The list of names to set.
     */
    void groups(const std::vector<key_exchange_group_t>& value) {
        _groups = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The list of groups */
    std::vector<key_exchange_group_t> _groups;
};

/**
 * @brief The TLS signature-algorithms extension for certificate verification
 * purposes.
 * @details
 * Defines the supported signature and hash algorithms. For example, to be
 * used in certificate verification. Any discouraged and unsupported algorithm
 * may be supplied but will not have an associated enumerated type (as defined
 * by the IANA recommendations).
 */
class signature_algorithms: public extension
{
public:
    /**
     * @brief Create an default signature algorithms extension.
     */
    signature_algorithms();

    /**
     * @brief Construct a signature-algorithms extension from a binary buffer.
     * @param ext The extension data
     */
    signature_algorithms(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of algorithms.
     */
    const std::vector<signature_algorithm_t>& algorithms() const {
        return _algos;
    }

    /**
     * @brief Set the list of algorithms.
     * @param value The list of names to set.
     */
    void algorithms(const std::vector<signature_algorithm_t>& value) {
        _algos = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The list of algorithms */
    std::vector<signature_algorithm_t> _algos;
};

/**
 * @brief The TLS server-name extension.
 */
class heartbeat: public extension
{
public:
    /**
     * The heartbeat mode types
     */
    enum class mode_t {
        peer_allowed_to_send = 1,
        peer_not_allowed_to_send = 2
    };

    /**
     * @brief Create a heartbeat extension.
     */
    heartbeat();

    /**
     * @brief Construct a heart-beat extension from a binary buffer.
     * @param ext The extension data
     */
    heartbeat(const mud::core::buffer_view& ext);

    /**
     * @brief Get the heartbeat mode
     */
    mode_t mode() const { return _mode; }

    /**
     * @brief Set the heartbeat mode.
     * @param value The mode to set.
     */
    void mode(mode_t value) { _mode = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The heartbeat mode */
    mode_t _mode;
};

/**
 * @brief The TLS application layer protoocl negotiation extension.
 * @details
 * Defines the protocols to use at the application layer.
 */
class application_layer_protocol_negotiation: public extension
{
public:
    /**
     * @brief Create an default supported-groups extension.
     */
    application_layer_protocol_negotiation();

    /**
     * @brief Construct a application_layer_protocol_negotiation extension from
     * a binary buffer.
     * @param ext The extension data
     */
    application_layer_protocol_negotiation(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of groups.
     */
    const std::vector<std::string>& protocols() const { return _protocols; }

    /**
     * @brief Set the list of protocols.
     * @param value The list of names to set.
     */
    void protocols(const std::vector<std::string>& value) {
        _protocols = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The list of protocols */
    std::vector<std::string> _protocols;
};

/**
 * @brief The TLS client certificate type extension.
 */
class client_certificate_type: public extension
{
public:
    /**
     * @brief Create a client certificate type extension.
     */
    client_certificate_type();

    /**
     * @brief Construct a client_certificate_type extension from a binary
     * buffer.
     * @param ext The extension data
     */
    client_certificate_type(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of types.
     */
    const std::vector<certificate_type_t>& types() const { return _types; }

    /**
     * @brief Set the list of protocols.
     * @param value The list of names to set.
     */
    void types(const std::vector<certificate_type_t>& value) {
        _types = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The heartbeat mode */
    std::vector<certificate_type_t> _types;
};

/**
 * @brief The TLS server certificate type extension.
 */
class server_certificate_type: public extension
{
public:
    /**
     * @brief Create a server certificate type extension.
     */
    server_certificate_type();

    /**
     * @brief Construct a server-certificate_type extension from a binary
     * buffer.
     * @param ext The extension data
     */
    server_certificate_type(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of types.
     */
    const std::vector<certificate_type_t>& types() const { return _types; }

    /**
     * @brief Set the list of protocols.
     * @param value The list of names to set.
     */
    void types(const std::vector<certificate_type_t>& value) {
        _types = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The heartbeat mode */
    std::vector<certificate_type_t> _types;
};

/**
 * @brief The TLS padding extension.
 */
class padding: public extension
{
public:
    /**
     * @brief Create a padding extension.
     */
    padding();

    /**
     * @brief Construct a padding extension from a binary buffer.
     * @param ext The extension data
     */
    padding(const mud::core::buffer_view& ext);

    /**
     * @brief Get the padding length (exluding the 4-byte etxension header).
     */
    size_t length() const { return _length; }

    /**
     * @brief Set the padding length.
     * @param value The padding length to set (exlcuding the 4-byte extension
     * header).
     */
    void length(size_t value) { _length = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The padding length (excluding the 4-btyte header) */
    size_t _length;
};

/**
 * @brief The TLS pre-shared key (PSK) extension
 */
class pre_shared_key: public extension
{
public:
    /**
     * @brief Create a pre-shared key extension.
     */
    pre_shared_key();

    /**
     * @brief Construct a pre-shared key extension from a binary buffer.
     * @param ext The extension data
     */
    pre_shared_key(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of offerings.
     */
    const std::vector<psk_identity>& offerings() const { return _offerings; }

    /**
     * @brief Set the list of offerings.
     * @param value The list of offerings to set.
     */
    void offerings(const std::vector<psk_identity>& value) {
        _offerings = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The pre-shared key offerings */
    std::vector<psk_identity> _offerings;
};

/**
 * @brief The TLS early-data extension
 * @details
 * The early-data extension signals a zero rount-trip handshake. This allows
 * the sending of encrypted application data alongside the initial Hello
 * message. The encryption key is derived from the pre-shared key of a previous
 * session.
 */
class early_data: public extension
{
public:
    /**
     * @brief Create a early data extension.
     */
    early_data();

    /**
     * @brief Construct an early-data extension from a binary buffer.
     * @param ext The extension data
     */
    early_data(const mud::core::buffer_view& ext);

    /**
     * @brief Get the data.
     */
    mud::core::buffer data() const { return _data; }

    /**
     * @brief Set the data.
     * @param value The data to set.
     */
    void data(const mud::core::buffer& value) { _data = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /** The data */
    mud::core::buffer _data;
};

/**
 * @brief The TLS supported versions extension for a client-hello extension.
 */
class client_supported_versions: public extension
{
public:
    /**
     * @brief Create a supported versions extension.
     */
    client_supported_versions();

    /**
     * @brief Construct a client supported versions extension from a binary
     * buffer.
     * @param ext The extension data
     */
    client_supported_versions(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of offerings.
     */
    const std::vector<protocol_version_t>& versions() const {
        return _versions;
    }

    /**
     * @brief Set the list of offerings.
     * @param value The list of offerings to set.
     */
    void versions(const std::vector<protocol_version_t>& value) {
        _versions = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The pre-shared key offerings */
    std::vector<protocol_version_t> _versions;
};

/**
 * @brief The TLS supported version extension for a server-hello extension.
 */
class server_supported_versions: public extension
{
public:
    /**
     * @brief Create a supported versions extension.
     */
    server_supported_versions();

    /**
     * @brief Construct a server supported versions extension from a binary
     * buffer.
     * @param ext The extension data
     */
    server_supported_versions(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of offerings.
     */
    protocol_version_t version() const { return _version; }

    /**
     * @brief Set the supported version.
     * @param value The version to set.
     */
    void version(protocol_version_t value) { _version = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The supported version */
    protocol_version_t _version;
};

/**
 * @brief The TLS cookie extension
 * @details
 * The cookie extension forces the client to demonstrate reachability and to
 * allow a server to offload state to the client.
 */
class cookie: public extension
{
public:
    /**
     * @brief Create a early data extension.
     */
    cookie();

    /**
     * @brief Construct a cookie extension from a binary buffer.
     * @param ext The extension data
     */
    cookie(const mud::core::buffer_view& ext);

    /**
     * @brief Get the data.
     */
    mud::core::buffer data() const { return _data; }

    /**
     * @brief Set the data.
     * @param value The data to set.
     */
    void data(const mud::core::buffer& value) { _data = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /** The data */
    mud::core::buffer _data;
};

/**
 * @brief The TLS pre-shared key exchange modes extension
 * @details
 * Defines the supported pre-shared key exchange modes.
 */
class psk_key_exchange_modes: public extension
{
public:
    /**
     * @brief Create a psk key exchange modes extension.
     */
    psk_key_exchange_modes();

    /**
     * @brief Construct a psk key exchange modes extension from a binary buffer.
     * @param ext The extension data
     */
    psk_key_exchange_modes(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of offerings.
     */
    const std::vector<key_exchange_mode_t>& modes() const { return _modes; }

    /**
     * @brief Set the list of offerings.
     * @param value The list of offerings to set.
     */
    void modes(const std::vector<key_exchange_mode_t>& value) {
        _modes = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The pre-shared key offerings */
    std::vector<key_exchange_mode_t> _modes;
};

/**
 * @brief The TLS certificate authorities extension.
 * @note: Unsupported until DER encoding is implemented.
 */
class certificate_authorities: public extension
{
public:
    /**
     * @brief Create a certificate authorities extension.
     */
    certificate_authorities();

    /**
     * @brief Construct a certificate authorities extension from a binary
     * buffer.
     * @param ext The extension data
     */
    certificate_authorities(const mud::core::buffer_view& ext);

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;
};

/**
 * @brief The TLS OID filters extension.
 * @note: Unsupported until DER encoding is implemented.
 */
class oid_filters: public extension
{
public:
    /**
     * @brief Create a OID filters extension.
     */
    oid_filters();

    /**
     * @brief Construct a OID filters extension from a binary buffer.
     * @param ext The extension data
     */
    oid_filters(const mud::core::buffer_view& ext);

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;
};

/**
 * @brief The TLS post-handshake auth extension.
 */
class post_handshake_auth: public extension
{
public:
    /**
     * @brief Create a post-handhsake auth extension.
     */
    post_handshake_auth();

    /**
     * @brief Construct a post-handshake auth extension from a binary buffer.
     * @param ext The extension data
     */
    post_handshake_auth(const mud::core::buffer_view& ext);

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;
};

/**
 * @brief The TLS signature-algorithms extension embedded in X.509 certificates.
 * @details
 * Defines the supported signature and hash algorithms. For example, to be
 * used in certificate verification. Any discouraged and unsupported algorithm
 * may be supplied but will not have an associated enumerated type (as defined
 * by the IANA recommendations).
 */
class signature_algorithms_cert: public extension
{
public:
    /**
     * @brief Create an default signature algorithms certs extension.
     */
    signature_algorithms_cert();

    /**
     * @brief Construct a signature algorithms extension from a binary buffer.
     * @param ext The extension data
     */
    signature_algorithms_cert(const mud::core::buffer_view& ext);

    /**
     * @brief Get the list of algorithms.
     */
    const std::vector<signature_algorithm_t>& algorithms() const {
        return _algos;
    }

    /**
     * @brief Set the list of algorithms.
     * @param value The list of names to set.
     */
    void algorithms(const std::vector<signature_algorithm_t>& value) {
        _algos = value;
    }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /* The list of algorithms */
    std::vector<signature_algorithm_t> _algos;
};

/**
 * @brief The key share extension for client hello
 * @details
 * The key share extension carries the cryptographic parameters.
 */
class client_key_share: public extension
{
public:
    /**
     * The key-share data entry
     */
    typedef std::pair<key_exchange_group_t, mud::crypto::key_t> entry_t;

    /**
     * @brief Create a client key share data extension.
     */
    client_key_share();

    /**
     * @brief Construct a client key share extension from a binary buffer.
     * @param ext The extension data
     */
    client_key_share(const mud::core::buffer_view& ext);

    /**
     * @brief Get the key shares.
     */
    const std::vector<entry_t>& shares() const { return _shares; }

    /**
     * @brief Set the key shares.
     * @param value The data to set.
     */
    void shares(const std::vector<entry_t>& value) { _shares = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /** The data */
    std::vector<entry_t> _shares;
};

/**
 * @brief The key share extension for server hello
 * @details
 * The key share extension carries the cryptographic parameters.
 */
class server_key_share: public extension
{
public:
    /**
     * The key-share data entry
     */
    typedef std::pair<key_exchange_group_t, mud::crypto::key_t> entry_t;

    /**
     * @brief Create a server key share data extension.
     */
    server_key_share();

    /**
     * @brief Construct a server key share extension from a binary buffer.
     * @param ext The extension data
     */
    server_key_share(const mud::core::buffer_view& ext);

    /**
     * @brief Get the key share.
     */
    const entry_t& share() const { return _share; }

    /**
     * @brief Set the key shares.
     * @param value The data to set.
     */
    void share(const entry_t& value) { _share = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

protected:
    /**
     * Read a TLS extension from an input stream.
     * @param istr The input stream.
     */
    virtual std::istream& read(std::istream& istr) override;

    /**
     * Write a TLS extension to an output stream.
     * @param ostr The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) const override;

private:
    /** The key-share data */
    entry_t _share;
};

/**
 * @brief Factory for TLS handshake extensions.
 * @details
 * Extension may only be allowed with certain handshake message type or have a
 * different specification.
 */
typedef ::mud::core::factory<std::pair<handshake_type_t,
                                       extension_type_t>,
                             extension,
                             const mud::core::buffer_view&> extension_factory;

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_EXTENSION_H_ */
