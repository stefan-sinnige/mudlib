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

#ifndef _MUDLIB_CORE_HEXDUMP_H_
#define _MUDLIB_CORE_HEXDUMP_H_

#include <mud/core/ns.h>
#include <cstdint>
#include <iostream>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Binary buffer output.
 * @details
 * Outputs a binary buffer in hexadecimal and character form to an output
 * stream. The output if of the form of
 * @code
 *   OOOO HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH CCCCCCCCCCCCCCCC
 * @endcode
 * where @c OOOO is the offset in hexadecimal form from the start of the buffer,
 * @c HH is the hexadecimal form of one byte and @c C is the printable character
 * form of the byte, or a dot if there is no printable form.
 *
 * The buffer can be supplied in different forms and the output is only through
 * a @c std::ostream . The output does not start or end with an new-line
 * character.
 *
 * An example of an output of a character buffer:
 * @code
 * const char* output = "Lorem ipsum dolor proin.";
 * std::cout << hexdump(output, strlen(output)) << std::endl
 * @endcode
 * will produce
 * @code
 * 0000: 4c6f 7265 6d20 6970 7375 6d20 646f 6c6f  Lorem ipsum dolo
 * 0010: 7220 7072 6f69 6e2e                      r proin.
 * @endcode
 */
class MUDLIB_CORE_API hexdump
{
public:
    /**
     * @brief Constructor to dump a character array.
     * @param data The pointer to the character array.
     * @param size The size of the character array
     */
    hexdump(const char* data, size_t len);

    /**
     * @brief Constructor to dump a character array.
     * @param data The pointer to the character array.
     * @param size The size of the character array
     */
    hexdump(const uint8_t* data, size_t len);

private:
    /* The pointer to the data */
    const uint8_t* _data;

    /* The size of the data */
    size_t _size;

    /* Friend */
    friend std::ostream& operator<<(std::ostream&, const mud::core::hexdump&);
};

std::ostream&
operator<<(std::ostream& ostr, const hexdump& value);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_HEXDUMP_H_ */
