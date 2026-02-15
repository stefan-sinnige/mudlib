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

#ifndef _MUDLIB_XML_CHARDATA_H_
#define _MUDLIB_XML_CHARDATA_H_

#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML character data
 */
class MUDLIB_XML_API char_data : public node
{
public:
    /**
     * @brief Type definition of a @c char_data pointer.
     */
    typedef std::shared_ptr<mud::xml::char_data> ptr;

    /**
     * @brief Create a @c char_data instance with text.
     * @param text The text to set.
     */
    static ptr create(const std::string& text);

    /**
     * @brief Move a character data.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     */
    char_data(char_data&& rhs) = default;

    /**
     * @brief Move a character data through assignment.
     * @param[in] rhs The character data to move from. After moving, it will
     * resemble an empty character data.
     * @return A reference to this character data.
     */
    char_data& operator=(char_data&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~char_data() = default;

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
    /**
     * @brief Create a character data with text.
     */
    char_data(const std::string& text);

    /** The character data contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CHARDATA_H_ */
