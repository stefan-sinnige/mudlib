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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) = 0;

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) = 0;

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

private:
    /** The extension type */
    extension_type_t _type;
};

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The list of names */
    std::vector<std::string> _names;

    /* Friends */
    friend std::istream& operator>>(std::istream&, server_name&);
    friend std::ostream& operator<<(std::ostream&, const server_name&);
};

/**
 * @brief Read a TLS server-name extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, server_name& ext);

/**
 * @brief Write a TLS server-name extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const server_name& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The maximum fragment length */
    uint16_t _length;

    /* Friends */
    friend std::istream& operator>>(std::istream&, max_fragment_length&);
    friend std::ostream& operator<<(std::ostream&, const max_fragment_length&);
};

/**
 * @brief Read a TLS max-fragment-length extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, max_fragment_length& ext);

/**
 * @brief Write a TLS maximum fragment length extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const max_fragment_length& ext);

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
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:

    /* Friends */
    friend std::istream& operator>>(std::istream&, status_request&);
    friend std::ostream& operator<<(std::ostream&, const status_request&);
};

/**
 * @brief Read a TLS status request extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, status_request& ext);

/**
 * @brief Write a TLS status request extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const status_request& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The list of groups */
    std::vector<key_exchange_group_t> _groups;

    /* Friends */
    friend std::istream& operator>>(std::istream&, supported_groups&);
    friend std::ostream& operator<<(std::ostream&, const supported_groups&);
};

/**
 * @brief Read a TLS supported groups extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, supported_groups& ext);

/**
 * @brief Write a TLS supported groups extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const supported_groups& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The list of algorithms */
    std::vector<signature_algorithm_t> _algos;

    /* Friends */
    friend std::istream& operator>>(std::istream&, signature_algorithms&);
    friend std::ostream& operator<<(std::ostream&, const signature_algorithms&);
};

/**
 * @brief Read a TLS signature algorithms extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, signature_algorithms& ext);

/**
 * @brief Write a TLS signature algorithms extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const signature_algorithms& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The heartbeat mode */
    mode_t _mode;

    /* Friends */
    friend std::istream& operator>>(std::istream&, heartbeat&);
    friend std::ostream& operator<<(std::ostream&, const heartbeat&);
};

/**
 * @brief Read a TLS heartbeat extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, heartbeat& ext);

/**
 * @brief Write a TLS heartbeat extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const heartbeat& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The list of protocols */
    std::vector<std::string> _protocols;

    /* Friends */
    friend std::istream& operator>>(std::istream&,
            application_layer_protocol_negotiation&);
    friend std::ostream& operator<<(std::ostream&,
            const application_layer_protocol_negotiation&);
};

/**
 * @brief Read a TLS application layer protocol negotiation extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, application_layer_protocol_negotiation& ext);

/**
 * @brief Write a TLS application layer protocol negotiation extension to an
 * output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr,
        const application_layer_protocol_negotiation& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The heartbeat mode */
    std::vector<certificate_type_t> _types;

    /* Friends */
    friend std::istream& operator>>(std::istream&,
            client_certificate_type&);
    friend std::ostream& operator<<(std::ostream&,
            const client_certificate_type&);
};

/**
 * @brief Read a TLS client certificate type extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, client_certificate_type& ext);

/**
 * @brief Write a TLS client certificate type extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const client_certificate_type& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The heartbeat mode */
    std::vector<certificate_type_t> _types;

    /* Friends */
    friend std::istream& operator>>(std::istream&,
            server_certificate_type&);
    friend std::ostream& operator<<(std::ostream&,
            const server_certificate_type&);
};

/**
 * @brief Read a TLS server certificate type extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, server_certificate_type& ext);

/**
 * @brief Write a TLS server certificate type extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const server_certificate_type& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The padding length (excluding the 4-btyte header) */
    size_t _length;

    /* Friends */
    friend std::istream& operator>>(std::istream&, padding&);
    friend std::ostream& operator<<(std::ostream&, const padding&);
};

/**
 * @brief Read a TLS padding extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, padding& ext);

/**
 * @brief Write a TLS padding extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const padding& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The pre-shread key offerings */
    std::vector<psk_identity> _offerings;

    /* Friends */
    friend std::istream& operator>>(std::istream&, pre_shared_key&);
    friend std::ostream& operator<<(std::ostream&, const pre_shared_key&);
};

/**
 * @brief Read a TLS pre-shared key extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, pre_shared_key& ext);

/**
 * @brief Write a TLS pre-shared key extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const pre_shared_key& ext);

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
     * @brief Get the data.
     */
    mud::crypto::data_t data() const { return _data; }

    /**
     * @brief Set the data.
     * @param value The data to set.
     */
    void data(const mud::crypto::data_t& value) { _data = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /** The data */
    mud::crypto::data_t _data;

    /* Friends */
    friend std::istream& operator>>(std::istream&, early_data&);
    friend std::ostream& operator<<(std::ostream&, const early_data&);
};

/**
 * @brief Read a TLS early-data extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, early_data& ext);

/**
 * @brief Write a TLS early-data extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const early_data& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The pre-shread key offerings */
    std::vector<protocol_version_t> _versions;

    /* Friends */
    friend std::istream& operator>>(std::istream&, client_supported_versions&);
    friend std::ostream& operator<<(std::ostream&,
            const client_supported_versions&);
};

/**
 * @brief Read a TLS supported versions for client hello extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, client_supported_versions& ext);

/**
 * @brief Write a TLS supported versions for client hello extension to an output
 * stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const client_supported_versions& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The supported version */
    protocol_version_t _version;

    /* Friends */
    friend std::istream& operator>>(std::istream&, server_supported_versions&);
    friend std::ostream& operator<<(std::ostream&,
            const server_supported_versions&);
};

/**
 * @brief Read a TLS supported version key for server hello extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, server_supported_versions& ext);

/**
 * @brief Write a TLS supported version for server hello extension to an output
 * stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const server_supported_versions& ext);

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
     * @brief Get the data.
     */
    mud::crypto::data_t data() const { return _data; }

    /**
     * @brief Set the data.
     * @param value The data to set.
     */
    void data(const mud::crypto::data_t& value) { _data = value; }

    /**
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /** The data */
    mud::crypto::data_t _data;

    /* Friends */
    friend std::istream& operator>>(std::istream&, cookie&);
    friend std::ostream& operator<<(std::ostream&, const cookie&);
};

/**
 * @brief Read a TLS cookie extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, cookie& ext);

/**
 * @brief Write a TLS cookie extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const cookie& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The pre-shread key offerings */
    std::vector<key_exchange_mode_t> _modes;

    /* Friends */
    friend std::istream& operator>>(std::istream&, psk_key_exchange_modes&);
    friend std::ostream& operator<<(std::ostream&,
            const psk_key_exchange_modes&);
};

/**
 * @brief Read a TLS PSK key exchange modes extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, psk_key_exchange_modes& ext);

/**
 * @brief Write a TLS PSK key exchange modes extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const psk_key_exchange_modes& ext);

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
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:

    /* Friends */
    friend std::istream& operator>>(std::istream&, certificate_authorities&);
    friend std::ostream& operator<<(std::ostream&,
            const certificate_authorities&);
};

/**
 * @brief Read a TLS certificate authorities extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, certificate_authorities& ext);

/**
 * @brief Write a TLS certificate authorities extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const certificate_authorities& ext);

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
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:

    /* Friends */
    friend std::istream& operator>>(std::istream&, oid_filters&);
    friend std::ostream& operator<<(std::ostream&, const oid_filters&);
};

/**
 * @brief Read a TLS certificate authorities extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, oid_filters& ext);

/**
 * @brief Write a TLS certificate authorities extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const oid_filters& ext);

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
     * @brief Return the extension size in bytes.
     */
    virtual size_t size() const override;

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:

    /* Friends */
    friend std::istream& operator>>(std::istream&, post_handshake_auth&);
    friend std::ostream& operator<<(std::ostream&, const post_handshake_auth&);
};

/**
 * @brief Read a TLS post-handshake auth extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, post_handshake_auth& ext);

/**
 * @brief Write a TLS post-handhake auth extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const post_handshake_auth& ext);

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /* The list of algorithms */
    std::vector<signature_algorithm_t> _algos;

    /* Friends */
    friend std::istream& operator>>(std::istream&, signature_algorithms_cert&);
    friend std::ostream& operator<<(std::ostream&,
            const signature_algorithms_cert&);
};

/**
 * @brief Read a TLS signature algorithms cert extension.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, signature_algorithms_cert& ext);

/**
 * @brief Write a TLS signature algorithms cert extension to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const signature_algorithms_cert& ext);

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
     * @brief Create a early data extension.
     */
    client_key_share();

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /** The data */
    std::vector<entry_t> _shares;

    /* Friends */
    friend std::istream& operator>>(std::istream&, client_key_share&);
    friend std::ostream& operator<<(std::ostream&, const client_key_share&);
};

/**
 * @brief Read a TLS key-share extension for client hello.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, client_key_share& ext);

/**
 * @brief Write a TLS key-share extension for client hello to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const client_key_share& ext);

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
     * @brief Create a early data extension.
     */
    server_key_share();

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

    /**
     * @brief Read an extension from an input stream.
     * @param istr The input stream, currently pointed to an extension.
     * @return The input stream.
     */
    virtual std::istream& read(std::istream& istr) override {
        istr >> *this;
        return istr;
    }

    /**
     * @brief Write an extension to an output stream.
     * @param istr The output stream.
     * @return The output stream.
     */
    virtual std::ostream& write(std::ostream& ostr) override {
        ostr << *this;
        return ostr;
    }

private:
    /** The key-share data */
    entry_t _share;

    /* Friends */
    friend std::istream& operator>>(std::istream&, server_key_share&);
    friend std::ostream& operator<<(std::ostream&, const server_key_share&);
};

/**
 * @brief Read a TLS key-share  extension for server hello.
 * @param istr The input stream to read from
 * @param ext The extention details.
 */
std::istream&
operator>>(std::istream& istr, server_key_share& ext);

/**
 * @brief Write a TLS key-share extension for server hello to an output stream.
 * @param ostr The output stream
 * @param ext The extension details.
 */
std::ostream&
operator<<(std::ostream& ostr, const server_key_share& ext);

/**
 * @brief Factory for TLS handshake extensions.
 * @details
 * Extension may only be allowed with certain handshake message type or have a
 * different specification.
 */
typedef ::mud::core::factory<std::pair<handshake_type_t,
                                       extension_type_t>,
                             extension> extension_factory;

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_EXTENSION_H_ */
