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

#ifndef _MUDLIB_CRYPTO_X25519_H_
#define _MUDLIB_CRYPTO_X25519_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/types.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief Key-Exchange algorithm using Elliptic-Curve 25519.
 * @details
 * The Curve is an elliptic curve define by the Montgomory Curve
 *     @f$ y^2 = x^3 + 486662*x^2 + x @f$
 * and all operations are in the prime finite field modulo @f$ 2^255 - 19 @f$.
 * This curve and its constants are defined to ensure a secure and efficient
 * mechanism to create public/private key-pairs and exchanging public keying
 * materials.
 *
 * The key-exchange requires each side to generate a (random) 32-byte secret key
 * @f$ k_{a} @f$ that is only known to them. This number is multiplied by base
 * point 9 (generally known as @f$ G @f$) on the curve to create the public key
 * @f$ k_{a} \cdot G @f$ which is sent to the peer. The peer performs the same
 * action using its own generated private key and sends @f$ k_{b} \cdot G @f$.
 * Each side can then derive their shared secret key without transmitting this
 * key. The shared key is calculated by multiplying the private key with the
 * peer's public key as
 * @f$ k_{a} \cdot ( k_{b} \cdot P ) = k_{b} \cdot ( k_{a} \cdot P) @f$.
 *
 * The @c x25519 class provides the mathematical operations to generate the
 * private and calculate the associated public key-pair as well as to calculate
 * the shared secret key.
 */
class x25519
{
public:
    /**
     * @brief Create a X25519 public / private keypair.
     */
    x25519();

    /**
     * @brief Copy constructor.
     * @param rhs the object to copy from
     */
    x25519(const x25519& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs the object to move from
     */
    x25519(x25519&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs the object to copy from
     * @return reference to this object.
     */
    x25519& operator=(const x25519& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs the object to move from
     * @return reference to this object.
     */
    x25519& operator=(x25519&& rhs) = default;

    /**
     * @brief Create a X25519 public / private keypair.
     * @param p The private key to use.
     * @details
     * The actual private key being used will undergo 'clamping' procedures that
     * affect bits 0, 1, 2, 254 and 255.
     */
    x25519(const data_t& p);

    /**
     * @brief Supply the peer public key and create the shared secret key.
     * @param key The peer public key in little endian.
     */
    void peer(const data_t& key);

    /**
     * @brief Return the public key of the key-pair.
     * @return the public key (x-coordinate of a point on the curve) in little
     * endian.
     */
    const data_t& public_key() const { return _public; }

    /**
     * @brief Return the shared key.
     * @return the shared key in little endian, or an empty buffer if no shared
     * key has been determined.
     */
    const data_t& shared_key() const { return _shared; }

    /**
     * @brief A curve field-element.
     * @details
     * The @c field_element is the representation of an element in the curve
     * 25519 field, as 64-bit signed integers in native endian.
     */
    class field_element
    {
    public:
        /**
         * @brief Create an undefined element.
         */
        field_element();

        /**
         * @brief Create a field element from a binary buffer.
         * @param data The buffer to copy from. The data is expected to be
         * in little-endian (LSB in first position).
         */
        field_element(const data_t& data);

        /**
         * @brief Copy a field-element.
         * @param rhs The field element to copy from.
         */
        field_element(const field_element& rhs) = default;

        /**
         * @brief Move a field-element.
         * @param rhs The field element to move from.
         */
        field_element(field_element&& rhs) = default;

        /**
         * @brief Copy assign a field-element.
         * @param rhs The field element to copy from.
         * @return Reference to this.
         */
        field_element& operator=(const field_element& rhs) = default;

        /**
         * @brief Move assign a field-element.
         * @param rhs The field element to move from.
         * @return Reference to this.
         */
        field_element& operator=(field_element&& rhs) = default;

        /**
         * Convert to a binary buffer.
         */
        operator data_t() const;

        /**
         * Access the 64-bit integer at the n-th position.
         * @param n The position (0-3) of the integer.
         * @note The lest-significant integer is at the 0-th position.
         */
        int64_t operator[](size_t n) const { return _value[n]; }
        int64_t& operator[](size_t n) { return _value[n]; }
      
        /**
         * @brief Addition in field modulo p.
         * @param rhs The value to add.
         * @return Reference to the current object.
         */
        field_element& operator+=(const field_element& rhs);

        /**
         * @brief Subtraction in field modulo p.
         * @param rhs The value to subtract.
         * @return Reference to the current object.
         */
        field_element& operator-=(const field_element& rhs);

        /**
         * @brief Multiplication in modulo p.
         * @param rhs The value to multiply with.
         * @return Reference to the current object.
         */
        field_element& operator*=(const field_element& rhs);

    private:
        /**
         * @brief Conditionally swap values.
         * @param p First field element subject to a swap.
         * @param q Second field element subject to a swap.
         * @param c Condition to indicate if swap needs to occur or not.
         * If @c c is @c true, @c p and @c q are swapped. The implementation is
         * constant-time.
         */
        static void swap(field_element& p, field_element& q, bool c);

        /**
         * @brief Bring a field element close to the [0 - 2^16-1] range.
         * @param e The field element to reduce.
         * @details
         * Bring the field-element close to the range. For full enclosure of the
         * range, the function will need to be called three times, but for some
         * operatations a lesser amount is enough to ensure that an overflow
         * does not occur.
         */
        static void carry(field_element& e);

        /** The field element value in native endian.  The least-significant
         * integer is at the 0-th position. */
        int64_t _value[16];

        /** Friends */
        friend class x25519;
        friend field_element operator+(const field_element& lhs,
                                       const field_element& rhs);
        friend field_element operator-(const field_element& lhs,
                                       const field_element& rhs);
        friend field_element operator*(const field_element& lhs,
                                       const field_element& rhs);
        friend field_element inverse(const field_element& rhs);
        friend std::ostream& operator<<(std::ostream&, const field_element&);
    };

private:
    /**
     * @brief Perform the X25519 multiplication of the x-coordinates of two
     * points on the curve.
     * @param scalarn The multiplier.
     * @param point The x-coordinate of a point on the curve.
     * @return The x-coordinate on the curve representing the multiplication of
     * @c scalar and @c point.
     * @details
     * The implementation is through a constant-time Montgomery Ladder.
     */
    field_element mult(const field_element& scalar, const field_element& point);

    /** The base point */
    data_t _basepoint;

    /** The private key */
    data_t _private;

    /** The public key */
    data_t _public;

    /** The shared secret */
    data_t _shared;
};

/**
 * @brief Arithmetic addition in field modulo p
 * @param lhs The augend.
 * @param rhs The addend
 * @return The sum of @c lhs and @c rhs.
 */
x25519::field_element
operator+(const x25519::field_element& lhs, const x25519::field_element& rhs);

/**
 * @brief Arithmetic subtraction in field modulo p
 * @param lhs The minuend.
 * @param rhs The subtrahend
 * @return The different of @c lhs and @c rhs.
 */
x25519::field_element
operator-(const x25519::field_element& lhs, const x25519::field_element& rhs);

/**
 * @brief Arithmetic multiplication in field modulo p
 * @param lhs The multiplicant.
 * @param rhs The multiplier
 * @return The multiplication of @c lhs and @c rhs.
 */
x25519::field_element
operator*(const x25519::field_element& lhs, const x25519::field_element& rhs);

/**
 * @brief Arithmetic inverse in field modulo p
 * @param rhs The value to take the inverse of.
 * @return The multiplication of @c rhs.
 */
x25519::field_element
inverse(const x25519::field_element& rhs);

/**
 * @brief Output the field-element
 * @param ostr The referencer to the stream.
 * @param e The element to print.
 * @return The referencer to the stream.
 */
std::ostream& operator<<(std::ostream& ostr, const x25519::field_element& e);

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_X25519_H_ */
