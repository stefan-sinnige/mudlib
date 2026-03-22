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

#ifndef _MUDLIB_CRYPTO_EXCEPTION_H_
#define _MUDLIB_CRYPTO_EXCEPTION_H_

#include <exception>
#include <string>
#include <mud/crypto/ns.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief Generic cryprographic error.
 * @details
 * An exception type that may be thrown when a cryptographic operation failed.
 * Refer to their derived classes for more detailed exception types.
 */
class crypto_error : public std::exception
{
public:
    /**
     * @brief Default constructor.
     */
    crypto_error() = default;

    /**
     * @brief Construct an exception with an explanatory message.
     * @param msg The explanatory message.
     */
    crypto_error(const std::string& msg) : _what(msg) {}

    /**
     * @brief Destructor.
     */
    virtual ~crypto_error() = default;

    /**
     * @brief Return the explanatory string.
     */
    const char* what() const noexcept override { return _what.c_str(); }

private:
    /** The explanatory string */
    std::string _what;
};

/**
 * @brief Cryprographic data size error.
 * @details
 * An exception type that is thrown when data that is required for a
 * cryptographic algorithm is not of the correct size.
 */
class size_error : public crypto_error
{
public:
    /**
     * @brief Default constructor.
     */
    size_error() = default;

    /**
     * @brief Construct a size exception with an explanatory message.
     * @param msg The explanatory message.
     */
    size_error(const std::string& msg) : crypto_error(msg) {}

    /**
     * @brief Destructor.
     */
    virtual ~size_error() = default;
};

/**
 * @brief Cryprographic padding error.
 * @details
 * An exception type that is thrown when a padding or unpadding operation has
 * failed.
 */
class padding_error : public crypto_error
{
public:
    /**
     * @brief Default constructor.
     */
    padding_error() = default;

    /**
     * @brief Construct a padding exception with an explanatory message.
     * @param msg The explanatory message.
     */
    padding_error(const std::string& msg) : crypto_error(msg) {}

    /**
     * @brief Destructor.
     */
    virtual ~padding_error() = default;
};

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_EXCEPTION_H_ */
