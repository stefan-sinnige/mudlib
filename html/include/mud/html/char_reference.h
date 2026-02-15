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

#ifndef _MUDLIB_HTML_CHAR_REFERENCE_H_
#define _MUDLIB_HTML_CHAR_REFERENCE_H_

#include <mud/html/ns.h>
#include <string>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Utility class to escape or unescape a string.
 */
class MUDLIB_HTML_API char_reference
{
public:
    /**
     * @brief Escape a string by replacing specific characters with its
     * escaped character reference representation.
     * @param[in] unescaped The unescaped string.
     * @return The escaped form.
     */
    static std::string escape(const std::string& unescaped);

    /**
     * @brief Unescape a string by replacing specific character references with
     * its unescaped reference representation.
     * @param[in] escaped The unescaped string.
     * @return The unescaped form.
     * @throw mud::html::exception when the escaped string contains unknown
     * character references.
     */
    static std::string unescape(const std::string& escaped);

    /**
     * Not constructible.
     */
    char_reference() = delete;
    char_reference(const char_reference&) = delete;
    char_reference& operator=(const char_reference&) = delete;

    /**
     * Not moveable.
     */
    char_reference(char_reference&&) = delete;
    char_reference& operator=(char_reference&&) = delete;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_CHAR_REFERENCE_H_ */
