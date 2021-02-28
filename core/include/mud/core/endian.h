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
    enum class endian_t {
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
