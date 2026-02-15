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

#ifndef _MUDLIB_HTML_RAWDATA_H_
#define _MUDLIB_HTML_RAWDATA_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML unescapable raw data
 */
class MUDLIB_HTML_API raw_data : public node
{
public:
    /**
     * @brief Create an empty raw data..
     */
    raw_data();

    /**
     * @brief Create raw data..
     * @param[in] data The raw data.
     */
    raw_data(const std::string& data);

    /**
     * @brief Copy a raw data.
     * @param[in] rhs The raw data to copy from.
     */
    raw_data(const raw_data& rhs);

    /**
     * @brief Copy a raw data through assignment.
     * @param[in] rhs The raw data to copy from.
     * @return A reference to this raw data.
     */
    raw_data& operator=(const raw_data& rhs);

    /**
     * @brief Move a raw data.
     * @param[in] rhs The raw data to move from. After moving, it will
     * resemble an empty raw data.
     */
    raw_data(raw_data&& rhs);

    /**
     * @brief Move a raw data through assignment.
     * @param[in] rhs The raw data to move from. After moving, it will
     * resemble an empty raw data.
     * @return A reference to this raw data.
     */
    raw_data& operator=(raw_data&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~raw_data();

    /**
     * @brief Return the contents of the raw data.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the raw data.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /** The raw data contents */
    std::string _text;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_RAWDATA_H_ */
