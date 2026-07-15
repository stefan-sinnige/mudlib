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
 *
 * This implementation follows the Keccak Implementation v3.1 and XKCP public
 * domain source code, with the following notable modifications:
 *    - Endian conversion on the state only occurs before and after the round
 *      loop. All operations inside the loop are executed on native uint64_t.
 *    - C++ row-order multi-dimensional arrays are used, while the reference
 *      uses column-order arrays. This means that row and column indices are
 *      swapped.
 */

#include "keccak.h"
#include "mud/core/log.h"
#include "mud/core/endian.h"
#include "mud/crypto/exception.h"
#include <algorithm>
#include <bit>
#include <cstring>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

/**
 * Internal logging is disabled for security reasons and performance. Only
 * enable it for development purposes.
 */
#define KECCAK_LOGGING false
#if KECCAK_LOGGING 
    #warning "CAUTION: KECCAK logging is enabled"
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

/* Keccak Round Constants */
const uint64_t keccak_rc[24] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

static void
TRACE_STATE(const uint64_t S[5][5], bool little_endian = true)
{
#if KECCAK_LOGGING
    LOG(log);
    TRACE(log) << std::hex << std::setfill('0');
    uint8_t* Sb = (uint8_t*)S;
    for (int i = 0; i < 5*5*sizeof(uint64_t); ++ i) {
        if (i % 16 == 0) {
            if ( i != 0) {
                TRACE(log) << std::endl;
            }
            TRACE(log) << "       ";
        }
        TRACE(log) << ' ' << std::setw(2) << (int)Sb[i];
    }
    TRACE(log) << std::endl;
    return;
#endif
}

keccak::keccak(size_t capacity, size_t digest_bit_size, delimited_suffix_t d)
    : _digest_size(digest_bit_size/8)
    , _capacity(capacity/8)
    , _rate(200 - _capacity)
    , _d(d)
{
    /* Initialise the zero state */
    memset(_S, 0, sizeof(_S));

    /* Reset the block */
    memset(_M, 0, sizeof(_M));
    _M_sz = 0;
}

void
keccak::update(const mud::core::buffer& chunk)
{
    update(chunk.view(chunk.cbegin(), chunk.cend()));
}

void
keccak::update(const mud::core::buffer_view& chunk)
{
    /*
     * Keep on adding the data from the chunk to the message block. When the
     * block is full, computate it to make space for any more data.
     */
    auto src = chunk.data();
    auto sz = chunk.size();
    while (sz > 0) {
        /* Fill up (part of) the block */
        size_t available = (_rate - _M_sz);
        size_t copy = (sz > available ? available : sz);
        memcpy(&_M[_M_sz], src, copy);
        sz -= copy;
        src += copy;
        _M_sz += copy;

        /* If the block is full, computate it. */
        if (_M_sz == _rate) {
            computate();
        }
    }
}

void
keccak::close()
{
    /* Add padding to the final block */
    _M[_M_sz] ^= static_cast<uint8_t>(_d);
    if ((static_cast<uint8_t>(_d) & 0x80) != 0 && (_M_sz == (_rate - 1))) {
        computate();
    }
    _M[_rate-1] ^= 0x80;
          
    /* Finalise the hash */
    computate();
    finalise();
}

void
keccak::computate()
{
    LOG(log);

    /* Temporary place holders */
    uint64_t C[5], D, S, T;

    /* Apply the message block to the state */
    uint8_t* Sb = (uint8_t*)&(_S[0][0]);
    for (int i = 0; i < _rate; ++i) {
        Sb[i] ^= _M[i];
    }

    /* Display the state */
    TRACE(log) << "Initial State:" << std::endl;
    TRACE_STATE(_S);

    /* Endian conversion to native */
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            _S[x][y] = endian::convert(endian_t::little, endian_t::native,
                                       _S[x][y]);
        }
    }

    /* Keccak Permutations 1600 bits / 24 rounds */
    for(int r = 0; r < 24; ++r) {
        TRACE(log) << "Round #" << std::dec << r << std::endl;

        /* Theta */
        {
            for (int x = 0; x < 5; ++x) {
                C[x] = _S[0][x] ^ _S[1][x] ^ _S[2][x] ^ _S[3][x] ^ _S[4][x];
            }
            for (int x = 0; x < 5; ++x) {
                D = C[(x + 4) % 5] ^ std::rotl(C[(x + 1) % 5], 1);
                for (int y = 0; y < 5; ++y) {
                    _S[y][x] ^= D;
                }
            }
        }
        // TRACE(log) << "After Theta" << std::endl;
        // TRACE_STATE(_S);

        /* Rho - rotate (x,y) by a position unique offset  */
        {
            int x = 1;
            int y = 0, Y;
            for (int t = 0; t < 24; ++t) {
                _S[y][x] = std::rotl(_S[y][x], ((t + 1)*(t +2 )/2) % 64);
                Y = (2 * x + 3 * y) % 5;
                x = y;
                y = Y;
            }

        }
        // TRACE(log) << "After Rho" << std::endl;
        // TRACE_STATE(_S);

        /* Pi - permutate a source (sx,sy) to a target (tx, ty) location in
         * order, such that the target location becomes the source in the next
         * permutation. Only one temporary value (the previous  target) is to
         * be held. */
        {
            int sx = 1, tx;
            int sy = 0, ty;
            S = _S[sy][sx];
            for (int t = 0; t < 24; ++t) {
                tx = sy;
                ty = (2*sx + 3*sy) % 5;
                T = _S[ty][tx];
                _S[ty][tx] = S;
                S = T;
                sx = tx;
                sy = ty;
            }

        }
        // TRACE(log) << "After Pi" << std::endl;
        // TRACE_STATE(_S);

        /* Chi - Permutate a plane (row) */
        {
            for (int y = 0; y < 5; ++y) {
                /* Copy the original plane */
                for (int x = 0; x < 5; ++x) {
                    C[x] = _S[y][x];
                }

                /* Permutate each lane in the plane */
                for (int x = 0; x < 5; ++x) {
                    _S[y][x] = C[x] ^ ((~C[(x+1)%5] & C[(x+2)%5]));
                }
            }
        }
        // TRACE(log) << "After Chi" << std::endl;
        // TRACE_STATE(_S);

        /* Iota - XOR the (0,0) with a round constant */
        {
            _S[0][0] ^= keccak_rc[r];
        }
        // TRACE(log) << "After Iota" << std::endl;
        // TRACE_STATE(_S);

        /* Display the round state */
        TRACE_STATE(_S, endian_t::native == endian_t::little);
    }

    /* Endian conversion to little */
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            _S[x][y] = endian::convert(endian_t::native, endian_t::little,
                                       _S[x][y]);
        }
    }

    /* Display the state */
    TRACE(log) << "Permutated State:" << std::endl;
    TRACE_STATE(_S, true);

    /* Reset the block */
    memset(_M, 0, sizeof(_M));
    _M_sz = 0;
}

void
keccak::finalise()
{
    _hash = mud::core::buffer(_digest_size);

    /* Squeeze up to the rate size. If more is needed, perform another state
     * permutation. */
    size_t offset = 0;
    while (offset < _digest_size) {
        size_t sz = std::min(_rate, _digest_size - offset);
        memcpy(_hash.data() + offset, _S, sz);
        offset += sz;
        if (offset < _digest_size) {
            computate();
        }
    }
}

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
