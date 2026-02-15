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

#ifndef _MUDLIB_IO_STREAMBUF_H_
#define _MUDLIB_IO_STREAMBUF_H_

#if defined(_WIN32)
    #ifndef MUDLIB_SSIZE_T
        #define MUDLIB_SSIZE_T
        typedef long ssize_t;
    #endif
#endif
#include <memory>
#include <mud/core/handle.h>
#include <mud/io/ns.h>
#include <streambuf>

BEGIN_MUDLIB_IO_NS

/**
 * @brief Stream buffer for generic I/O operations.
 *
 * This class defines the stream buffer for generic (but system implementation
 * dependent) I/O operations.
 */
class MUDLIB_IO_API basic_streambuf : public std::streambuf
{
public:
    /**
     * Constructor, passing a handle to use.
     * @param [in] handle  The handle to use.
     * @param [in] bufsize The initial buffer size.
     * @param [in] putbacksize The size of the putback buffer.
     */
    basic_streambuf(std::shared_ptr<mud::core::handle> handle,
                    size_t bufsize = 10, size_t putbacksize = 4);

    /**
     * Destructor.
     */
    virtual ~basic_streambuf();

    /**
     * Read new characters into the buffer.
     */
    virtual int underflow();

    /**
     * Write the characters from the buffer.
     */
    virtual int overflow(int c);

    /**
     * Synchronise data.
     */
    virtual int sync();

    /**
     * Non-copyable.
     */
    basic_streambuf(const basic_streambuf&) = delete;
    basic_streambuf& operator=(const basic_streambuf&) = delete;

protected:
    /**
     * Low-level buffer reading and writing. This can be device specific.
     */
    virtual ssize_t read(void* buffer, size_t count) = 0;
    virtual ssize_t write(const void* buffer, size_t count) = 0;

    /**
     * Return to the resource handle.
     */
    std::shared_ptr<mud::core::handle> handle();

private:
    std::shared_ptr<mud::core::handle> _handle; /**< Handle */
    const size_t _bufsize;     /**< Size of the buffer */
    const size_t _putbacksize; /**< Size of the putback buffer */
    char* _buffer;             /**< The buffer. */
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_STREAMBUF_H_ */
