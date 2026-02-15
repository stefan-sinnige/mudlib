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

#ifndef _MUDLIB_CORE_UUID_H_
#define _MUDLIB_CORE_UUID_H_

#include <stdint.h>
#include <iostream>
#include <string>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A Universally Unique Identifier (UUID) as defined by RFC-9562.
 *
 * @details
 * A UUID is a 128 bit number represented as a hexadecimal string of a series
 * of 6 hexadedimal octets of eight, four, four, four and twelve hexadecimal
 * characters, separated by a dash. For example, the following is a UUID
 * @code
 *    032db337-8e24-4f9c-835d-0a21f4719818
 * @endcode
 * The UUID is in practical terms unique with a probability of duplication
 * nearing zero. The UUID generation has eveolved over time and resulted in
 * a number of versions, differing in their method of generation:
 *  -# Version 1: MAC address and timestamp.
 *  -# Version 2: Reserved for DCE security.
 *  -# Version 3: MD5 hash of a namespace ID.
 *  -# Version 4: Random.
 *  -# Version 5: SHA-1 hash of a namespace ID.
 *  -# Version 6: MAC address and ordered timestamp.
 *  -# Version 7: Timestamp and random.
 * Version 4 is the default (and currently only supported method).
 */
class MUDLIB_CORE_API uuid_v4
{
public:
    /**
     * @brief Create a UUID number.
     * @param isNull True if a null-UUID is to be created.
     */
    uuid_v4(bool isNull = false);

    /**
     * @brief Create a new from a string representation.
     * @param str The UUID value.
     *
     * @details
     * If the string representation is not a valid UUID representation, the
     * value will be set to null.
     */
    uuid_v4(const std::string& str);

    /**
     * @bried Destructor.
     */
    ~uuid_v4() = default;

    /**
     * @brief Create new UUID as a copy of another one.
     * @param other The UUID to copy from.
     */
    uuid_v4(const uuid_v4& other) = default;

    /**
     * @brief Create new UUID by moving the value from another one.
     * @param other The UUID to move from.
     *
     * @details
     * The value of @c other will be set to the null value.
     */
    uuid_v4(uuid_v4&& other);

    /**
     * @brief Copy the UUID value for another one.
     * @param other The UUID to copy from.
     * @returns The reference to this instance. 
     */
    uuid_v4& operator=(const uuid_v4& other) = default;

    /**
     * @brief Move the UUID value for another one.
     * @param other The UUID to move from.
     * @returns The reference to this instance. 
     *
     * @details
     * The value of @c other will be set to the null value.
     */
    uuid_v4& operator=(uuid_v4&& other);

    /**
     * @brief Equality operator.
     * @param other The UUID to compare against,
     * @return True if the @c other UUID value matches.
     */
    bool operator==(const uuid_v4& other) const;

    /**
     * @brief In-Equality operator.
     * @param other The UUID to compare against,
     * @return True if the @c other UUID value do not match.
     */
    bool operator!=(const uuid_v4& other) const;

    /**
     * @brief Ordering of UUID values.
     * @param other The UUID to compare against,
     * @return True if the value is mathematically smaller than @c other.
     */
    bool operator<(const uuid_v4& other) const;

    /**
     * @brief Check if the UUID is null.
     *
     * @details
     * A UUID has a null value if all its bits are 0.
     */
    bool null() const;

    /**
     * @brief Return the string representation.
     * @return The UUID in the standard representation.
     */
    const std::string& str() const;

    /**
     * @brief Type conversion
     */
    operator const std::string&() const;

private:
    /**
     * @brief Clear the value.
     *
     * @details
     * Clearing the value will make this UUID a null value one.
     */
    void clear();

    /**
     * @brief Product the string representation of the number.
     */
    void to_string();

    /** The two 64-bit values. */
    uint64_t _lower, _upper;

    /** The string representation. */
    std::string _str;
};

/**
 * @brief The default UUID version is version 4 (fully random).
 */
typedef uuid_v4 uuid;

END_MUDLIB_CORE_NS

/**
 * @brief Write a UUID to an output stream.
 * @param ostr The stream to write to.
 * @param id The value to write.
 */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::uuid_v4& id);

/**
 * @brief Read a UUID from an input stream.
 * @param istr The stream to read from.
 * @param id The object to save the UUID to.
 */
std::istream&
operator>>(std::istream& istr, mud::core::uuid_v4& id);

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_UUID_H_ */
