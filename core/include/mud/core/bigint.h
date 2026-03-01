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

#ifndef _MUDLIB_CORE_BIGINT_H_
#define _MUDLIB_CORE_BIGINT_H_

#include <mud/core/ns.h>
#include <compare>
#include <iostream>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Arbitrary length integer.
 * @details
 * The arbitrary length integers (or colloquially known as big integer) is a
 * representation of an integer value of arbitrary length. These provide the
 * integer calculation beyond the standard integer values of C++. Such
 * calculations are often required for cryptographic functionality, financial
 * institutions, scientific computations etc.
 *
 * The @c bigint class defines a single integer value that can be used in
 * integral calculations as seamlessly as ordinary built-in integers. They
 * can be constructed from a built-in integer type, from parsing a decimal
 * string representing the value, or from a big-endian consecutive sequence
 * of the value.
 *
 * The integeral operator overloading are applied to the @c bigint class and
 * include integer style
 *    - Addition and subtraction
 *    - Multiplication, Division and Modulus
 *    - Exponentiation
 *    - Comparison 
 */
class MUDLIB_CORE_API bigint
{
public:
    /**
     * @brief Construct an integer, holding a 8-bit value of zero.
     */
    bigint();

    /**
     * @brief Copy an integer from another integer.
     * @param rhs The integer value to copy form.
     */
    bigint(const bigint& rhs);

    /**
     * @brief Move an integer from another integer.
     * @param rhs The integer value to move. The value will be set to zero.
     */
    bigint(bigint&& rhs);

    /**
     * @brief Construct an integer from a decimal string.
     * @param str The string containing a decimal integer number.
     */
    bigint(const std::string& str);

    /**
     * @brief Construct a signed integer from the built-in types.
     * @param value The integral value.
     * @tparam T The built-in integral type.
     */
    template<std::signed_integral T>
    bigint(T value);

    /**
     * @brief Construct an unsigned integer from the built-in types.
     * @param value The integral value.
     * @tparam T The built-in integral type.
     */
    template<std::unsigned_integral T>
    bigint(T value);

    /**
     * @brief Construct an unsigned integer value from a binary array.
     * @param value The binary array in big-endian form.
     * @param size The number of bytes in the binary array to use.
     * @details
     * Construct an unsigned integer directly from a binary big-endian array.
     * The array is copied to the big integer as-is.
     */
    bigint(const uint8_t* value, size_t size);

    /**
     * @brief Destructor.
     */
    ~bigint();

    /**
     * @brief Assign an integer from another integer.
     * @param rhs The integer value to copy form.
     * @return Reference to this object.
     */
    bigint& operator=(const bigint& rhs);

    /**
     * @brief Move an integer from another integer.
     * @param rhs The integer value to move. The value will be set to zero.
     * @return Reference to this object.
     */
    bigint& operator=(bigint&& rhs);

    /**
     * @brief Comparison with another integer
     * @param rhs The value to compare against.
     * @return The comparison result
     */
    std::strong_ordering operator<=>(const bigint& rhs) const;

    /**
     * @brief Comparison with another integer for eqaulity
     * @param rhs The value to compare against.
     * @return @true if the integer values are equal.
     */
    bool operator==(const bigint& rhs) const;

    /**
     * @brief Comparison with another integer for in-eqaulity
     * @param rhs The value to compare against.
     * @return @true if the integer values are equal.
     */
    bool operator!=(const bigint& rhs) const;

    /**
     * @brief Add an integral value.
     * @param rhs The value to add.
     * @return Reference to the current object.
     */
    bigint& operator+=(const bigint& rhs);

    /**
     * @brief Subtract an integral value.
     * @param rhs The value to subtract.
     * @return Reference to the current object.
     */
    bigint& operator-=(const bigint& rhs);

    /**
     * @brief Multiply an integral value.
     * @param rhs The value to multiply with.
     * @return Reference to the current object.
     */
    bigint& operator*=(const bigint& rhs);

    /**
     * @brief Compute the quotient of a division with an integral value.
     * @param rhs The value to divide with.
     * @return Reference to the current object.
     */
    bigint& operator/=(const bigint& rhs);

    /**
     * @brief Compute the remainder of a division with an integral value.
     * @param rhs The value to divide with.
     * @return Reference to the current object.
     */
    bigint& operator%=(const bigint& rhs);

    /**
     * @brief Perform a binay shift to the left.
     * @param pos The number of bits to shift.
     * @return Reference to the current object.
     */
    bigint& operator<<=(std::size_t pos);

    /**
     * @brief Perform a binay shift to the right.
     * @param pos The number of bits to shift.
     * @return Reference to the current object.
     */
    bigint& operator>>=(std::size_t pos);

    /**
     * @brief The number of significant bits that holds the value.
     * If the value is zero, the number of significant bits is 1 to reflect the
     * zero value. Note that this is not a reflection of the allocated size in
     * bytes returned by the @c size() function.
     */
    size_t bits() const;

    /**
     * @brief Check if the integer is a positive value
     * @return @c true if the value is positive.
     */
    bool pos() const { return _positive; }

    /**
     * @brief Check if the integer is a negative value
     * @return @c true if the value is negative.
     */
    bool neg() const { return !_positive; }

    /**
     * @brief The number of allocated bytes of the binary buffer.
     * This is larger or equal to the minimum number of bytes to hold its
     * current value (as represented by the @c bits() function).
     *
     * An allocated buffer of 1024 zero-bytes would return @c 1024 but only has
     * one significant bit as it represents the value of zero. Use the @c
     * compact() to resize the buffer to the minimum size to hold the current
     * value.
     */
    size_t size() const { return _size; }

    /**
     * @brief The pointer to the binary buffer.
     * The binary buffer stores the value as an unsigned value in big-endian
     * form, with the least significant byte allocated at the end of the buffer.
     */
    const uint8_t* data() const { return _value; }

    /**
     * @brief Resize the integer buffer
     * @param size The new size in bytes
     * Resize the underlying buffer to contain the desired size. If the size
     * is too small to hold the current value, the resize will be ignored.
     */
    void resize(size_t size);

    /**
     * @brief Compact the integer buffer.
     * @details
     * Ensure that the underlying buffer is of the minimum size to hold the
     * current value.
     */
    void compact();

private:
    /**
     * @brief Check the comparisons between two absolute values.
     * @param lhs The first operand to compare
     * @param rhs The second operand to compare
     * @return The @c std::partial_ordering based on their absolute values.
     */
    static std::partial_ordering abs_cmp(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Add two integers
     * @param lhs The augend
     * @param rhs The addend
     * @return The sum of @c lhs and @c rhs
     * Based on the @c lhs and @c rhs, the most performant option is chosen.
     */
    static bigint add(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Subtract two integers
     * @param lhs The minuend
     * @param rhs The subtrahend
     * @return The different of @c lhs and @c rhs
     * @details
     * Based on the @c lhs and @c rhs, the most performant option is chosen.
     */
    static bigint subtract(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Ripple adder on absolute numbers (sign ignored)
     * @param lhs The augend
     * @param rhs The addend
     * @return The sum of @c |lhs| and @c |rhs|
     * @details
     * Perform a ripple addition with carry for each eight-bit component of the
     * @c lhs and @c rhs.
     * @code
     *    result = | |lhs| + |rhs| | = | |rhs| + |lhs| |
     * @endcode
     * Assume that @c |lhs| is greater than @c |rhs|
     */
    static bigint ripple_adder(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Ripple subtractor on absolute numbers (sign ignored)
     * @param lhs The minuend
     * @param rhs The subtrahend
     * @return The absolute difference between @c |lhs| and @c |rhs|
     * @details
     * Perform a ripple subtraction with borrow for each eight-bit component of
     * the @c lhs and @c rhs.
     * @code
     *    result = | |lhs| - |rhs| | = | |rhs| - |lhs| |
     * @endcode
     * Assume that @c |lhs| is greater than @c |rhs|
     */
    static bigint ripple_subtractor(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Multiply two integers
     * @param lhs The mutliplicant
     * @param rhs The multiplier
     * @return The multiplication of @c lhs and @c rhs
     * @details
     * Based on the @c lhs and @c rhs, the most performant option is chosen.
     */
    static bigint multiply(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Multiply two integers using log multiplication
     * @param lhs The mutliplicant
     * @param rhs The multiplier (smaller than multiplicant)
     * @return The multiplication of @c lhs and @c rhs
     */
    static bigint long_multiply(const bigint& lhs, const bigint& rhs);

    /**
     * @brief Divide two integers
     * @param lhs The divident
     * @param rhs The divisor
     * @param quotient The computed quotient
     * @param remainder The computed remainder
     * @param Return @c true if the division could be made, or @c false if the
     * @c rhs was zero.
     * @details
     * Based on the @c lhs and @c rhs, the most performant option is chosen.
     */
    static bool divide_modulo(const bigint& lhs, const bigint& rhs,
                              bigint& quotient, bigint& remainder);

    /**
     * @brief Divide two integers using long division
     * @param lhs The divident
     * @param rhs The divisor
     * @param quotient The computed quotient
     * @param remainder The computed remainder
     * @param Return @c true if the division could be made, or @c false if the
     * @c rhs was zero.
     * @details
     * Based on the @c lhs and @c rhs, the most performant option is chosen.
     */
    static bool long_divide_modulo(const bigint& lhs, const bigint& rhs,
                                   bigint& quotient, bigint& remainder);

    /**
     * @brief Perform a binary shift to the left on a number.
     * @param value The number to apply the binary shoft to
     * @param pos The number of bits to shift to the left
     * @return The number after the shift has been applied.
     */
    static bigint shift_left(const bigint& value, std::size_t pos);

    /**
     * @brief Perform a binary shift to the right on a number.
     * @param value The number to apply the binary shift to
     * @param pos The number of bits to shift to the right
     * @return The number after the shift has been applied.
     */
    static bigint shift_right(const bigint& value, std::size_t pos);

    /* The size of the byte sequence */
    size_t _size;

    /* The byte sequence. This is the raw value in big-endian, disregarding the
     * sign. */
    uint8_t* _value;

    /* Boolean flag to indicate a the sign of the integer */
    bool _positive;

    /* Friends */
    friend mud::core::bigint operator+(const mud::core::bigint& b);
    friend mud::core::bigint operator-(const mud::core::bigint& b);
    friend mud::core::bigint operator+(const mud::core::bigint& lhs,
                                       const mud::core::bigint& rhs);
    friend mud::core::bigint operator-(const mud::core::bigint& lhs,
                                       const mud::core::bigint& rhs);
    friend mud::core::bigint operator*(const mud::core::bigint& lhs,
                                       const mud::core::bigint& rhs);
    friend mud::core::bigint operator/(const mud::core::bigint& lhs,
                                       const mud::core::bigint& rhs);
    friend mud::core::bigint operator%(const mud::core::bigint& lhs,
                                       const mud::core::bigint& rhs);
    friend std::ostream& operator<<(std::ostream& ostr, const bigint& b);
};

template<std::signed_integral T>
bigint::bigint(T value)
    : _size(sizeof(T))
    , _value(new uint8_t[_size])
    , _positive(true)
{
    if (value < 0) {
        _positive = false;
        value = -value;
    }
    memset(_value, 0, _size);
    uint8_t *dst = _value;
    for (ssize_t index = _size - 1; index >= 0; --index)
    {
        uint8_t byte = ((value & ((T)0xFF << index*8)) >> index*8) & 0xFF;
        *(dst++) = byte;
    }
}
 
template<std::unsigned_integral T>
bigint::bigint(T value)
    : _size(sizeof(T))
    , _value(new uint8_t[_size])
    , _positive(true)
{
    memset(_value, 0, _size);
    uint8_t *dst = _value;
    for (ssize_t index = _size - 1; index >= 0; --index)
    {
        uint8_t byte = ((value & ((T)0xFF << index*8)) >> index*8) & 0xFF;
        *(dst++) = byte;
    }
}

/**
 * @brief Arithmetic unary plus (promotion)
 * @param value The integer value.
 * @return The integer value.
 */
mud::core::bigint
operator+(const mud::core::bigint& value);

/**
 * @brief Arithmetic unary minus (negation)
 * @param value The integer value.
 * @return The negated representation of @c value.
 */
mud::core::bigint
operator-(const mud::core::bigint& value);

/**
 * @brief Arithmetic addition
 * @param lhs The augend
 * @param rhs The addend
 * @return The sum of @c lhs and @c rhs
 * Based on the @c lhs and @c rhs, the most performant option is chosen.
 */
mud::core::bigint
operator+(const mud::core::bigint& lhs, const mud::core::bigint& rhs);

/**
 * @brief Arithmetic subtraction
 * @param lhs The minuend
 * @param rhs The subtrahend
 * @return The difference of @c lhs and @c rhs
 * Based on the @c lhs and @c rhs, the most performant option is chosen.
 */
mud::core::bigint
operator-(const mud::core::bigint& lhs, const mud::core::bigint& rhs);

/**
 * @brief Arithmetic multiplication
 * @param lhs The multiplicant
 * @param rhs The multiplier
 * @return The multiplication of @c lhs and @c rhs
 * Based on the @c lhs and @c rhs, the most performant option is chosen.
 */
mud::core::bigint
operator*(const mud::core::bigint& lhs, const mud::core::bigint& rhs);

/**
 * @brief Arithmetic division
 * @param lhs The divident
 * @param rhs The divisor
 * @return The quotient of @c lhs divided by @c rhs
 * Based on the @c lhs and @c rhs, the most performant option is chosen.
 */
mud::core::bigint
operator/(const mud::core::bigint& lhs, const mud::core::bigint& rhs);

/**
 * @brief Arithmetic remainder
 * @param lhs The divident
 * @param rhs The divisor
 * @return The remainder of @c lhs divided by @c rhs
 * Based on the @c lhs and @c rhs, the most performant option is chosen.
 */
mud::core::bigint
operator%(const mud::core::bigint& lhs, const mud::core::bigint& rhs);

/**
 * @brief Output a @c bigint to an output stream.
 * @param ostr The stream to output to
 * @param b The arbitrary large integer to output
 * @return The reference of @c ostr
 */
std::ostream& operator<<(std::ostream& ostr, const bigint& b);

END_MUDLIB_CORE_NS

#endif /* _MUDLIB_CORE_BIGINT_H_ */
