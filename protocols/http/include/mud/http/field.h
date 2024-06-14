#ifndef _MUDLIB_HTTP_FIELD_H_
#define _MUDLIB_HTTP_FIELD_H_

#include <cctype>
#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <mud/core/optional.h>
#include <mud/core/poly_vector.h>
#include <mud/core/uri.h>
#include <mud/http/ns.h>
#include <stdexcept>
#include <string>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief Base class for an HTTP field.
 */
class base_field
{
public:
    /**
     * The HTTP fields. Not all fields are supported by all versions of HTTP.
     */
    enum class field
    {
        /* HTTP 1.0 */
        ALLOW,             /**< The Allow field */
        AUTHORIZATION,     /**< The Authorization field */
        CONTENT_ENCODING,  /**< The Content-Encoding field */
        CONTENT_LENGTH,    /**< The Content-Length field */
        CONTENT_TYPE,      /**< The Content-Type field */
        DATE,              /**< The Date field */
        EXPIRES,           /**< The Expires field */
        FROM,              /**< The From field */
        IF_MODIFIED_SINZE, /**< The If-Modified-Since field */
        LAST_MODIFIED,     /**< The Last-Modified field */
        LOCATION,          /**< The Location field */
        PRAGMA,            /**< The Pragma field */
        REFERER,           /**< The Referer field */
        SERVER,            /**< The Server field */
        USER_AGENT,        /**< The User-Agent field */
        WWW_AUTHENTICATE,  /**< The WWW-Auh=thenticate field */
        /* HTTP 1.1 */
        CONNECTION,        /**< The Connection field */
        HOST,              /**< The Host field */
        TRANSFER_ENCODING, /**< The Transfer-Encoding field */
        /* HTTP Extensions for unrecognised fields */
        EXTENSION = -1     /**< An extension field */
    };

    /**
     * Destructor of an HTTP field.
     */
    virtual ~base_field() = default;

    /**
     * The field type.
     */
    virtual base_field::field type() const = 0;

    /**
     * The field name.
     */
    virtual const char* key() const = 0;

    /**
     * Write the value to an output stream.
     * @param[in] ostr  The output stream to write to.
     */
    virtual void value(std::ostream& ostr) const = 0;

    /**
     * Read the value from an input stream.
     * @param[in] ostr  The output stream to read from.
     */
    virtual void value(std::istream& istr) = 0;
};

/**
 * @brief Template class for an single-value HTTP field.
 * @tparam The type of the field.
 */
template<enum base_field::field Field, typename Type, const char* Key>
class field : public base_field
{
public:
    /** The HTTP field key name. */
    static constexpr char const* key_name = Key;

    /** The HTTP field type enumerated value. */
    static constexpr enum base_field::field field_type = Field;

    /** The type of the field value */
    typedef Type value_type;

    /** The field is not a multi-valued type */
    static constexpr bool multi_valued = false;

    /**
     * @brief Construct an HTTP field with an undefined value.
     */
    field() = default;

    /**
     * @brief Construct an HTTP field of a particular value.
     * @param[in] value The HTTP field value.
     */
    field(const value_type& value) : _value(value) {}

    /**
     * @brief Copy an HTTP field.
     * @param[in] rhs The HTTP field to copy the values from.
     */
    field(const field& rhs) = default;

    /**
     * @brief Assign the values of another HTTP field.
     * @param[in] rhs The HTTP Vfield to copy the values from.
     * @return Reference to this HTTP field.
     */
    field& operator=(const field& rhs) = default;

    /**
     * @brief Destructor
     */
    virtual ~field() = default;

    /**
     * The field type.
     */
    base_field::field type() const override { return field_type; }

    /**
     * The field name.
     */
    const char* key() const override { return key_name; }

    /**
     * @brief Return the HTTP field value.
     */
    value_type value() const { return _value; }

    /**
     * @brief Set the HTTP field value.
     * @param[in] value The HTTP field value.
     */
    void value(value_type value) { _value = value; }

    /**
     * @brief Implicit conversion to the HTTP field type.
     */
    operator value_type() const { return _value; }

    /**
     * Write the value to an output stream.
     * @param[in] ostr  The output stream to write to.
     */
    void value(std::ostream& ostr) const override { ostr << *this; }

    /**
     * Read the value from an input stream.
     * @param[in] ostr  The output stream to read from.
     */
    void value(std::istream& istr) override { istr >> *this; }

private:
    /**
     * The value of the field.
     */
    value_type _value;
};

/**
 * @brief Template class for an multi-value HTTP field.
 * @tparam The type of the field.
 */
template<enum base_field::field Field, typename Type, const char* Key>
class field_list : public base_field
{
public:
    /** The HTTP field key name. */
    static constexpr char const* key_name = Key;

    /** The HTTP field type enumerated value. */
    static constexpr enum base_field::field field_type = Field;

    /** The type of the field value */
    typedef Type value_type;

    /** The field is a multi-valued type */
    static constexpr bool multi_valued = true;

    /**
     * @brief Construct an HTTP field with an undefined value.
     */
    field_list() = default;

    /**
     * @brief Construct an HTTP field with a predefined values.
     * @param[in] value The HTTP field value(s).
     */
    template<class... Args>
    field_list(Args... args) {
        // Add all arguments in the same order
        (_value.push_back(std::forward<Args>(args)), ...);
    }

    /**
     * @brief Construct an HTTP field of a particular value.
     * @param[in] value The HTTP field value.
     */
    field_list(const std::list<value_type>& value) : _value(value) {}

    /**
     * @brief Copy an HTTP field.
     * @param[in] rhs The HTTP field to copy the values from.
     */
    field_list(const field_list& rhs) = default;

    /**
     * @brief Assign the values of another HTTP field.
     * @param[in] rhs The HTTP Vfield to copy the values from.
     * @return Reference to this HTTP field.
     */
    field_list& operator=(const field_list& rhs) = default;

    /**
     * @brief Destructor
     */
    virtual ~field_list() = default;

    /**
     * The field type.
     */
    base_field::field type() const override { return field_type; }

    /**
     * The field name.
     */
    const char* key() const override { return key_name; }

    /**
     * @brief Return the HTTP field value.
     */
    const std::list<value_type>& value() const { return _value; }

    /**
     * @brief Return the HTTP field value.
     */
    std::list<value_type>& value() { return _value; }

    /**
     * @brief Set the HTTP field value.
     * @param[in] value The HTTP field value.
     */
    void value(const std::list<value_type>& value) { _value = value; }

    /**
     * @brief Implicit conversion to the HTTP field type.
     */
    operator const std::list<value_type>&() const { return _value; }

    /**
     * Write the multi-value to an output stream in a comma-separated form.
     * @param[in] ostr  The output stream to write to.
     */
    void value(std::ostream& ostr) const override
    {
        auto iter = _value.cbegin();
        while (iter != _value.cend()) {
            if (iter != _value.cbegin()) {
                ostr << ", ";
            }
            ostr << *(iter++);
        }
    }

    /**
     * Read the value from an input stream in a comma-separated form.
     * @param[in] ostr  The output stream to read from.
     */
    void value(std::istream& istr) override
    {
        bool skip_csv(std::istream&);
        while (skip_csv(istr)) {
            value_type single_value;
            istr >> single_value;
            _value.push_back(single_value);
        }
    }

private:
    /**
     * The value of the field.
     */
    std::list<value_type> _value;
};

/**
 * Generic extension header field for unrecognised fields. These fields are
 * available by their name and provide thier value as a string.
 */
class field_ext : public base_field
{
public:
    /** The HTTP field type enumerated value. */
    static constexpr enum base_field::field field_type =
        base_field::field::EXTENSION;

    /** The field is not a multi-valued type */
    static constexpr bool multi_valued = false;

    /**
     * @brief Construct an HTTP field with an undefined value.
     * @param[in] key The HTTP field name.
     */
    field_ext(const std::string& key) : _key(key) {}

    /**
     * @brief Construct an HTTP field of a particular value.
     * @param[in] key The HTTP field name.
     * @param[in] value The HTTP field value.
     */
    field_ext(const std::string& key, const std::string& value)
      : _key(key), _value(value)
    {
    }

    /**
     * @brief Copy an HTTP field.
     * @param[in] rhs The HTTP field to copy the values from.
     */
    field_ext(const field_ext& rhs) = default;

    /**
     * @brief Assign the values of another HTTP field.
     * @param[in] rhs The HTTP Vfield to copy the values from.
     * @return Reference to this HTTP field.
     */
    field_ext& operator=(const field_ext& rhs) = default;

    /**
     * @brief Destructor
     */
    virtual ~field_ext() = default;

    /**
     * The field type.
     */
    base_field::field type() const override { return field_type; }

    /**
     * The field name.
     */
    const char* key() const override { return _key.c_str(); }

    /**
     * @brief Return the HTTP field value.
     */
    const std::string& value() const { return _value; }

    /**
     * @brief Set the HTTP field value.
     * @param[in] value The HTTP field value.
     */
    void value(const std::string& value) { _value = value; }

    /**
     * @brief Implicit conversion to the HTTP field type.
     */
    operator std::string() const { return _value; }

    /**
     * Write the value to an output stream.
     * @param[in] ostr  The output stream to write to.
     */
    void value(std::ostream& ostr) const override;

    /**
     * Read the value from an input stream.
     * @param[in] ostr  The output stream to read from.
     */
    void value(std::istream& istr) override;

private:
    /**
     * The name of the field.
     */
    std::string _key;

    /**
     * The value of the field.
     */
    std::string _value;
};
std::ostream&
operator<<(std::ostream&, const field_ext&);
std::istream&
operator>>(std::istream&, field_ext&);

/**
 * The class describing an HTTP version field.
 */
enum class version_e
{
    HTTP10, /**< HTTP/1.0 (RFC 1945) */
    HTTP11  /**< HTTP/1.1 (RFC 9110/9111/9112) */
};
extern const char _HTTP_VERSION[];
typedef field<(base_field::field)-10000, version_e, _HTTP_VERSION> version;
std::ostream&
operator<<(std::ostream&, const version&);
std::istream&
operator>>(std::istream&, version&);

/**
 * The class describing an HTTP method field.
 */
enum class method_e
{
    GET,  /**< HTTP GET method */
    HEAD, /**< HTTP HEAD method */
    POST  /**< HTTP POST method */
};
std::ostream&
operator<<(std::ostream&, const method_e&);
std::istream&
operator>>(std::istream&, method_e&);

extern const char _HTTP_METHOD[];
typedef field<(base_field::field)-10001, method_e, _HTTP_METHOD> method;
std::ostream&
operator<<(std::ostream&, const method&);
std::istream&
operator>>(std::istream&, method&);

/**
 * The class describing a relative HTTP uri field.
 */
extern const char _HTTP_URI[];
typedef field<(base_field::field)-10002, mud::core::uri, _HTTP_URI> uri;
std::ostream&
operator<<(std::ostream&, const uri&);
std::istream&
operator>>(std::istream&, uri&);

/**
 * The class describing an HTTP status-code field.
 */
enum class status_code_e : unsigned
{
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    MovedPermanently = 301,
    MovedTemporarily = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    InternalServerError = 405,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503
};
extern const char _HTTP_STATUS_CODE[];
typedef field<(base_field::field)-10003, status_code_e, _HTTP_STATUS_CODE>
    status_code;
std::ostream&
operator<<(std::ostream&, const status_code&);
std::istream&
operator>>(std::istream&, status_code&);

/**
 * The class describing an HTTP reason phrase field.
 */
enum class reason_phrase_e
{
    OK,
    Created,
    Accepted,
    NoContent,
    MovedPermanently,
    MovedTemporarily,
    NotModified,
    BadRequest,
    Unauthorized,
    Forbidden,
    NotFound,
    InternalServerError,
    NotImplemented,
    BadGateway,
    ServiceUnavailable
};
extern const char _HTTP_REASON_PHRASE[];
typedef field<(base_field::field)-10004, reason_phrase_e, _HTTP_REASON_PHRASE>
    reason_phrase;
std::ostream&
operator<<(std::ostream&, const reason_phrase&);
std::istream&
operator>>(std::istream&, reason_phrase&);

/**
 * The class describing an HTTP Entity-Body field.
 */
extern const char _HTTP_ENTITY_BODY[];
typedef field<(base_field::field)-10005, std::string, _HTTP_ENTITY_BODY>
    entity_body;
std::ostream&
operator<<(std::ostream&, const entity_body&);
std::istream&
operator>>(std::istream&, entity_body&);

/**
 * The class describing an HTTP Allow.
 */
extern const char _HTTP_ALLOW[];
typedef field_list<base_field::field::ALLOW, method_e, _HTTP_ALLOW> allow;
std::ostream&
operator<<(std::ostream&, const allow&);
std::istream&
operator>>(std::istream&, allow&);

/**
 * The class describing an HTTP Connection field.
 */
enum class connection_e
{
    Close,
    KeepAlive
};
extern const char _HTTP_CONNECTION[];
typedef field<base_field::field::CONNECTION, connection_e, _HTTP_CONNECTION>
    connection;
std::ostream&
operator<<(std::ostream&, const connection&);
std::istream&
operator>>(std::istream&, connection&);

/**
 * The class describing an HTTP Content-Length field.
 */
extern const char _HTTP_CONTENT_LENGTH[];
typedef field<base_field::field::CONTENT_LENGTH, int, _HTTP_CONTENT_LENGTH>
    content_length;
std::ostream&
operator<<(std::ostream&, const content_length&);
std::istream&
operator>>(std::istream&, content_length&);

/**
 * The class describing an HTTP Date (UTC/GMT).
 */
extern const char _HTTP_DATE[];
typedef field<base_field::field::DATE,
              std::chrono::time_point<std::chrono::system_clock>, _HTTP_DATE>
    date;
std::ostream&
operator<<(std::ostream&, const date&);
std::istream&
operator>>(std::istream&, date&);

/**
 * The class describing an HTTP Host field.
 */
extern const char _HTTP_HOST[];
typedef field<base_field::field::HOST, std::string, _HTTP_HOST>
    host;
std::ostream&
operator<<(std::ostream&, const host&);
std::istream&
operator>>(std::istream&, host&);

/**
 * The class describing an HTTP Transfer-Encoding field.
 */
enum class transfer_coding_e
{
    CHUNKED,  /**< Chunked encoding */
    COMPRESS, /**< Lempel-Ziv-Welch (LZW) */
    DEFLATE,  /**< ZLib Deflate */
    GZIP      /**< Lempel-Ziv (LZ77) with 32-bit CRC */
};
std::ostream&
operator<<(std::ostream&, const transfer_coding_e&);
std::istream&
operator>>(std::istream&, transfer_coding_e&);

extern const char _HTTP_TRANSFER_ENCODING[];
typedef field_list<base_field::field::TRANSFER_ENCODING, transfer_coding_e,
                   _HTTP_TRANSFER_ENCODING>
    transfer_encoding;
std::ostream&
operator<<(std::ostream&, const transfer_encoding&);
std::istream&
operator>>(std::istream&, transfer_encoding&);

/**
 * The factory of HTTP fields.
 *
 * This is a customisation of mud::core::factory to allow a non-standard
 * comparison on the key-field.
 */
class field_factory
{
public:
    /**
     * Type definition for a map of fields.
     */
    typedef mud::core::poly_vector<base_field> field_vector;

    /**
     * @brief The registrar that defines a concerete class and associated
     * concrete key to be registered in the factory.
     */
    template<const char* ConcreteKey, class ConcreteClass>
    class registrar
    {
    public:
        /**
         * @brief Constructor that registers the class and key into the
         * factory.
         */
        registrar()
        {
            field_factory::instance().register_registrar(ConcreteKey, creator);
        }

        /**
         * @brief Destructo.
         */
        ~registrar() {}

    private:
        /**
         * @brief The creator function that creates an instance of the
         * concrete class.
         */
        static mud::core::optional_ref<base_field> creator(field_vector& fields)
        {
            /* If the type is a multi-value list field, attempt to return a
             * reference to an already existing field. */
            if (ConcreteClass::multi_valued) {
                auto found = std::find_if(
                    fields.begin(), fields.end(), [](const auto& field) {
                        return field.type() == ConcreteClass::field_type;
                    });
                if (found != fields.end()) {
                    return mud::core::optional_ref<base_field>(*found);
                }
            }
            base_field& f = *(fields.insert(fields.end(), ConcreteClass()));
            return mud::core::optional_ref<base_field>(f);
        }

        /**
         * Not copyable.
         */
        registrar(const registrar&) = delete;
        registrar& operator=(const registrar&) = delete;

        /**
         * Not moveable.
         */
        registrar(registrar&&) = delete;
        registrar& operator=(registrar&&) = delete;
    };

    /**
     * Constructor.
     */
    field_factory() {}

    /**
     * Destructor.
     */
    ~field_factory() {}

    /**
     * @brief Create function to return a new instance associated to the
     * key.
     */
    mud::core::optional_ref<base_field> create(const char* key,
                                               field_vector& fields)
    {
        auto find = _map.find(key);
        if (find == _map.end()) {
            return mud::core::optional_ref<base_field>();
        }
        return find->second(fields);
    }

    /**
     * @brief The instance of the factory.
     */
    static field_factory& instance();

    /**
     * Not copyable.
     */
    field_factory(const field_factory&) = delete;
    field_factory& operator=(const field_factory&) = delete;

    /**
     * Not moveable.
     */
    field_factory(field_factory&&) = delete;
    field_factory& operator=(field_factory&&) = delete;

private:
    /**
     * The case insensitive lexicographical comparison.
     */
    struct less_case
    {
        bool operator()(const char* a, const char* b) const;
    };

    /**
     * Type definition of the creator function.
     */
    typedef std::function<mud::core::optional_ref<base_field>(field_vector&)>
        creator_type;

    /**
     * Type definition of the concrete key and concrete class creator function
     * mapping.
     */
    typedef std::map<const char*, field_factory::creator_type, less_case>
        creator_map_type;

    /**
     * @brief Register a registrar that handles the creation of an instance
     * of * concrete * class and is associated to a particular key.
     */
    void register_registrar(const char* key, creator_type creator)
    {
        _map[key] = creator;
    }

    creator_map_type _map;
};

END_MUDLIB_HTTP_NS

#endif /* _MUDLIB_HTTP_FIELD_H_ */
