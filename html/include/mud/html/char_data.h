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

#ifndef _MUDLIB_HTML_CHARDATA_H_
#define _MUDLIB_HTML_CHARDATA_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML escapable character data. The text stored
 * in this object is in its unescaped form.
 */
class MUDLIB_HTML_API char_data : public node
{
public:
    /**
     * @brief Create an empty character data..
     */
    char_data();

    /**
     * @brief Create character data..
     * @param[in] data The character data.
     */
    char_data(const std::string& data);

    /**
     * @brief Copy a character data.
     * @param[in] rhs The character data to copy from.
     */
    char_data(const char_data& rhs);

    /**
     * @brief Copy a character data through assignment.
     * @param[in] rhs The character data to copy from.
     * @return A reference to this character data.
     */
    char_data& operator=(const char_data& rhs);

    /**
     * @brief Move a character data.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     */
    char_data(char_data&& rhs);

    /**
     * @brief Move a character data through assignment.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     * @return A reference to this character data.
     */
    char_data& operator=(char_data&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~char_data();

    /**
     * @brief Return the contents of the character data.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the character data.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /** The character data contents */
    std::string _text;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_CHARDATA_H_ */
