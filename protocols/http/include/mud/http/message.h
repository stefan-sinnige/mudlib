#ifndef _MUDLIB_HTTP_MESSAGE_H_
#define _MUDLIB_HTTP_MESSAGE_H_

#include <chrono>
#include <iostream>
#include <map>
#include <mud/core/poly_map.h>
#include <mud/http/ns.h>
#include <string>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of an HTTP message. This conforms with
 *    RFC 1945: Section 4, 5 and 6
 */
class MUDLIB_HTTP_API message
{
public:
    /**
     * @brief Base class for an HTTP field.
     */
    class base_field
    {
    public:
        /**
         * Destructor of an HTTP field.
         */
        virtual ~base_field() = default;

        /**
         * Serialise the field to an output stream
         * @param[in] ostr The stream to output to.
         */
        virtual void serialise(std::ostream&) const = 0;
    };

    /**
     * The HTTP message type
     */
    enum class type_t
    {
        UNDETERMINED, /**< Undetermined */
        REQUEST,      /**< Request */
        RESPONSE      /**< Response */
    };

    /**
     * Default constructor.
     */
    message();

    /**
     * Copy constructor.
     */
    message(const message& rhs);

    /**
     * Destructor.
     */
    virtual ~message() = default;

    /**
     * Assign an HTTP mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    message& operator=(const message& rhs);

    /**
     * Clear the HTTP message to its undetermined state.
     */
    void clear();

    /**
     * Validate the message.
     * @return True if the message is valid.
     */
    bool valid() const;

    /**
     * The message type.
     */
    type_t type() const;
    void type(type_t value);

    /**
     * Template to set an HTTP field.
     * @tparam Field  The field type to set.
     * @param[in] value  The field value to set.
     */
    template<typename Field>
    void field(const Field& value)
    {
        _fields.insert(Field::field, value);
    }

    /**
     * Template to return an HTTP field value.
     * @tparam Field  The field type to return.
     * @return The value of the field.
     * @throw std::out_of_range Field is not defined in the message.
     */
    template<typename Field>
    const Field& field() const
    {
        return static_cast<const Field&>(_fields.at(Field::field));
    }

    /**
     * Template to check if an HTTP field has been defined.
     * @tparam Field  The field to verify.
     * @return True if the field has been defined in the message.
     */
    template<typename Field>
    bool exists() const
    {
        return _fields.find(Field::field) != _fields.cend();
    }

    /**
     * @brief Return the total number of fields defined.
     */
    size_t field_size() const;

private:
    /**
     * The HTTP fields. Not all fields are supported by all versions of HTTP.
     */
    enum class field_t
    {
        VERSION,        /**< The HTTP Version field */
        METHOD,         /**< The Method field */
        URI,            /**< The URI field */
        DATE,           /**< The Date field */
        STATUS_CODE,    /**< The Status-Code field */
        REASON_PHRASE,  /**< The Reason Phrase field */
        CONTENT_LENGTH, /**< The Content-Length field */
        ENTITY_BODY     /**< The Entity-Body */
    };

    /**
     * Data members
     */
    type_t _type;
    mud::core::poly_map<field_t, base_field> _fields;

    /**
     * Template to parse and set an HTTP field value from a string token.
     * @tparam Field  The field to parse.
     * @param[in] token The string token to parse.
     * @return The value of the HTTP field.
     * @throw std::out_of_range Field is not defined in the message.
     */
    template<typename Field>
    void parse_field(const std::string& token)
    {
        Field fld;
        fld.parse(token);
        _fields.insert(Field::field, fld);
    }

    /** Friends */
    friend class version;
    friend class method;
    friend class uri;
    friend class date;
    friend class status_code;
    friend class reason_phrase;
    friend class content_length;
    friend class entity_body;
    friend std::istream& operator>>(std::istream&, message&);
    friend std::ostream& operator<<(std::ostream&, const message&);
};

/** Read an HTTP message from an input stream. */
std::istream&
operator>>(std::istream&, message&);

/** Write an HTTP message to an output stream. */
std::ostream&
operator<<(std::ostream&, const message&);

/**
 * The class describing an HTTP version field.
 */
class version : public message::base_field
{
public:
    /**
     * The HTTP version.
     */
    enum Version
    {
        HTTP10 /**< HTTP/1.0 (RFC 1945) */
    };

    /** The type of the field value */
    typedef Version type_t;

    /**
     * @brief Construct an HTTP Version field.
     */
    version() : _value(Version::HTTP10) {}

    /**
     * @brief Construct an HTTP Version field of a particular version.
     * @param[in] value The HTTP Version.
     */
    version(Version value) : _value(value) {}

    /**
     * @brief Copy an HTTP Version field.
     * @param[in] rhs The HTTP Version to copy the valeus from.
     */
    version(const version& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Version field.
     * @param[in] rhs The HTTP Version to copy the valeus from.
     * @return Reference to this HTTP Version.
     */
    version& operator=(const version& rhs);

    /**
     * @brief Destructor
     */
    virtual ~version() = default;

    /**
     * @brief Return the HTTP Version value.
     */
    Version value() const { return _value; }

    /**
     * @brief Set the HTTP Version value.
     * @param[in] version The HTTP Version.
     */
    void value(Version value) { _value = value; }

    /**
     * @brief Impliicit conversion to the HTTP Version value.
     */
    operator Version() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the version field.
     */
    Version _value;

    /**
     * The field that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP method field.
 */
class method : public message::base_field
{
public:
    /**
     * Predefined mehods
     */
    static const std::string GET;
    static const std::string HEAD;
    static const std::string POST;

    /** The type of the field value */
    typedef std::string type_t;

    /**
     * @brief Construct an HTTP Method field.
     */
    method() = default;

    /**
     * @brief Construct an HTTP Method field of a particular method.
     * @param[in] value The HTTP Method.
     */
    method(const std::string& value) : _value(value) {}

    /**
     * @brief Construct an HTTP Method field of a particular method.
     * @param[in] value The HTTP Method.
     */
    method(const char* value) : _value(value) {}

    /**
     * @brief Copy an HTTP Method field.
     * @param[in] rhs The HTTP Method to copy the valeus from.
     */
    method(const method& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Method field.
     * @param[in] rhs The HTTP Method to copy the valeus from.
     * @return Reference to this HTTP Method.
     */
    method& operator=(const method& rhs);

    /**
     * @brief Destructor
     */
    virtual ~method() = default;

    /**
     * @brief Return the HTTP Method value.
     */
    const std::string& value() const { return _value; }

    /**
     * @brief Set the HTTP Method value.
     * @param[in] method The HTTP Method.
     */
    void value(const std::string& value) { _value = value; }

    /**
     * @brief Impliicit conmethod to the HTTP Method value.
     */
    operator std::string() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the version field.
     */
    std::string _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP uri field.
 */
class uri : public message::base_field
{
public:
    /** The type of the field value */
    typedef std::string type_t;

    /**
     * @brief Construct an HTTP URI field.
     */
    uri() = default;

    /**
     * @brief Construct an HTTP URI field of a particular location.
     * @param[in] value The HTTP URI location.
     */
    uri(const std::string& value) : _value(value) {}

    /**
     * @brief Construct an HTTP URI field of a particular location.
     * @param[in] value The HTTP URI location.
     */
    uri(const char* value) : _value(value) {}

    /**
     * @brief Copy an HTTP URI field.
     * @param[in] rhs The HTTP URI to copy the valeus from.
     */
    uri(const uri& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP URI field.
     * @param[in] rhs The HTTP URI to copy the valeus from.
     * @return Reference to this HTTP URI.
     */
    uri& operator=(const uri& rhs);

    /**
     * @brief Destructor
     */
    virtual ~uri() = default;

    /**
     * @brief Return the HTTP URI value.
     */
    const std::string& value() const { return _value; }

    /**
     * @brief Set the HTTP URI value.
     * @param[in] method The HTTP URI.
     */
    void value(const std::string& value) { _value = value; }

    /**
     * @brief Impliicit conmethod to the HTTP Method value.
     */
    operator std::string() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the version field.
     */
    std::string _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP status-code field.
 */
class status_code : public message::base_field
{
public:
    /**
     * Predefined status-codes.
     */
    enum StatusCode
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

    /** The type of the field value */
    typedef int type_t;

    /**
     * @brief Construct an HTTP Status-Code field.
     */
    status_code() = default;

    /**
     * @brief Construct an HTTP Status-Code field of a particular status.
     * @param[in] value The HTTP Status-Code.
     */
    status_code(int value) : _value(value) {}

    /**
     * @brief Copy an HTTP Status-Code field.
     * @param[in] rhs The HTTP Status-Code to copy the valeus from.
     */
    status_code(const status_code& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Status-Code field.
     * @param[in] rhs The HTTP Status-Code to copy the valeus from.
     * @return Reference to this HTTP Status-Code.
     */
    status_code& operator=(const status_code& rhs);

    /**
     * @brief Destructor
     */
    virtual ~status_code() = default;

    /**
     * @brief Return the HTTP Status-Code value.
     */
    int value() const { return _value; }

    /**
     * @brief Set the HTTP Status-Code value.
     * @param[in] status_code The HTTP Status-Code.
     */
    void value(int value) { _value = value; }

    /**
     * @brief Impliicit constatus_code to the HTTP Status-Code value.
     */
    operator int() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the status-code field.
     */
    int _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP Date (UTC/GMT).
 */
class date : public message::base_field
{
public:
    /** The type of the field value */
    typedef std::chrono::time_point<std::chrono::system_clock> type_t;

    /**
     * @brief Construct an HTTP Date field.
     */
    date() = default;

    /**
     * @brief Construct an HTTP Date field of a particular date.
     * @param[in] value The HTTP Date.
     */
    date(const std::chrono::time_point<std::chrono::system_clock>& value)
      : _value(value)
    {}

    /**
     * @brief Copy an HTTP Date field.
     * @param[in] rhs The HTTP Date to copy the valeus from.
     */
    date(const date& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Date field.
     * @param[in] rhs The HTTP Date to copy the valeus from.
     * @return Reference to this HTTP Date.
     */
    date& operator=(const date& rhs);

    /**
     * @brief Destructor
     */
    virtual ~date() = default;

    /**
     * @brief Return the HTTP Date value.
     */
    const std::chrono::time_point<std::chrono::system_clock>& value() const
    {
        return _value;
    }

    /**
     * @brief Set the HTTP Date value.
     * @param[in] date The HTTP Date.
     */
    void value(const std::chrono::time_point<std::chrono::system_clock>& value)
    {
        _value = value;
    }

    /**
     * @brief Impliicit condate to the HTTP Date value.
     */
    operator std::chrono::time_point<std::chrono::system_clock>() const
    {
        return _value;
    }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the date field.
     */
    std::chrono::time_point<std::chrono::system_clock> _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/** Write an HTTP version to an output stream. */
std::ostream&
operator<<(std::ostream&, date::type_t);

/**
 * The class describing an HTTP reason-phrase field.
 */
class reason_phrase : public message::base_field
{
public:
    /**
     * Predefined reason phrases
     */
    static const std::string OK;
    static const std::string Created;
    static const std::string Accepted;
    static const std::string NoContent;
    static const std::string MovedPermanently;
    static const std::string MovedTemporarily;
    static const std::string NotModified;
    static const std::string BadRequest;
    static const std::string Unauthorized;
    static const std::string Forbidden;
    static const std::string NotFound;
    static const std::string InternalServerError;
    static const std::string NotImplemented;
    static const std::string BadGateway;
    static const std::string ServiceUnavailable;

    /** The type of the field value */
    typedef std::string type_t;

    /**
     * @brief Construct an HTTP Reason-Phrase field.
     */
    reason_phrase() = default;

    /**
     * @brief Construct an HTTP Reason-Phrase field of a particular phrase.
     * @param[in] value The HTTP Reason-Phrase.
     */
    reason_phrase(const std::string& value) : _value(value) {}

    /**
     * @brief Construct an HTTP Reason-Phras field of a particular phrase.
     * @param[in] value The HTTP Method.
     */
    reason_phrase(const char* value) : _value(value) {}

    /**
     * @brief Copy an HTTP Reason-Phrase field.
     * @param[in] rhs The HTTP Reason-Phrase to copy the valeus from.
     */
    reason_phrase(const reason_phrase& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Reason-Phrase field.
     * @param[in] rhs The HTTP Reason-Phrase to copy the valeus from.
     * @return Reference to this HTTP Reason-Phrase.
     */
    reason_phrase& operator=(const reason_phrase& rhs);

    /**
     * @brief Destructor
     */
    virtual ~reason_phrase() = default;

    /**
     * @brief Return the HTTP Reason-Phrase value.
     */
    const std::string& value() const { return _value; }

    /**
     * @brief Set the HTTP Reason-Phrase value.
     * @param[in] reason_phrase The HTTP Reason-Phrase.
     */
    void value(const std::string& value) { _value = value; }

    /**
     * @brief Impliicit conreason_phrase to the HTTP Reason-Phrase value.
     */
    operator std::string() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the reason-phrase field.
     */
    std::string _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP Content-Length field.
 */
class content_length : public message::base_field
{
public:
    /** The type of the field value */
    typedef int type_t;

    /**
     * @brief Construct an HTTP Content-Length field.
     */
    content_length() = default;

    /**
     * @brief Construct an HTTP Content-Length field of a particular size.
     * @param[in] value The HTTP Content-Length.
     */
    content_length(int value) : _value(value) {}

    /**
     * @brief Copy an HTTP Content-Length field.
     * @param[in] rhs The HTTP Content-Length to copy the valeus from.
     */
    content_length(const content_length& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Content-Length field.
     * @param[in] rhs The HTTP Content-Length to copy the valeus from.
     * @return Reference to this HTTP Content-Length.
     */
    content_length& operator=(const content_length& rhs);

    /**
     * @brief Destructor
     */
    virtual ~content_length() = default;

    /**
     * @brief Return the HTTP Content-Length value.
     */
    int value() const { return _value; }

    /**
     * @brief Set the HTTP Content-Length value.
     * @param[in] content_length The HTTP Content-Length.
     */
    void value(int value) { _value = value; }

    /**
     * @brief Impliicit concontent_length to the HTTP Content-Length value.
     */
    operator int() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the content length field.
     */
    size_t _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
};

/**
 * The class describing an HTTP Entity-Body field.
 */
class entity_body : public message::base_field
{
public:
    /** The type of the field value */
    typedef std::string type_t;

    /**
     * @brief Construct an HTTP Entity-Body field.
     */
    entity_body() = default;

    /**
     * @brief Construct an HTTP Entity-Body field of a particular body.
     * @param[in] value The HTTP Entity-Body.
     */
    entity_body(const std::string& value) : _value(value) {}

    /**
     * @brief Construct an HTTP Entity-Body field of a particular body.
     * @param[in] value The HTTP Entity-Body.
     */
    entity_body(const char* value) : _value(value) {}

    /**
     * @brief Copy an HTTP Entity-Body field.
     * @param[in] rhs The HTTP Entity-Body to copy the valeus from.
     */
    entity_body(const entity_body& rhs) : _value(rhs._value) {}

    /**
     * @brief Assign the values of another HTTP Entity-Body field.
     * @param[in] rhs The HTTP Entity-Body to copy the valeus from.
     * @return Reference to this HTTP Entity-Body.
     */
    entity_body& operator=(const entity_body& rhs);

    /**
     * @brief Destructor
     */
    virtual ~entity_body() = default;

    /**
     * @brief Return the HTTP Entity-Body value.
     */
    const std::string& value() const { return _value; }

    /**
     * @brief Set the HTTP Entity-Body value.
     * @param[in] entity_body The HTTP Entity-Body.
     */
    void value(const std::string& value) { _value = value; }

    /**
     * @brief Impliicit conentity_body to the HTTP Entity-Body value.
     */
    operator std::string() const { return _value; }

    /**
     * Serialise the field to an output stream.
     * @param[in] ostr The stream to output to.
     */
    virtual void serialise(std::ostream& ostr) const;

private:
    /**
     * The value of the content body field.
     */
    std::string _value;

    /**
     * (One of) the fields that is used by this field.
     */
    static const message::field_t field;

    /**
     * Parse an HTTP version token value.
     * @param[in] token The HTTP version token value.
     * @throw std::out_of_range The token value isinvalid.
     */
    void parse(const std::string& token);

    /** Friends */
    friend class message;
    friend std::istream& operator>>(std::istream&, message&);
};

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_MESSAGE_H_ */
