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

#ifndef _MUDLIB_HTTP_CHUNK_H_
#define _MUDLIB_HTTP_CHUNK_H_

#include <iostream>
#include <mud/http/ns.h>
#include <string>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief The definition of a chunk.
 *    RFC cw9112: Section 7.1
 */
class MUDLIB_HTTP_API chunk
{
public:
    /**
     * @brief Construct an empty chunk message.
     */
    chunk() = default;

    /**
     * @brief Construct a chunk message with data.
     */
    chunk(const std::string& data) : _data(data) {}

    /**
     * @brief Copy a chunk message.
     * @param[in] rhs  The chunk to copy from.
     */
    chunk(const chunk& rhs) = default;

    /**
     * @brief Move a chunk message.
     * @param[in] rhs  The chunk to move from.
     */
    chunk(chunk&& rhs) = default;

    /**
     * Destructor.
     */
    virtual ~chunk() = default;

    /**
     * Copy assign a chunk,
     * @param[in] rhs The chunk to copy.
     * @return Reference to this chunk.
     */
    chunk& operator=(const chunk& rhs) = default;

    /**
     * Move assign a chunk,
     * @param[in] rhs The chunk to move.
     * @return Reference to this chunk.
     */
    chunk& operator=(chunk&& rhs) = default;

    /**
     * The chunk data.
     */
    const std::string& data() const { return _data; }

    /**
     * The chunk size.
     */
    size_t size() const { return _data.size(); }

private:
    /** The chunk data. */
    std::string _data;
};

/** Read an HTTP chunk from an input stream. */
std::istream&
operator>>(std::istream&, chunk&);

/** Write an HTTP chunk to an output stream. */
std::ostream&
operator<<(std::ostream&, const chunk&);

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_CHUNK_H_ */
