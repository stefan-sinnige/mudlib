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
        unresolved, /**< Unresolved native endian */
        little,     /**< Little-endian */
        big         /**< Big-endian */
    };

    /**
     * @brief Return the native endian system of the host system.
     */
    static endian_t native();

    /**
     * @brief Templated endian conversion for integers.
     */
    template<endian_t From, endian_t To, typename Integer>
    Integer convert(Integer);

    /**
     * @brief Endian conversion for 16-bit integers.
     */
    static uint16_t convert(endian_t From, endian_t To, uint16_t value);

    /**
     * @brief Endian conversion for 32-bit integers.
     */
    static uint32_t convert(endian_t From, endian_t To, uint32_t value);

    /**
     * @brief Endian conversion for 64-bit integers.
     */
    static uint64_t convert(endian_t From, endian_t To, uint64_t value);
};

template<endian::endian_t From, endian::endian_t To, typename Integer>
Integer
endian::convert(Integer value)
{
    return endian::convert(From, To, value);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_ENDIAN_H_ */
