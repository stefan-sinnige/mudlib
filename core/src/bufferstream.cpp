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

#include "mud/core/bufferstream.h"

BEGIN_MUDLIB_CORE_NS

/* ======================================================================
 * Stream buffer
 * ====================================================================== */

buffer_streambuf::buffer_streambuf()
{
    buffer b;
    buf(std::move(b));
}

buffer_streambuf::buffer_streambuf(const buffer& b)
{
    buf(b);
}

buffer_streambuf::buffer_streambuf(buffer&& b)
{
    buf(std::move(b));
}

int
buffer_streambuf::underflow()
{
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }
    return traits_type::eof();
}

int
buffer_streambuf::overflow(int c)
{
    if (c != traits_type::eof()) {
        /* Save the current and end location of the get area. */
        auto cur_diff = gptr() - (char*)_buf.data();
        auto end_diff = egptr() - (char*)_buf.data();

        /* Resize and reset the pointers */
        _buf.resize(_buf.size() + 64);
        setg((char*)_buf.data(),
             (char*)_buf.data() + cur_diff,
             (char*)_buf.data() + _buf.size());
        setp((char*)_buf.data() + end_diff,
             (char*)_buf.data() + _buf.size());
        *pptr() = c;
        pbump(1);
    }
    return c;
}

buffer
buffer_streambuf::buf() const
{
    return buffer((const uint8_t*)_buf.data(), pptr() - (char*)_buf.data());
}

void
buffer_streambuf::buf(const buffer& b)
{
    /* Copy the buffer */
    _buf = b;

    /* Set the get area pointer */
    setg((char*)_buf.data(),
         (char*)_buf.data(),
         (char*)_buf.data() + _buf.size());

    /* Set the put area pointer */
    setp((char*)_buf.data() + _buf.size(),
         (char*)_buf.data() + _buf.size());
}

void
buffer_streambuf::buf(buffer&& b)
{
    /* Move the buffer */
    _buf = std::move(b);

    /* Set the get area pointer */
    setg((char*)_buf.data(),
         (char*)_buf.data(),
         (char*)_buf.data() + _buf.size());

    /* Set the put area pointer */
    setp((char*)_buf.data() + _buf.size(),
         (char*)_buf.data() + _buf.size());
}

/* ======================================================================
 * Input stream buffer
 * ====================================================================== */

ibufferstream::ibufferstream()
    : std::basic_istream<char, std::char_traits<char>>(nullptr)
{
    _sbuf = std::make_unique<buffer_streambuf>();
    rdbuf(_sbuf.get());
}

ibufferstream::ibufferstream(const buffer& buf)
    : std::basic_istream<char, std::char_traits<char>>(nullptr)
{
    _sbuf = std::make_unique<buffer_streambuf>(buf);
    rdbuf(_sbuf.get());
}

ibufferstream::ibufferstream(buffer&& buf)
    : std::basic_istream<char, std::char_traits<char>>(nullptr)
{
    _sbuf = std::make_unique<buffer_streambuf>(std::move(buf));
    rdbuf(_sbuf.get());
}

ibufferstream::ibufferstream(ibufferstream&& rhs)
    : std::basic_istream<char, std::char_traits<char>>(std::move(rhs))
{
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
}

ibufferstream&
ibufferstream::operator=(ibufferstream&& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
    return *this;
}

buffer
ibufferstream::buf() const
{
    return _sbuf->buf();
}

void
ibufferstream::buf(const buffer& b)
{
    _sbuf->buf(b);
}

void
ibufferstream::buf(buffer&& b)
{
    _sbuf->buf(std::move(b));
}

/* ======================================================================
 * Output stream buffer
 * ====================================================================== */

obufferstream::obufferstream()
    : std::basic_ostream<char, std::char_traits<char>>(nullptr)
{
    _sbuf = std::make_unique<buffer_streambuf>();
    rdbuf(_sbuf.get());
}

obufferstream::obufferstream(obufferstream&& rhs)
    : std::basic_ostream<char, std::char_traits<char>>(std::move(rhs))
{
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
}

obufferstream&
obufferstream::operator=(obufferstream&& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
    return *this;
}

buffer
obufferstream::buf() const
{
    return _sbuf->buf();
}

void
obufferstream::buf(const buffer& b)
{
    _sbuf->buf(b);
}

void
obufferstream::buf(buffer&& b)
{
    _sbuf->buf(std::move(b));
}

/* ======================================================================
 * Input and output stream buffer
 * ====================================================================== */

bufferstream::bufferstream()
    : std::basic_iostream<char, std::char_traits<char>>(nullptr)
{
    _sbuf = std::make_unique<buffer_streambuf>();
    rdbuf(_sbuf.get());
}

bufferstream::bufferstream(bufferstream&& rhs)
    : std::basic_iostream<char, std::char_traits<char>>(std::move(rhs))
{
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
}

bufferstream&
bufferstream::operator=(bufferstream&& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    _sbuf = std::move(rhs._sbuf);
    rdbuf(_sbuf.get());
    return *this;
}

buffer
bufferstream::buf() const
{
    return _sbuf->buf();
}

void
bufferstream::buf(const buffer& b)
{
    _sbuf->buf(b);
}

void
bufferstream::buf(buffer&& b)
{
    _sbuf->buf(std::move(b));
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
