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

#ifndef _MUDLIB_HTML_CDATASECTION_H_
#define _MUDLIB_HTML_CDATASECTION_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML CDATA section
 */
class MUDLIB_HTML_API cdata_section : public node
{
public:
    /**
     * @brief Create an empty CDATA section.
     */
    cdata_section();

    /**
     * @brief Create an CDATA section.
     * @param[in] data The character data.
     */
    cdata_section(const std::string& data);

    /**
     * @brief Copy a CDATA section.
     * @param[in] rhs The CDATA section to copy from.
     */
    cdata_section(const cdata_section& rhs);

    /**
     * @brief Copy a CDATA section through assignment.
     * @param[in] rhs The CDATA section to copy from.
     * @return A reference to this CDATA section.
     */
    cdata_section& operator=(const cdata_section& rhs);

    /**
     * @brief Move a CDATA section.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     */
    cdata_section(cdata_section&& rhs);

    /**
     * @brief Move a CDATA section through assignment.
     * @param[in] rhs The CDATA section to move from. After moving, it will
     * resemble an empty CDATA section.
     * @return A reference to this CDATA section.
     */
    cdata_section& operator=(cdata_section&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~cdata_section();

    /**
     * @brief Return the contents of the CDATA section.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the CDATA section.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /** The CDATA section contents */
    std::string _text;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_CDATASECTION_H_ */
