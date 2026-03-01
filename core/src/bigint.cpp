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

#include "mud/core/bigint.h"
#include <algorithm>

BEGIN_MUDLIB_CORE_NS

bigint::bigint()
    : _size(1)
    , _value(new uint8_t[_size])
    , _positive(true)
{
    *_value = 0;
}

bigint::bigint(const std::string& str)
    : _positive(true)
{
    // On an empty string, we assume 0.
    if (str.empty()) {
        return;
    }

    // Allocate a binary buffer to hold both the BCD representation of the
    // string and the final binary result. The reverse double-dabble algorithm
    // is used to convert it. Use some conservative calculation of allocating
    // enough to hold both in one buffer.
    _size = str.size() + 1;
    _value = new uint8_t[_size];
    memset(_value, 0, _size);

    // If the first character is an explicit sign, process it.
    size_t index = 0;
    if (str[index] == '+' || str[index] == '-') {
        if (str[index] == '-') {
            _positive = false;
        }
        ++index;
    }

    // Convert the string to BCD until either we have reached the end or until
    // we have a non-digit.
    uint8_t* dst = _value;
    uint8_t shift = 4;
    size_t bcd = 0;
    for (; index < str.size(); ++index) {
        // Get the digit as a character
        char digit = str[index];
        if (digit < '0' || digit > '9') {
            break;
        }

        // Apply it
        ++bcd;
        *dst |=(digit - '0') << shift;
        if (shift == 4) {
            shift = 0;
        }
        else {
            ++dst;
            shift = 4;
        }
    }

    // Make sure that we move the BCD to a byte boundary, which would make the
    // conversion algorithm easier.
    if (shift == 0) {
        *this >>= 4;
    }

    // Keep track the start of the binary section and the number of bits in the
    // binary section.
    size_t binary = (bcd + 1)/2;
    size_t bits = 0;
    for (size_t bcd_bits = 0; bcd_bits < bcd*4; ++bcd_bits) {
        ++bits;
        *this >>= 1;
        for (size_t index = 0; index < binary; ++index) {
            if ((_value[index] & 0xF0) >= 0x80) {
                _value[index] -= 0x30;
            }
            if ((_value[index] & 0x0F) >= 0x08) {
                _value[index] -= 0x03;
            }
        }
    }

    // Shift the result to the right and compact it.
    *this >>= ((_size - binary)*8 - bits);
    compact();
}

bigint::bigint(const uint8_t* value, size_t size)
    : _size(size)
    , _value(new uint8_t[_size])
    , _positive(true)
{
    memcpy(_value, value, size);
}

bigint::bigint(const bigint& rhs)
    : _size(0)
    , _value(new uint8_t[_size])
    , _positive(true)
{
    (void) operator=(rhs);
}

bigint::bigint(bigint&& rhs)
    : _size(0)
    , _value(new uint8_t[_size])
    , _positive(true)
{
    (void) operator=(rhs);
}

bigint&
bigint::operator=(const bigint& rhs)
{
    if (this != &rhs) {
        delete []_value;
        _size = rhs._size;
        _value = new uint8_t[_size];
        _positive = rhs._positive;
        memcpy(_value, rhs._value, _size);
    }
    return *this;
}

bigint&
bigint::operator=(bigint&& rhs)
{
    if (this != &rhs) {
        _positive = rhs._positive;
        _value = rhs._value;
        _size = rhs._size;
        rhs._size = 0;
        rhs._value = new uint8_t[rhs._size];
        rhs._positive = true;
    }
    return *this;
}

std::strong_ordering
bigint::operator<=>(const bigint& rhs) const
{
    /* First, check the sign differences */
    if (_positive != rhs._positive) {
        return (_positive ? std::strong_ordering::greater
                          : std::strong_ordering::less);
    }

    /* Second, check the number of bits (signs are equal) */
    auto lhs_bits = bits();
    auto rhs_bits = rhs.bits();
    if (lhs_bits != rhs_bits) {
        if (_positive) {
            return (lhs_bits > rhs_bits ? std::strong_ordering::greater
                                        : std::strong_ordering::less);
        }
        else {
            return (lhs_bits > rhs_bits ? std::strong_ordering::less
                                        : std::strong_ordering::greater);
        }
    }

    /* Third, check the value (signs and bits are equal). Note that their
     * underlying sizes might be different. */
    ssize_t bytes = (lhs_bits - 1)/8 + 1;
    size_t lhs_idx = _size - bytes;
    size_t rhs_idx = rhs._size - bytes; 
    for (; bytes >= 0; --bytes, ++lhs_idx, ++rhs_idx) {
        if (_value[lhs_idx] != rhs._value[rhs_idx]) {
            if (_positive) {
                return (_value[lhs_idx] > rhs._value[rhs_idx]
                       ? std::strong_ordering::greater 
                       : std::strong_ordering::less);
            }
            else {
                return (_value[lhs_idx] > rhs._value[rhs_idx]
                       ? std::strong_ordering::less 
                       : std::strong_ordering::greater);
            }

        }
    }
    return std::strong_ordering::equal;
}

bool
bigint::operator==(const bigint& rhs) const
{
    return operator<=>(rhs) == std::strong_ordering::equal;
}

bool
bigint::operator!=(const bigint& rhs) const
{
    return !operator==(rhs);
}

bigint&
bigint::operator+=(const bigint& rhs)
{
    *this = add(*this, rhs);
    return *this;
}

bigint&
bigint::operator-=(const bigint& rhs)
{
    *this = subtract(*this, rhs);
    return *this;
}

bigint&
bigint::operator*=(const bigint& rhs)
{
    *this = multiply(*this, rhs);
    return *this;
}

bigint&
bigint::operator/=(const bigint& rhs)
{
    bigint quotient, remainder;
    if (!divide_modulo(*this, rhs, quotient, remainder)) {
        throw std::overflow_error("divide by zero");
    }
    *this = quotient;
    return *this;
}

bigint&
bigint::operator%=(const bigint& rhs)
{
    bigint quotient, remainder;
    if (!divide_modulo(*this, rhs, quotient, remainder)) {
        throw std::overflow_error("divide by zero");
    }
    *this = remainder;
    return *this;
}

bigint&
bigint::operator<<=(std::size_t pos)
{
    *this = shift_left(*this, pos);
    return *this;
}

bigint&
bigint::operator>>=(std::size_t pos)
{
    *this = shift_right(*this, pos);
    return *this;
}

size_t
bigint::bits() const
{
    // Get the first byte that holds a value. If there is no value (zero) we
    // assume the value zero represented by 1 bit.
    size_t index;
    for (index = 0; index < _size && _value[index] == 0; ++index);
    if (index >= _size) {
        return 1;
    }

    // Get the bit-position.
    size_t bits;
    uint8_t byte = _value[index];
    uint8_t mask = 0x80;
    for (bits = 8; bits > 0 && ((byte & mask) == 0); --bits, mask >>= 1);
    return (_size - index - 1) * 8 + bits;
}

bigint::~bigint()
{
    delete[] _value;
}

void
bigint::compact()
{
    // Get the required size in bytes.
    auto new_bits = bits();
    auto new_size = (new_bits == 0 ? 1 : (new_bits - 1)/8 + 1);

    // If already at the minimum size, there is nothing to do.
    if (new_size == _size) {
        return;
    }

    // Allocate the new buffer
    uint8_t* new_value = new uint8_t[new_size];
    memset(new_value, 0, new_size);

    // Copy over the old data, aligned to the right
    memcpy(new_value, &(_value[_size - new_size]), new_size);

    // Apply the new buffer
    delete[] _value;
    _size = new_size;
    _value = new_value;
}

void
bigint::resize(size_t new_size)
{
    // If the size is already the required size, we're done
    if (_size == new_size) {
        return;
    }

    // If the required size is more than desired, we bail out
    auto required = (bits() - 1)/8 + 1;
    if (required > new_size) {
        return;
    }

    // Allocate the new buffer
    uint8_t* new_value = new uint8_t[new_size];
    memset(new_value, 0, new_size);

    // Copy over the old data, aligned to the right
    memcpy(&(new_value[new_size - required]), &(_value[_size - required]),
           required);

    // Apply the new buffer
    delete[] _value;
    _size = new_size;
    _value = new_value;
}

std::partial_ordering
bigint::abs_cmp(const bigint& lhs, const bigint& rhs)
{
    /* Check the number of bits */
    auto lhs_bits = lhs.bits();
    auto rhs_bits = rhs.bits();
    if (lhs_bits > rhs_bits) {
        return std::partial_ordering::greater;
    }
    else
    if (lhs_bits < rhs_bits) {
        return std::partial_ordering::less;
    }

    /* Check the value (bits are equal). Note that their underlying sizes might
     * be different. */
    ssize_t bytes = (lhs_bits - 1)/8 + 1;
    size_t lhs_idx = lhs._size - bytes;
    size_t rhs_idx = rhs._size - bytes; 
    for (; bytes >= 0; --bytes, ++lhs_idx, ++rhs_idx) {
        if (lhs._value[lhs_idx] > rhs._value[rhs_idx]) {
            return std::partial_ordering::greater;
        }
        else
        if (lhs._value[lhs_idx] < rhs._value[rhs_idx]) {
            return std::partial_ordering::less;
        }
    }
    return std::partial_ordering::equivalent;
}

bigint
bigint::add(const bigint& lhs, const bigint& rhs)
{
    bigint result;
    if (lhs._positive && rhs._positive) {
        result = ripple_adder(lhs, rhs);
        result._positive = true;
        return result;
    }
    else
    if (!lhs._positive && !rhs._positive) {
        result = ripple_adder(lhs, rhs);
        result._positive = false;
        return result;
    }
    else
    if (lhs._positive && !rhs._positive) {
        auto cmp = abs_cmp(lhs, rhs);
        if (cmp == std::partial_ordering::greater) {
            result = ripple_subtractor(lhs, rhs);
            result._positive = true;
            return result;
        }
        else
        if (cmp == std::partial_ordering::less) {
            result = ripple_subtractor(rhs, lhs);
            result._positive = false;
            return result;
        }
        else {
            return result;
        }
    }
    else 
    if (!lhs._positive && rhs._positive) {
        auto cmp = abs_cmp(lhs, rhs);
        if (cmp == std::partial_ordering::greater) {
            result = ripple_subtractor(lhs, rhs);
            result._positive = false;
            return result;
        }
        else 
        if (cmp == std::partial_ordering::less) {
            result = ripple_subtractor(rhs, lhs);
            result._positive = true;
            return result;
        }
        else {
            return result;
        }
    }
    return result;
}

bigint
bigint::subtract(const bigint& lhs, const bigint& rhs)
{
    bigint result;
    if (lhs._positive && rhs._positive) {
        if (lhs >= rhs) {
            result = ripple_subtractor(lhs, rhs);
            result._positive = true;
            return result;
        }
        else {
            result = ripple_subtractor(rhs, lhs);
            result._positive = false;
            return result;
        }
    }
    else
    if (!lhs._positive && !rhs._positive) {
        auto cmp = abs_cmp(lhs, rhs);
        if (cmp == std::partial_ordering::greater) {
            result = ripple_subtractor(lhs, rhs);
            result._positive = false;
            return result;
        }
        else {
            result = ripple_subtractor(rhs, lhs);
            result._positive = true;
            return result;
        }
    }
    else
    if (lhs._positive && !rhs._positive) {
        result = ripple_adder(rhs, lhs);
        result._positive = true;
        return result;
    }
    else
    if (!lhs._positive && rhs._positive) {
        result = ripple_adder(rhs, lhs);
        result._positive = false;
        return result;
    }

    return result;
}

bigint
bigint::ripple_adder(const bigint& lhs, const bigint& rhs)
{
    bigint result;
    const bigint* augend = &lhs;
    const bigint* addend = &rhs;

    // Allocate the size of the result, which is the highest number of bits of
    // the augend and addend, plus 1 (carry).
    result.resize((std::max(augend->bits(), addend->bits()) - 1)/8 + 1);

    // Perform ripple carry addition for each byte, starting with LSB
    const uint8_t *augend_ptr = &(augend->_value[augend->_size-1]);
    const uint8_t *addend_ptr = &(addend->_value[addend->_size-1]);
    uint8_t *result_ptr = &(result._value[result._size-1]);
    uint8_t c_in = 0;
    uint8_t c_out = 0;
    while (true) {
        uint16_t sum = *augend_ptr + *addend_ptr + c_in;
        *result_ptr = sum & 0x00FF;
        c_out = (sum & 0x0100) ? 1 : 0;
        c_in = c_out;
        if (addend_ptr == addend->_value) {
            break;
        }
        --augend_ptr, --addend_ptr, --result_ptr;
    }
    return result;
}

bigint
bigint::ripple_subtractor(const bigint& lhs, const bigint& rhs)
{
    bigint result;
    const bigint* minuend = &lhs;
    const bigint* subtrahend = &rhs;

    // Allocate the size of the result, which is the highest number of bits of
    // the minuend.
    result.resize(minuend->_size);

    // Perform ripple borrow subtraction for each byte, starting with LSB
    const uint8_t *minuend_ptr = &(minuend->_value[minuend->_size-1]);
    const uint8_t *subtrahend_ptr = &(subtrahend->_value[subtrahend->_size-1]);
    uint8_t *result_ptr = &(result._value[result._size-1]);
    uint8_t b_in = 0;
    uint8_t b_out = 0;
    while (true) {
        uint16_t diff = 0x0100 + *minuend_ptr - *subtrahend_ptr - b_in;
        *result_ptr = diff & 0x00FF;
        b_out = ((diff & 0x0100) == 0x0000) ? 1 : 0;
        b_in = b_out;
        if (subtrahend_ptr == subtrahend->_value) {
            break;
        }
        --minuend_ptr, --subtrahend_ptr, --result_ptr;
    }
    return result;
}

bigint
bigint::multiply(const bigint& lhs, const bigint& rhs)
{
    bigint result;
    if (lhs > rhs) {
        result = long_multiply(lhs, rhs);
    }
    else {
        result = long_multiply(rhs, lhs);
    }
    result._positive = ((lhs._positive && rhs._positive) ||
                        (!lhs._positive && !rhs._positive));
    return result;
}

bigint
bigint::long_multiply(const bigint& multiplicant, const bigint& multiplier)
{
    // Reserve the resulting size to avoid multiple resizing
    bigint result;
    result.resize((multiplicant.bits() * 2 - 1)/8 + 1);

    // Copy the multiplicant that safely can be shifted without multiple
    // resizing operations. After every bit is examined from the multiplier,
    // this value will be shifted by one.
    bigint partial = multiplicant;
    partial.resize((multiplicant.bits() + multiplier.bits() - 1)/8 + 1);

    // Loop over every bit in the multiplier, from LSB to MSB. If the LSB is one
    // then add the partial value. Then shift the partial value to the left and
    // examine the next bit in the multiplier.
    for (ssize_t index = multiplier._size - 1; index >=0 ; --index) {
        uint8_t mask = 0x01;
        for (size_t bit = 0; bit < 8; ++bit) {
            if (multiplier._value[index] & mask) {
                result += partial;
            }
            mask <<= 1;
            partial <<= 1;
        }
    }
    return result;
}

bool
bigint::divide_modulo(const bigint& lhs, const bigint& rhs,
                      bigint& quotient, bigint& remainder)
{
    bool result = long_divide_modulo(lhs, rhs, quotient, remainder);
    if (result) {
        remainder._positive = true;
        quotient._positive = ((lhs._positive && rhs._positive) ||
                              (!lhs._positive && !rhs._positive));
    }
    return result;
}

bool
bigint::long_divide_modulo(const bigint& dividend, const bigint& divisor,
                           bigint& quotient, bigint& remainder)
{
    // If the divisor is zero, bail out as we cannot allow that.
    if (divisor == 0) {
        return false;
    }

    // If the divisor is larger than the dividend, there is nothing to do.
    if (abs_cmp(divisor, dividend) == std::partial_ordering::greater) {
        quotient = 0;
        remainder = dividend;
        return true;
    }

    // Start with a blank slate, and resize the quotient and remainder to their
    // maximum size to avoid resizing.
    quotient = remainder = 0;
    quotient.resize((dividend.bits() - 1)/8 + 1);
    remainder.resize((divisor.bits() - 1)/8 + 1);

    // Copy the first bits from the MSB of the dividend as the starting point
    // for the remainder. Calculate the start and end index to the dividend
    // value buffer that contains the first and last bit of the initial
    // remainder. Copy all of them over to the remainder and shift right to make
    // the LSB bit at the correct position.
    std::size_t sidx = dividend._size - (dividend.bits() - 1)/8 - 1;
    std::size_t eidx = sidx + divisor.bits()/8;
    uint8_t *src = &dividend._value[sidx];
    uint8_t *end = &dividend._value[eidx];
    uint8_t *dst = &remainder._value[remainder._size - (eidx - sidx) - 1];
    while (src <= end) {
        *dst++ = *src++;
    }
    uint8_t shift = (dividend.bits() - divisor.bits()) % 8;
    remainder >>= shift;

    // Perform long division by dividing the remainder with the divisor and
    // bringing down the next bit at every loop. Repeat until there are no more
    // bits to bring down.
    src = end;
    end = &dividend._value[dividend._size - 1];
    uint8_t mask = 0x01 << shift;

    while (true) {
        // Check if the remainder can be divided. If it can, record a '1' in the
        // quotient and perform the subtraction.
        quotient <<= 1;
        if (remainder >= divisor) {
            quotient._value[quotient._size - 1] |= 0x01;
            remainder -= divisor;
        }

        // Determine the bit mask to get the next one to bring down. We're done
        // if there are no more bits left.
        if (mask == 0x01) {
            if (++src > end) {
                break;
            }
            mask = 0x80;
        }
        else {
            mask >>= 1;
        }

        // Add the bit to the remainder
        remainder <<= 1;
        if (*src & mask) {
            remainder._value[remainder._size - 1] |= 0x01;
        }
    }
    return true;
}

bigint
bigint::shift_left(const bigint& value, std::size_t pos)
{
    // Copy with the value and resize it to fit the shifted result
    bigint result = value;
    size_t required_size = (result.bits() + pos - 1)/8 + 1;
    if (required_size > result._size) {
        result.resize(required_size);
    }

    // Calculate the number of bytes and remaining bits to shift.
    std::size_t bytes = pos / 8;
    std::size_t bits = pos % 8;

    // Shift whole bytes first
    if (bytes != 0) {
        for (int index = 0; index < (result._size - bytes); ++index) {
            result._value[index] = result._value[index + bytes];
        }
        for (int index = result._size - bytes; index < result._size; ++index) {
            result._value[index] = 0;
        }
    }

    // Shift the remaining bits, and carry-over a mask of the MSB bits from next
    // byte.
    for (int index = 0; index < result._size; ++index) {
        result._value[index] <<= bits;
        if (index < (result._size - 1)) {
            result._value[index] |= result._value[index+1] >> (8-bits);
        }
    }
    return result;
}

bigint
bigint::shift_right(const bigint& value, std::size_t pos)
{
    // Copy with the value and resize it to fit the shifted result
    bigint result = value;

    // Calculate the number of bytes and remaining bits to shift.
    std::size_t bytes = pos / 8;
    std::size_t bits = pos % 8;

    // Shift whole bytes first
    if (bytes != 0) {
        for (int index = (result._size - 1); index >= 0; --index) {
            result._value[index] = result._value[index - bytes];
        }
        for (int index = 0; index < bytes; ++index) {
            result._value[index] = 0;
        }
    }

    // Shift the remaining bits, and carry-over a mask of the LSB bits from the
    // previous byte.
    for (int index = (result._size - 1); index >= 0; --index) {
        result._value[index] >>= bits;
        if (index > 0) {
            result._value[index] |= result._value[index-1] << (8-bits);
        }
    }

    return result;
}

std::ostream&
operator<<(std::ostream& ostr, const bigint& b)
{ 
    // Using the double-dabble algorithm to convert binary to BCD first. Create
    // a scratch space that can hold the BCD and binary value such that the
    // BCD representation is at the MSB and binary representation at the LSB.
    // The additional size required for the BCD is '4*ceil(n/3)'
    bigint scratch = b;
    scratch.compact();
    size_t binary_bytes = scratch._size;
    size_t binary_bits  = scratch.bits();
    scratch.resize((binary_bits + 4*(binary_bits/3+1))/8 + 1);

    // Shift the binary part such that the MSB bit is at the most significant
    // bit. This means that with the first shift, it will move into the BCD
    // scratch space.
    if (binary_bits % 8 != 0) {
        scratch <<= (8 - binary_bits % 8);
    }

    // Shift all bits from the binary to the BCD scratch space one at a time and
    // examine all BCD values. Any BCD value that is at greater than 4, is
    // incremented by 3. This basically iteratively multiplies the binary number
    // by two (double) and move it to BCD while applying a carry.
    for (size_t bit = binary_bits; bit > 0; --bit) {
        scratch <<= 1;
        if (bit == 1) {
            // No dabble on final shift
            break;;
        }
        for (size_t byte = 0; byte < (scratch._size - binary_bytes); ++byte) {
            if ((scratch._value[byte] & 0xF0) > 0x40) {
                scratch._value[byte] += 0x30;
            }
            if ((scratch._value[byte] & 0x0F) > 0x04) {
                scratch._value[byte] += 0x03;
            }
        }
    }

    // BCD representation is now ready in the top section of the scratch space.
    // Skip the first numbers of 0's as they would have been fillers.
    size_t nibble = 0;
    bool skip = true;
    for (; nibble < 2*(scratch._size - binary_bytes); ++nibble) {
        // Get the BCD from the nibble
        uint8_t byte = scratch._value[nibble/2];
        uint8_t bcd;
        if (nibble % 2 == 0) {
            bcd = (byte & 0xF0) >> 4;
        }
        else {
            bcd = (byte & 0x0F);
        }
        if (skip) {
            if (bcd == 0) {
                continue;
            }
            else {
                skip = false;
                if (b.neg()) {
                    ostr << '-';
                }
            }
        }

        // Output the nibble
        ostr << char('0' + bcd);
    }

    // If there was nothing in the output, we have a zero
    if (skip) {
        ostr << '0';
    }
    return ostr;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
