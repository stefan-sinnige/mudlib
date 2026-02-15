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

#ifndef _MUDLIB_CORE_JSON_H_
#define _MUDLIB_CORE_JSON_H_

#include <mud/core/ns.h>
#include <map>
#include <string>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A JSON Value.
 *
 * @details
 * Javascript Object Notation (JSON) is a widely used data interchange format
 * that has been particularly implemented in web communication. Often used
 * in REST (Representational State Transfer) API. JSON is defined to be
 * conforming RFC 8259.
 *
 * A JSON value can either be one of the following types:
 *    - null
 *    - string
 *    - 64-bit integer
 *    - double floating point
 *    - boolean
 *    - array of JSON values
 *    - object of key-value pairs fo a string typed key and a JSON value
 * The latter two values allow a structure representation of the data. An array
 * and an object can hold different JSON value types within the same structure.
 */
class MUDLIB_CORE_API json
{
public:
    /**
     * Type definition of a JSON string value.
     */
    typedef std::string string_t;

    /**
     * Type definition of a JSON integer number value.
     */
    typedef int64_t integer_t;

    /**
     * Type definition of a JSON decimal number value.
     */
    typedef long double decimal_t;

    /**
     * Type definition of a JSON boolean value.
     */
    typedef bool boolean_t;

    /**
     * Type definition of a JSON array value.
     */
    typedef std::vector<json> array_t;

    /**
     * Type definition of a JSON object value.
     */
    typedef std::map<std::string, json> object_t;

    /**
     * @brief The type of the value.
     */
    enum class type_t
    {
        null,       /*!< A null value */
        string,     /*!< A string value */
        integer,    /*!< An integer number value */
        decimal,    /*!< A decimal number value */
        boolean,    /*!< A boolean value */
        array,      /*!< An array of values */
        object      /*!< A key-value pair structured value */
    };

    /**
     * @brief Create a @c null JSON value.
     */
    json() {}
    json(std::nullptr_t) {}

    /**
     * @brief Create a string JSON value.
     * @param s The string value to use.
     */
    json(const char* s) : _value(std::string(s)) {}
    json(const std::string& s) : _value(s) {}

    /**
     * @brief Create an integer JSON value.
     * @param i The integer value to use.
     */
    json(int8_t i) : _value((int64_t)i) {}
    json(int16_t i) : _value((int64_t)i) {}
    json(int32_t i) : _value(i) {}
    json(int64_t i) : _value(i) {}
    json(uint8_t i) : _value((int64_t)i) {}
    json(uint16_t i) : _value((int64_t)i) {}
    json(uint32_t i) : _value((int64_t)i) {}
    json(uint64_t i) : _value((int64_t)i) {}

    /**
     * @brief Create a decimal or scientific floating-point number.
     * @param d The number value to use.
     */
    json(float d) : _value((long double)d) {}
    json(double d) : _value((long double)d) {}
    json(long double d) : _value(d) {}

    /**
     * @brief Create a boolean value.
     * @param b The boolean value to use.
     */
    json(bool b) : _value(b) {}

    /**
     * @brief Create an array of JSON values.
     * @param arr The vector array of JSON values to use.
     */
    json(const array_t& arr) : _value(arr) {}

    /**
     * @brief Create a JSON object value, which is a list of key-value pairs.
     * @param obj The map of key-value JSON values to use.
     */
    json(const object_t& obj) : _value(obj) {}

    /**
     * @brief Create a copy of a JSON value.
     * @param other The value to copy.
     */
    json(const json&) = default;

    /**
     * @brief Move a JSON value.
     * @param other The value to move.
     */
    json(json&&) = default;

    /**
     * @brief Copy assignment of a JSON value.
     * @param other The value to copy.
     */
    json& operator=(const json&) = default;

    /**
     * @brief Move assignment of a JSON value.
     * @param other The value to move.
     */
    json& operator=(json&&) = default;

    /**
     * @brief Comparison of JSON values.
     * @param rhs The value to compare against.
     * @return true if the value is equal.
     */
    bool operator==(const json& rhs) const {
        return _value == rhs._value;
    }

    /**
     * @brief Comparison of JSON values.
     * @param rhs The value to compare against.
     * @return true if the value is not equal.
     */
    bool operator!=(const json& rhs) const {
        return operator==(rhs);
    }

    /**
     * @brief Return the type of the value.
     */
    type_t type() const { return (type_t)_value.index(); }

    /**
     * @brief Return the value as a string.
     * @thow @c std::bad_variant_access if the JSON value is not an integer.
     */
    const string_t& string() const {
        return std::get<(int)type_t::string>(_value);
    }

    /**
     * @brief Return the value as an integer.
     * @thow @c std::bad_variant_access if the JSON value is not an integer.
     */
    integer_t integer() const {
        return std::get<(int)type_t::integer>(_value);
    }

    /**
     * @brief Return the value as a double floating-point.
     * @thow @c std::bad_variant_access if the JSON value is not a double.
     */
    decimal_t decimal() const {
        return std::get<(int)type_t::decimal>(_value);
    }

    /**
     * @brief Return the value as a boolean.
     * @thow @c std::bad_variant_access if the JSON value is not a boolean.
     */
    boolean_t boolean() const {
        return std::get<(int)type_t::boolean>(_value);
    }

    /**
     * @brief Return the value as an array.
     * @thow @c std::bad_variant_access if the JSON value is not an array.
     */
    const array_t& array() const {
        return std::get<(int)type_t::array>(_value);
    }

    /**
     * @brief Return the value as an object.
     * @thow @c std::bad_variant_access if the JSON value is not an object.
     */
    const object_t& object() const {
        return std::get<(int)type_t::object>(_value);
    }

private:
    /** The multi-variant data value */
    std::variant<std::monostate,
                 string_t,
                 integer_t,
                 decimal_t,
                 boolean_t,
                 array_t,
                 object_t> _value;
};

END_MUDLIB_CORE_NS

/** Read JSON from an input stream. */
std::istream&
operator>>(std::istream& istr, mud::core::json&);

/** Write JSON to an output stream. */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::json&);

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_JSON_H_ */
