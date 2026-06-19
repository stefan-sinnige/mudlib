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

#include "mud/core/log.h"
#include "mud/crypto/hash.h"
#include "mud/crypto/exception.h"

/**
 * Internal logging is disabled for security reasons and performance. Only
 * enable it for development purposes.
 */
#define HASH_LOGGING false
#if HASH_LOGGING 
    #warning "CAUTION: hash logging is enabled"
#else
    #undef LOG
    #undef TRACE
    #undef DEBUG
    #undef INFO
    #undef WARN
    #undef ERROR
    #undef FATAL
    #undef TYPEINFO
    #define LOG(obj)
    #define TRACE(obj)  if (true) {} else std::cout
    #define DEBUG(obj)  if (true) {} else std::cout
    #define INFO(obj)   if (true) {} else std::cout
    #define WARN(obj)   if (true) {} else std::cout
    #define ERROR(obj)  if (true) {} else std::cout
    #define FATAL(obj)  if (true) {} else std::cout
    #define TYPEINFO(t) ""
#endif

BEGIN_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * The basic hash.
 * ========================================================================== */

basic_hash::basic_hash(const hash_material_t& hashing)
    : _hashing(hashing)
{
}

basic_hash::~basic_hash()
{
    delete _sbuf;
}

hash_streambuf*
basic_hash::sbuf(std::streambuf* chain)
{
    if (_sbuf == nullptr) {
        _sbuf = new hash_streambuf(chain, *this);
    }
    return _sbuf;
}

/* ==========================================================================
 * The hash stream buffer.
 * ========================================================================== */

hash_streambuf::hash_streambuf(
        std::streambuf* chain,
        basic_hash& hash)
    : _chain(chain)
    , _hash(hash)
    , _bufsize(256)
    , _putbacksize(4)
    , _eof(false)
{
    /* Allocate the data structure */
    _buffer = new char[_bufsize + _putbacksize];

    /* Set the get area pointers for reading */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize);

    /* Set the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
}

hash_streambuf::~hash_streambuf()
{
    delete[] _buffer;
}

void
hash_streambuf::close()
{
    _eof = true;
    sync();
    _hash.close();
}

int
hash_streambuf::underflow()
{
    /* Is there still data ready in the buffer. */
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    /* Read new block of data from the chain */
    int nr = _chain->sgetn(_buffer + _putbacksize, _bufsize);
    if (nr <= 0) {
        return traits_type::eof();
    }

    /* Update the hash */
    _hash.update(mud::core::buffer((uint8_t*)_buffer + _putbacksize, nr));

    /* Check if there is more deta */
    _eof = _chain->sgetc() == traits_type::eof();

    /* Reset the get buffer pointers with the number of characters read. */
    setg(_buffer + _putbacksize, _buffer + _putbacksize,
         _buffer + _putbacksize + nr);

    /* Check if there is no more data (a full block of padding was unpad) */
    if (gptr() == egptr()) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*gptr());
}

int
hash_streambuf::overflow(int c)
{
    /* Insert the character into the buffer */
    if (c != traits_type::eof()) {
        *pptr() = c;
        pbump(1);
    }

    /* Hash the block */
    if (sync() != 0) {
        return traits_type::eof();
    }
    return c;
}

int
hash_streambuf::sync()
{
    /* Update the hash */
    _hash.update(mud::core::buffer((uint8_t*)pbase(), pptr() - pbase()));

    /* Write the block of data to the chain */
    int nr = _chain->sputn(pbase(), pptr() - pbase());
    if (nr < (pptr() - pbase())) {
        return -1;
    }

    /* Reset the put area pointers for writing */
    setp(_buffer, _buffer + (_bufsize - 1));
    return 0;
}

END_MUDLIB_CRYPTO_NS

/* ==========================================================================
 * Hash Factory
 * ========================================================================== */

template<>
mud::crypto::hash_factory&
mud::crypto::hash_factory::instance()
{
    static mud::crypto::hash_factory _instance;
    return _instance;
}

/* vi: set ai ts=4 expandtab: */
