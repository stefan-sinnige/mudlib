#ifndef _MUDLIB_HTTP_MESSAGE_H_
#define _MUDLIB_HTTP_MESSAGE_H_

#include <algorithm>
#include <iostream>
#include <mud/core/poly_vector.h>
#include <mud/http/field.h>
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
     * Type definition of the container of fields.
     */
    typedef mud::core::poly_vector<base_field> fields_type;

    /**
     * The HTTP message type
     */
    enum class type
    {
        REQUEST, /**< Request */
        RESPONSE /**< Response */
    };

    /**
     * Copy constructor.
     */
    message(const message& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~message() = default;

    /**
     * Assign an HTTP mesage.
     * @param[in] rhs The message details to copy.
     * @return Reference to this message.
     */
    message& operator=(const message& rhs) = default;

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
    message::type type() const;

    /**
     * Insert a new HTTP field in place.
     * @tparam Field  The field type to set.
     * @param[in] arg  The field value construction parameter.
     * @param[in] args  Additional construction parameters. 
     */
    template<typename Field, typename FieldValue, class... Args>
    void field(const FieldValue& arg, Args... args)
    {
        Field fld(arg, std::forward<Args>(args)...);
        _fields.insert(_fields.end(), fld);
    }

    /**
     * Insert an HTTP field.
     * @tparam Field  The field type to set.
     * @param[in] fld  The field to set.
     */
    template<typename Field>
    void field(const Field& fld) {
        _fields.insert(_fields.end(), fld);
    }

    /**
     * Return the first HTTP field found.
     * @tparam Field  The field type to return.
     * @return The value of the field.
     * @throw std::out_of_range Field is not defined in the message.
     */
    template<typename Field>
    const Field& field() const
    {
        auto iter = find(Field::field_type);
        if (iter == _fields.end()) {
            throw std::out_of_range("field not found");
        }
        return static_cast<const Field&>(*iter);
    }

    /**
     * Return an HTTP field by it's key name.
     * @param[in] key  The key to search for.
     * @return The value of the field.
     * @throw std::out_of_range Field is not defined in the message.
     */
    const base_field& field_by_key(const std::string& key) const;

    /**
     * Check if a  standard HTTP field has been defined.
     * @tparam Field  The field to verify.
     * @return True if the field has been defined in the message.
     */
    template<typename Field>
    bool exists() const
    {
        return std::find_if(_fields.begin(), _fields.end(),
                            [](const base_field& fld) {
                                return Field::field_type == fld.type();
                            }) != _fields.end();
    }

    /**
     * Check if an HTTP field has been defined, both standard and extension
     * fields.
     * @param[in] key  The field name to searcjh for.
     * @return True if the field has been defined in the message.
     */
    bool exists(const std::string& key) const;

    /**
     * Get all the header fields.
     */
    const fields_type& fields() const { return _fields; }
    fields_type& fields() { return _fields; }

    /**
     * @brief Return the total number of fields defined
     */
    size_t field_size() const;

    /**
     * Set the version.
     * @param[in] value  The version value to set.
     */
    void version(const http::version& value) { _version = value; }

    /**
     * Get the version.
     */
    const http::version& version() const { return _version; }
    http::version& version() { return _version; }

    /**
     * Set the entity body.
     * @param[in] value  The entity body value to set.
     */
    void entity_body(const http::entity_body& value) { _entity_body = value; }
    void entity_body(const std::string& value)
    {
        _entity_body = http::entity_body(value);
    }

    /**
     * Get the entity body.
     */
    const http::entity_body& entity_body() const { return _entity_body; }
    http::entity_body& entity_body() { return _entity_body; }

protected:
    /**
     * Construction of a message of a certain type.
     */
    message(enum message::type);

private:
    /**
     * Return an iterator to the first field found at or after the start
     * position that matches the type.
     * @param[in] start  The position to start searching for the matching item.
     * @param[in] type  The type to search for.
     */
    fields_type::iterator find(base_field::field type)
    {
        return std::find_if(
            _fields.begin(), _fields.end(),
            [type](const base_field& fld) { return type == fld.type(); });
    }

    /**
     * Return an iterator to the first field found at or after the start
     * position that matches the type.
     * @param[in] start  The position to start searching for the matching item.
     * @param[in] type  The type to search for.
     */
    fields_type::const_iterator find(base_field::field type) const
    {
        return std::find_if(
            _fields.begin(), _fields.end(),
            [type](const base_field& fld) { return type == fld.type(); });
    }

    /**
     * Return an iterator to the first field found at or after the start
     * position that matches the key.
     * @param[in] start  The position to start searching for the matching item.
     * @param[in] key  The key to search for.
     */
    fields_type::const_iterator find(const char* key) const
    {
        return std::find_if(_fields.begin(), _fields.end(),
                            [key](const base_field& fld) {
                                return strcasecmp(key, fld.key()) == 0;
                            });
    }

    /**
     * Return an iterator to the first field found at or after the start
     * position that matches the key.
     * @param[in] start  The position to start searching for the matching item.
     * @param[in] key  The key to search for.
     */
    fields_type::iterator find(const char* key)
    {
        return std::find_if(_fields.begin(), _fields.end(),
                            [key](const base_field& fld) {
                                return strcasecmp(key, fld.key()) == 0;
                            });
    }

    /**
     * Data members
     */
    enum message::type _type;
    http::version _version;
    http::entity_body _entity_body;
    fields_type _fields;

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
        _fields.insert(_fields.end(), fld);
    }
};

/** Read an HTTP message from an input stream. */
std::istream&
operator>>(std::istream&, message&);

/** Write an HTTP message to an output stream. */
std::ostream&
operator<<(std::ostream&, const message&);

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_MESSAGE_H_ */
