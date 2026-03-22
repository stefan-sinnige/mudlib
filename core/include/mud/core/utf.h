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

#ifndef _MUDLIB_CORE_UTF_H_
#define _MUDLIB_CORE_UTF_H_

#include <mud/core/ns.h>
#include <string>
#include <cstdint>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A Unicode Codepoint
 *
 * @details
 * A Unicode codepoint represents a particular position in a character table. In
 * Unicode character encodingsm these are represented in the Basic Multilingual
 * Plane (BMP) and are of the value between U+0000 to U+FFFF. Code points in the
 * supplementary plane are of the value of U+010000 to U+10FFFF. Its value will
 * effectively fit 16-bits for the BMP or 18-bits for the all the planes.
 *
 * A codepoint is therefore effectively a fixed 32-bit number.
 *
 * In practical terms, the code points are represented in a different format,
 * the Unicode Transformation Format (UTF), of which the following are supported
 *
 *   - UTF-8
 *   - UTF-16 (LE/BE)
 *   - UTF-32 (LE/BE)
 *
 * The UTF-8 encoding can represent a single codepoint as a sequent of one to
 * four bytes. The first code-point U+0000 (NUL) is represented as a single
 * null-character to determine the end of the character sequence.
 *
 * The UTF-16 represents each codepoint of the BMP by two bytes, and of the
 * supplementary plane as four bytes. This can either be in little-ending (LE)
 * or in big-endian (BE) format.
 *
 * The UTF-32 represents each code-point directly as fixed four bytes, without
 * an encoding scheme like UTF-8 and UTF-16 require. Just like UTF-16, this
 * can be either little-endian or big-endian.
 *
 * The Unicode standard is specified in ISO/IEC 10646.
 */
class MUDLIB_CORE_API codepoint
{
public:
    /**
     * @brief Create a NULL codepoint.
     */
    codepoint();

    /**
     * @brief Create a codepoint from a 32-bit value.
     * @param cp The codepoint value (host byte order).
     *
     * @note If the codepoint if out of its reserved range, an empty (NULL)
     * codepoint is created.
     */
     codepoint(uint32_t cp);

    /**
     * @brief Create a codepoint from a single @c U+XXXX or @c U+XXXXX string
     * representation.
     * @param cp The codepoint value.
     *
     * @note If the string cannot be parsed successfully, an empty (NULL)
     * codepoint is created.
     */
     codepoint(const std::string& cp);

    /**
     * @brief Copy constructor.
     * @param rhs The codepoint to copy from.
     */
    codepoint(const codepoint& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The codepoint to move from.
     */
    codepoint(codepoint&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~codepoint() = default;

    /**
     * @brief Copy assignment.
     * @param rhs The codepoint to copy from.
     * @return Reference to this object.
     */
    codepoint& operator=(const codepoint& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The codepoint to move from.
     * @return Reference to this object.
     */
    codepoint& operator=(codepoint&& rhs) = default;

    /**
     * @brief Equality operator.
     * @param rhs The codepoint to compare against.
     * @return True if the codepoints are the same.
     */
    bool operator==(const codepoint& rhs) const;

    /**
     * @brief Inequality operator.
     * @param rhs The codepoint to compare against.
     * @return True if the codepoints are not the same.
     */
    bool operator!=(const codepoint& rhs) const;

    /**
     * @brief Return the Unicode plane.
     * @details
     * Unicode defines a contiguous group of 65,536 codepoints as a plane. The
     * upper 16-bits of the codepoint equals the plane number and the lower
     * 16-bits define the codepoint within that plane.
     *
     * There are a total of 17 planes defined in the standard. Plane 0, also
     * known as the Basic Multilingual Plane (BMP) defines all the characters
     * for most of the modern languages. The remaining planes are known as the
     * Supplementary planes and define historic languages, symbols and others.
     * Most planes are as yet undefined and reserved for future use. Planes
     * 15 and 16 are private use planes.
     */
    uint8_t plane() const;

    /**
     * @brief Conversion to the 32-bit representation (host byte order).
     */
    operator uint32_t() const;

private:
    /** The inetrnal representation in host byte order. */
    uint32_t _cp;
};

/**
 * @brief A UTF-8 encoded character.
 *
 * @details
 * A single Unicode codepoint can be presented by one to four bytes in the UTF-8
 * encoding scheme. This class represents a single such codepoint and can
 * therefore contain multiple bytes, in contrast to the C++ standard @c char8_t.
 */
class MUDLIB_CORE_API utf8
{
public:
    /**
     * @brief Type-definition of the underlying base type.
     */
    typedef uint8_t underlying_type;

    /**
     * @brief Type-definition of the unicode character.
     */
    typedef std::vector<underlying_type> data_type;

    /**
     * @brief Create a NULL UTF-8 character.
     */
    utf8();

    /**
     * @brief Create a UTF-8 character from a Unicode codepoint.
     * @param cp The Unicode codepoint.
     */
    utf8(codepoint cp);

    /**
     * @brief Copy constructor.
     * @param rhs The UTF-8 value to copy from.
     */
    utf8(const utf8& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The UTF-8 value to move from.
     */
    utf8(utf8&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~utf8() = default;

    /**
     * @brief Copy assignment.
     * @param rhs The UTF-8 value to copy from.
     * @return Reference to this object.
     */
    utf8& operator=(const utf8& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The UTF-8 value to move from.
     * @return Reference to this object.
     */
    utf8& operator=(utf8&& rhs) = default;

    /**
     * @brief Equality operator.
     * @param rhs The UTF-8 value to compare against.
     * @return True if the UTF-8 values are the same.
     */
    bool operator==(const utf8& rhs) const;

    /**
     * @brief Inequality operator.
     * @param rhs The UTF-8 value to compare against.
     * @return True if the UTF-8 values are not the same.
     */
    bool operator!=(const utf8& rhs) const;

    /**
     * @brief Return the number of bytes of the UTF-8 encoded character.
     */
    size_t size() const ;

    /**
     * @brief The UTF-8 character bytes.
     * Depending on the Unicode codepoint it represents, it can contain up
     * to four characters.
     */
    const data_type& data() const;

    /**
     * @brief Represent the UTF-8 character as a Unicode codepoint.
     * @return The Unicode codepoint.
     */
    operator codepoint() const;

private:
    /**
     * The physical representation of the character as a sequence of bytes.
     */
    data_type _ch;
};

/**
 * @brief A UTF-16 encoded character.
 *
 * @details
 * A single Unicode codepoint can be presented by one or two bytes in the UTF-16
 * encoding scheme. This class represents a single such codepoint and can
 * therefore contain multiple bytes, in contrast to the C++ standard @c
 * char16_t.
 *
 * The representation is always in host-order.
 */
class MUDLIB_CORE_API utf16
{
public:
    /**
     * @brief Type-definition of the underlying base type.
     */
    typedef uint16_t underlying_type;

    /**
     * @brief Type-definition of the unicode character.
     */
    typedef std::vector<underlying_type> data_type;

    /**
     * @brief Create a NULL UTF-16 character.
     */
    utf16();

    /**
     * @brief Create a UTF-16 character from a Unicode codepoint.
     * @param cp The Unicode codepoint.
     */
    utf16(codepoint cp);

    /**
     * @brief Copy constructor.
     * @param rhs The UTF-16 value to copy from.
     */
    utf16(const utf16& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The UTF-16 value to move from.
     */
    utf16(utf16&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~utf16() = default;

    /**
     * @brief Copy assignment.
     * @param rhs The UTF-16 value to copy from.
     * @return Reference to this object.
     */
    utf16& operator=(const utf16& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The UTF-16 value to move from.
     * @return Reference to this object.
     */
    utf16& operator=(utf16&& rhs) = default;

    /**
     * @brief Equality operator.
     * @param rhs The UTF-16 value to compare against.
     * @return True if the UTF-16 values are the same.
     */
    bool operator==(const utf16& rhs) const;

    /**
     * @brief Inequality operator.
     * @param rhs The UTF-16 value to compare against.
     * @return True if the UTF-16 values are not the same.
     */
    bool operator!=(const utf16& rhs) const;

    /**
     * @brief Return the number of words of the UTF-16 encoded character.
     */
    size_t size() const ;

    /**
     * @brief The UTF-16 character bytes.
     * Depending on the Unicode codepoint it represents, it can contain up
     * to four characters.
     */
    const data_type& data() const;

    /**
     * @brief Represent the UTF-16 character as a Unicode codepoint.
     * @return The Unicode codepoint.
     */
    operator codepoint() const;

private:
    /**
     * The physical representation of the character as a sequence of bytes.
     */
    data_type _ch;
};

/**
 * @brief A UTF-32 encoded character.
 *
 * @details
 * A single Unicode codepoint can be presented by UTF-32 encoding scheme as a
 * direct mapping.
 *
 * The representation is always in host-order.
 */
class MUDLIB_CORE_API utf32
{
public:
    /**
     * @brief Type-definition of the underlying base type.
     */
    typedef uint32_t underlying_type;

    /**
     * @brief Type-definition of the unicode character.
     */
    typedef underlying_type data_type;

    /**
     * @brief Create a NULL UTF-32 character.
     */
    utf32();

    /**
     * @brief Create a UTF-32 character from a Unicode codepoint.
     * @param cp The Unicode codepoint.
     */
    utf32(codepoint cp);

    /**
     * @brief Copy constructor.
     * @param rhs The UTF-32 value to copy from.
     */
    utf32(const utf32& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The UTF-32 value to move from.
     */
    utf32(utf32&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~utf32() = default;

    /**
     * @brief Copy assignment.
     * @param rhs The UTF-32 value to copy from.
     * @return Reference to this object.
     */
    utf32& operator=(const utf32& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The UTF-32 value to move from.
     * @return Reference to this object.
     */
    utf32& operator=(utf32&& rhs) = default;

    /**
     * @brief Equality operator.
     * @param rhs The UTF-32 value to compare against.
     * @return True if the UTF-32 values are the same.
     */
    bool operator==(const utf32& rhs) const;

    /**
     * @brief Inequality operator.
     * @param rhs The UTF-32 value to compare against.
     * @return True if the UTF-32 values are not the same.
     */
    bool operator!=(const utf32& rhs) const;

    /**
     * @brief The UTF-32 character bytes.
     * Depending on the Unicode codepoint it represents, it can contain up
     * to four characters.
     */
    data_type data() const;

    /**
     * @brief Represent the UTF-32 character as a Unicode codepoint.
     * @return The Unicode codepoint.
     */
    operator codepoint() const;

private:
    /**
     * The physical representation of the character as a sequence of bytes.
     */
    data_type _ch;
};

namespace utf
{
    /**
     * @brief Manipulator to use big-ending streaming.
     */
    std::ios_base&
    be(std::ios_base& str);

    /**
     * @brief Manipulator to use little-ending streaming.
     */
    std::ios_base&
    le(std::ios_base& str);
};

END_MUDLIB_CORE_NS

#if 0

/**
 * @brief Character traits of the @c mud::core::utf8 character
 */
namespace std {
template<>
struct char_traits<mud::core::utf8>
{
    /** The type of the character. */
    typedef mud::core::utf8 char_type;

    /** The (maximum) integer value that can represent @c char_type. A UTF-8
     * character is at least 1 and at most 4 bytes. */
    typedef uint32_t int_type;

    /** The type that can represent offsets between positions in a stream */
    typedef std::streamoff off_type;

    /** The type to represent positions in a stream */
    typedef std::streampos pos_type;

    /** The type to represent conversion states of multibyte characters. */
    typedef std::mbstate_t state_type;

    /**
     * @brief Assign a character.
     * @param c1 The character to assign to.
     * @param c2 The character value to assign.
     */
    static void assign(char_type& c1, const char_type& c2);

    /**
     * @brief Assign a character to a sequence string.
     * @details Assigns character @c c to @c count characters in the sequence
     * starting at @c s. 
     * @param s The character sequence to assign to.
     * @param count The character to assign to.
     * @param c The character value to assign.
     * @return The value of @c s.
     */
    static char_type* assign(char_type* s, size_t count, char_type c);

    /**
     * @brief Compare two characters
     * @param a The character to compare.
     * @param b The character to compare.
     * @return @c true if @c a and @c b are equal.
     */
    static bool eq(char_type a, char_type b);

    /**
     * @brief Compare two characters
     * @param a The character to compare.
     * @param b The character to compare.
     * @return @c true if @c a is less than @c b.
     */
    static bool lt(char_type a, char_type b);

    /**
     * @brief Lexicographically compare two character sequences.
     * @param s1 The character sequencee to compare.
     * @param s2 The character sequencee to compare.
     * @param count The number of characters to compare.
     * @return @c Negative value if @c s1 is less than @c s2. Positive value if
     * @c s1 is greater than @c s2, and zero if @c s1 is equal to @c s2.
     */
    static int compare(const char_type* s1, const char_type* s2, size_t count);

    /**
     * @brief Return the number of characters of the character sequence.
     * @param s The character sequence to return the length of.
     * @return The number of characters in the sequence @c s.
     */
    static size_t length(const char_type* s);

    /**
     * @brief Find a character in a character sequence.
     * @param s The character sequence to search in.
     * @param count The number of characters to analyse.
     * @param c The character to search for.
     * @return The pointer to the first occurrence of the character @c in the
     * sequence @c s within the range of [@c s, @c s + @c count). Returns a
     * @c nullptr if the character is not found within the range.
     */
    static const char_type* find(const char_type* s, size_t count,
            const char_type& c);

    /**
     * @brief Move characters to another sequence.
     * @details Copies correctly, even if the [@c dst, @c dst + @c count) and
     * [@c src, @c src + @c count) ranges overlap. 
     * @param dst The character sequence to copy to.
     * @param src The character sequence to copy from.
     * @param count The number of characters to copy.
     * @return The @dst value.
     */
    static char_type* move(char_type* dst, const char_type* src, size_t count);

    /**
     * @brief Move characters to another sequence.
     * @details If the [@c dst, @c dst + @c count) and [@c src, @c src +
     * @c count) ranges overlap, the behaviour is undefined. 
     * @param dst The character sequence to copy to.
     * @param src The character sequence to copy.
     * @param count The number of characters to copy.
     * @return The @dst value.
     */
    static char_type* copy(char_type* dst, const char_type* src, size_t count);

    /**
     * @brief Test if a value is the end-of-file character (eof).
     * @param e The value to test
     * @return @c e if the value is not an end-of-file character.
     */
    static int_type not_eof(int_type);

    /**
     * @brief Return a value that does represents end-of-file (eof).
     * @details The value is not part of the character representation.
     * @return The value representing end-of-file.
     */
    static int_type  eof();

    /**
     * @brief Convert an integer to an equivalent character value.
     * @details If the integer value @c c cannot be represented to an equivalent
     * character, then the returned result is undefined.
     * @param c The value to convert.
     * @return The converted value.
     */
    static char_type to_char_type(int_type c);

    /**
     * @brief Convert a character value to an equivalent integer value.
     * @param c The value to convert.
     * @return The converted value.
     */
    static int_type to_int_type(char_type c);

    /**
     * @brief Test if two characters are equal.
     * @param a The character to compare.
     * @param b The character to compare.
     * @return @c true if the characters are equal.
     */
    static bool eq_int_type(int_type a, int_type b);
};

}; /* namespace std */

#endif

/** Read a Unicode codepoint from an input stream. */
std::istream&
operator>>(std::istream& istr, mud::core::codepoint&);

/** Write a Unicode codepoint to an output stream. */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::codepoint&);

#if 0

/** Read a UTF-8 character from an input stream. */
std::istream&
operator>>(std::istream& istr, mud::core::utf8&);

/** Read a UTF-16 character from an input stream. */
std::istream&
operator>>(std::istream& istr, mud::core::utf16&);

/** Read a UTF-32 character from an input stream. */
std::istream&
operator>>(std::istream& istr, mud::core::utf32&);

/** Write a UTF-8 character to an output stream. */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::utf8&);

/** Write a UTF-16 character to an output stream. */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::utf16&);

/** Write a UTF-32 character to an output stream. */
std::ostream&
operator<<(std::ostream& ostr, const mud::core::utf32&);

#endif

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_UTF_H_ */
