#ifndef _MUDLIB_CORE_URI_H_
#define _MUDLIB_CORE_URI_H_

#include <mud/core/ns.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A Unified Resource Identifier.
 *
 * @details
 * A Unified Resource Identifier (URI) defines a specification to identiy a
 * generic reource. The URI is defined to be conforming RFC 3986.
 */
class MUDLIB_CORE_API uri
{
public:
    /**
     * A URI query is often represented as key/value pairs in the form of
     *    key=value&key=value
     */
    typedef std::vector<std::pair<std::string, std::string>> query_params_t;

    /**
     * Create an empty URI.
     */
    uri();

    /**
     * Create a URI from a string.
     * @param str  The string representation of the URI.
     */
    uri(const std::string& str);

    /**
     * Copy constructor.
     * @param rhs  The uri to copy from.
     */
    uri(const uri& rhs) = default;

    /**
     * Move constructor.
     * @param rhs  The uri to move from.
     */
    uri(uri&& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~uri() = default;

    /**
     * Copy assignment.
     * @param rhs  The uri to copy from.
     */
    uri& operator=(const uri& rhs) = default;

    /**
     * Move assignment.
     * @param rhs  The uri to move from.
     */
    uri& operator=(uri&& rhs) = default;

    /**
     * Equality operator.
     * @param other The uri to test against.
     * @return True if the @c other URI is the same. 
     */
    bool operator==(const uri& other) const;

    /**
     * In-equality operator.
     * @param other The uri to test against.
     * @return True if the @c other URI is not the same. 
     */
    bool operator!=(const uri& other) const;

    /**
     * @brief Return true if the URI is absolute.
     */
    bool absolute() const { return !_scheme.empty(); }

    /**
     * @brief Return true if the URI is relative.
     */
    bool relative() const { return _scheme.empty(); }

    /**
     * @brief Return the scheme part of the URI.
     */
    const std::string& scheme() const { return _scheme; }

    /**
     * @brief Set the scheme part of the URI.
     */
    void scheme(const std::string& value);

    /**
     * @brief Return the authority part of the URI. The authority is made of
     @ the user-info, host and port information.
     */
    std::string authority() const;

    /**
     * @brief Return the user-info part of the URI.
     * @note Passwords are not supported as part of the user-info.
     */
    const std::string& user_info() const { return _user_info; }

    /**
     * @brief Set the user-info part of the URI.
     * @note Passwords are not supported as part of the user-info.
     */
    void user_info(const std::string& value) { _user_info = value; }

    /**
     * @brief Return the host of the URI.
     */
    const std::string& host() const { return _host; }

    /**
     * @brief Set the host part of the URI.
     */
    void host(const std::string& value);

    /**
     * @brief Return the port of the URI.
     */
    uint16_t port() const { return _port; }

    /**
     * @brief Set the port part of the URI.
     */
    void port(uint16_t value) { _port = value; }

    /**
     * @brief Return the path of the URI.
     */
    const std::filesystem::path& path() const { return _path; }

    /**
     * @brief Set the path part of the URI.
     */
    void path(const std::filesystem::path& value);

    /**
     * @brief Return the query as a single entity.
     */
    const std::string& query() const { return _query; }

    /**
     * @brief Set the query from a single entity.
     * @note Setting the query with both the @c query and @c query_params
     * methods may lead to unexpected behaviour.
     */
    void query(const std::string& value);

    /**
     * @brief Return the query as query parameters.
     */
    const query_params_t& query_params() const { return _query_params; }

    /**
     * @brief Set the query from query parameters.
     * @note Setting the query with both the @c query and @c query_params
     * methods may lead to unexpected behaviour.
     */
    void query_params(const query_params_t& value);

    /**
     * @brief Return the fragment part of the URI.
     */
    const std::string& fragment() const { return _fragment; }

    /**
     * @brief Set the fragment part of the URI.
     */
    void fragment(const std::string& value) { _fragment = value; }

    /**
     * @brief Clear the URI.
     */
    void clear();

    /**
     * @brief Return true if the URI is empty.
     */
    bool empty() const;

    /**
     * Normalise the URI. This will perform
     *   Syntax-based normalisation
     *   Percent encoding normalisation
     *   Path segment normalisation
     *   Scheme-based normalisation
     *   Protocol-based normalisation
     */
    void normalise();

private:
    /**
     * Perform syntax normalisation.
     */
    void syntax_normalisation();

    /**
     * Perform case normalisation.
     */
    void case_normalisation();

    /**
     * Perform percent-encoding normalisation.
     */
    void percent_encoding_normalisation();

    /**
     * Perform path segment normalisation.
     */
    void path_segment_normalisation();

    /**
     * Perform scheme based normalisation.
     */
    void scheme_normalisation();

    /**
     * Perform protocol based normalisation.
     */
    void protocol_normalisation();

   /**
     * The scheme.
     */
    std::string _scheme;

    /**
     * The user-info.
     */
    std::string _user_info;

    /**
     * The host.
     */
    std::string _host;

    /**
     * The port.
     */
    uint16_t _port;

    /**
     * The path.
     */
    std::filesystem::path _path;

    /**
     * The query.
     */
    std::string _query;

    /**
     * The query as parameters.
     */
    query_params_t _query_params;

    /**
     * The fragment.
     */
    std::string _fragment;
};

/** Read a URI from an input stream, while applying encoding. */
std::istream&
operator>>(std::istream& istr, uri&);

/** Write a URI to an output stream, while applying decoding. */
std::ostream&
operator<<(std::ostream& ostr, const uri&);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_URI_H_ */
