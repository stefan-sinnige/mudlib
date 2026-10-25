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

#ifndef _MUDLIB_CORE_ENDIAN_H_
#define _MUDLIB_CORE_ENDIAN_H_

#include <mud/core/ns.h>
#include <stdint.h>
#include <concepts>
#include <stdexcept>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Endian operations.
 *
 * Conversion betweem different endian systems (little and big).
 */
class MUDLIB_CORE_API endian
{
public:
    /**
     * The supported endian systems.
     * */
    enum class endian_t
    {
        native, /**< Native endian, effectively either little or big */
        little, /**< Little-endian */
        big     /**< Big-endian */
    };

    /**
     * @brief Return the native endian system of the host system.
     * @details
     * Return either @c endian_t::big or @c endian_t::little depending on the
     * resolved system endian system. It shall never return @c endian_t::native.
     */
    static endian_t native();

    /**
     * @brief Convert an integral value between two endian systems.
     * @param from The endian system to convert from. The @c value is expected
     * to be represented in this system.
     * @param to The endian system to convert to.
     * @param value The value represented in the @c From endian system to
     * convert.
     * @return The @c value represented in the @c To endian system.
     */
    template<std::integral Integer>
    static Integer convert(endian_t from, endian_t to, Integer value)
    {
        switch (sizeof(Integer))
        {
            case 1:
                return value;
            case 2:
                return static_cast<Integer>(
                    _convert(from, to, static_cast<uint16_t>(value)));
            case 4:
                return static_cast<Integer>(
                    _convert(from, to, static_cast<uint32_t>(value)));
            case 8:
                return static_cast<Integer>(
                    _convert(from, to, static_cast<uint64_t>(value)));
            default:
                throw std::length_error(
                        "unsupported integral length endian conversion");
        }
    }

private:
    /**
     * @brief Endian conversion for 16-bit integers.
     */
    static uint16_t _convert(endian_t From, endian_t To, uint16_t value);

    /**
     * @brief Endian conversion for 32-bit integers.
     */
    static uint32_t _convert(endian_t From, endian_t To, uint32_t value);

    /**
     * @brief Endian conversion for 64-bit integers.
     */
    static uint64_t _convert(endian_t From, endian_t To, uint64_t value);
};

/**
 * @brief Compare the relationship between two endian systems.
 * @param lhs The first endian system to compare.
 * @param rhs The second endian system to compare.
 * @return True if the relation-ship has met.
 */
bool operator==(endian::endian_t lhs, endian::endian_t rhs);

/**
 * @brief Compare if two endian systems are not equal.
 * @param lhs The first endian system to compare.
 * @param rhs The second endian system to compare.
 * @return True if the two endian systems are not equal.
 */
bool operator!=(endian::endian_t lhs, endian::endian_t rhs);

//
END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_ENDIAN_H_ */
