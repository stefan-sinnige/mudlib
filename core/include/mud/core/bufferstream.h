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

#ifndef _MUDLIB_CORE_BUFFER_STREAM_H_
#define _MUDLIB_CORE_BUFFER_STREAM_H_

#include <mud/core/ns.h>
#include <mud/core/buffer.h>
#include <iostream>
#include <memory>
#include <streambuf>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The stream buffer providing raw access to the buffer streams.
 */
class MUDLIB_CORE_API buffer_streambuf
    : public std::basic_streambuf<char, std::char_traits<char>>
{
public:
    /**
     * @brief Create a stream-buffer with an empty buffer.
     */
    buffer_streambuf();

    /**
     * @brief Create the stream-buffer with a copy a buffer.
     * @param b The buffer to copy.
     */
    buffer_streambuf(const buffer& b);

    /**
     * @brief Create the stream-buffer with a buffer.
     * @param b The buffer to move.
     */
    buffer_streambuf(buffer&& b);

    /**
     * @brief Destructor.
     */
    ~buffer_streambuf() = default;

    /**
     * @brief Return a copy of the current buffer.
     */
    buffer buf() const;

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(const buffer& b);

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(buffer&& b);

protected:
    /**
     * @brief Reading characters from the associated input sequence while
     * decrypting.
     * @return The value pointed to by the @c gptr or @c traits::eof() if no
     * more data is available.
     */
    int underflow() override;

    /**
     * @brief Writing characters to the associated input sequence while
     * encrypting.
     * @param ch The character to store.
     * @return The value @c ch or @c traits::eof() on error.
     */
    int overflow(int c) override;

private:
    /* The buffer */
    buffer _buf;
};

/**
 * @brief Provide access to a buffer through streaming input operations.
 */
class MUDLIB_CORE_API ibufferstream
    : public std::basic_istream<char, std::char_traits<char>>
{
public:
    /**
     * @brief Create an empty buffer stream
     */
    ibufferstream();

    /**
     * @brief Create a buffer stream with initial buffer contents.
     * @param buf The buffer contents to copy.
     * @details
     * The buffer @c buf contents will be copied.
     */
    ibufferstream(const buffer& buf);

    /**
     * @brief Create a buffer stream with initial buffer contents.
     * @param buf The buffer contents to move.
     * @details
     * The buffer @c buf contents will be moved and the @c buf will be cleared.
     */
    ibufferstream(buffer&& buf);

    /**
     * @brief Copy constructor.
     */
    ibufferstream(const ibufferstream&) = delete;

    /**
     * @brief Move constructor.
     * @param rhs The object to move.
     */
    ibufferstream(ibufferstream&& rhs);

    /**
     * @brief Destruct the buffer stream.
     */
    ~ibufferstream() = default;

    /**
     * @brief Copy assignment.
     */
    ibufferstream& operator=(const ibufferstream&) = delete;

    /**
     * @brief Move assignment.
     * @param rhs The object to move.
     * @return Reference to this stream.
     */
    ibufferstream& operator=(ibufferstream&& rhs);

    /**
     * @brief Return a copy of the current buffer.
     */
    buffer buf() const;

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(const buffer& b);

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(buffer&& b);

private:
    /** The buffer */
    std::unique_ptr<buffer_streambuf> _sbuf;
};

/**
 * @brief Provide access to a buffer through streaming output operations.
 */
class MUDLIB_CORE_API obufferstream
    : public std::basic_ostream<char, std::char_traits<char>>
{
public:
    /**
     * @brief Create an empty buffer stream
     */
    obufferstream();

    /**
     * @brief Copy constructor.
     */
    obufferstream(const obufferstream&) = delete;

    /**
     * @brief Move constructor.
     * @param rhs The object to move.
     */
    obufferstream(obufferstream&& rhs);

    /**
     * @brief Destruct the buffer stream.
     */
    ~obufferstream() = default;

    /**
     * @brief Copy assignment.
     */
    obufferstream& operator=(const obufferstream&) = delete;

    /**
     * @brief Move assignment.
     * @param rhs The object to move.
     * @return Reference to this stream.
     */
    obufferstream& operator=(obufferstream&& rhs);

    /**
     * @brief Return a copy of the current buffer.
     */
    buffer buf() const;

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(const buffer& b);

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(buffer&& b);

private:
    /** The buffer */
    std::unique_ptr<buffer_streambuf> _sbuf;
};

/**
 * @brief Provide access to a buffer through streaming input/output operations.
 */
class MUDLIB_CORE_API bufferstream
    : public std::basic_iostream<char, std::char_traits<char>>
{
public:
    /**
     * @brief Create an empty buffer stream
     */
    bufferstream();

    /**
     * @brief Create a buffer stream with initial buffer contents.
     * @param buf The buffer contents to copy.
     * @details
     * The buffer @c buf contents will be copied.
     */
    bufferstream(const buffer& buf);

    /**
     * @brief Create a buffer stream with initial buffer contents.
     * @param buf The buffer contents to move.
     * @details
     * The buffer @c buf contents will be moved and the @c buf will be cleared.
     */
    bufferstream(buffer&& buf);

    /**
     * @brief Copy constructor.
     */
    bufferstream(const bufferstream&) = delete;

    /**
     * @brief Move constructor.
     * @param rhs The object to move.
     */
    bufferstream(bufferstream&& rhs);

    /**
     * @brief Destruct the buffer stream.
     */
    ~bufferstream() = default;

    /**
     * @brief Copy assignment.
     */
    bufferstream& operator=(const bufferstream&) = delete;

    /**
     * @brief Move assignment.
     * @param rhs The object to move.
     * @return Reference to this stream.
     */
    bufferstream& operator=(bufferstream&&);

    /**
     * @brief Return a copy of the current buffer.
     */
    buffer buf() const;

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(const buffer& b);

    /**
     * @brief Set a new buffer.
     * @param b The buffer to set.
     * @details
     * The reading position is the start of the buffer, the writing position is
     * the end of the buffer.
     */
    void buf(buffer&& b);

private:
    /** The buffer */
    std::unique_ptr<buffer_streambuf> _sbuf;
};

END_MUDLIB_CORE_NS

#endif /*  _MUDLIB_CORE_BUFFER_STREAM_H_ */
