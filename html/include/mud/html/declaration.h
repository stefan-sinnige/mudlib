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

#ifndef _MUDLIB_HTML_DECLARATION_H_
#define _MUDLIB_HTML_DECLARATION_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML declaration
 */
class MUDLIB_HTML_API declaration : public node
{
public:
    /**
     * @brief Create an empty declaration..
     */
    declaration();

    /**
     * @brief Copy a declaration.
     * @param[in] rhs The declaration to copy from.
     */
    declaration(const declaration& rhs);

    /**
     * @brief Copy a declaration through assignment.
     * @param[in] rhs The declaration to copy from.
     * @return A reference to this declaration.
     */
    declaration& operator=(const declaration& rhs);

    /**
     * @brief Move a declaration.
     * @param[in] rhs The declaration to move from. After moving,
     * it will resemble an empty declaration.
     */
    declaration(declaration&& rhs);

    /**
     * @brief Move a declaration through assignment.
     * @param[in] rhs The declaration to move from. After moving,
     * it will resemble an empty declaration.
     * @return A reference to this declaration.
     */
    declaration& operator=(declaration&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~declaration();

    /**
     * @brief Return the version of the declaration.
     */
    const std::string& version() const;

    /**
     * @brief Set the version of the declaration
     * @param[in] value The value to set.
     */
    void version(const std::string& value);
    void version(std::string&& value);

    /**
     * @brief Return the encoding of the HTML document.
     */
    const std::string& encoding() const;

    /**
     * @brief Set the encoding of the HTML document
     * @param[in] value The value to set.
     */
    void encoding(const std::string& value);
    void encoding(std::string&& value);

    /**
     * @brief Return the standalone status.
     */
    bool standalone() const;

    /**
     * @brief Set the standalone status.
     * @param[in] value The value to set.
     */
    void standalone(bool value);

private:
    /** The declaration version */
    std::string _version;

    /** The encoding */
    std::string _encoding;

    /** The standalone declaration */
    bool _standalone;

    /** The declaration data */
    std::string _data;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_DECLARATION_H_ */
