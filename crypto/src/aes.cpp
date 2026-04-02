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

#include "mud/core/endian.h"
#include "mud/core/log.h"
#include "mud/crypto/aes.h"
#include "mud/crypto/exception.h"
#include <cstring>
#include <iomanip>
#include <iostream>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

/**
 * Internal logging is disabled for seurity reasons and performance. Only enable
 * it for development purpososes.
 */
#define AES_LOGGING false
#if AES_LOGGING
    #warning "CAUTION: AES Logging is enabled"
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

/**
 * If set to ture, AES uses faster table lookup at the expense of using 512
 * byets of memory to hold two 256 byte arrays.
 */
#define MUL_TABLES true

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief The AES state between transformations.
 * @details
 * The AES state is a two-dimensional array of 4x4 bytes that is initialised
 * from the input text as
 * @f[
 * state[r][c] = input[r + 4c]
 * @f]
 */ 
class basic_aes::state
{
public:
    /**
     * @brief Construct from a sequence of bytes.
     * @param input The sequence of bytes.
     * @note The sequence should contain at least 16 bytes.
     */
    state(const data_t& input);

    /**
     * @brief Copy constructor
     * @param rhs The state to copy from.
     */
    state(const state& rhs);

    /**
     * @brief Move constructor
     * @param rhs The state to move from.
     */
    state(state&& rhs);

    /**
     * @brief Copy assignment
     * @param rhs The state to copy from.
     */
    state& operator=(const state& rhs);

    /**
     * @brief Move assignment
     * @param rhs The state to move from.
     */
    state& operator=(state&& rhs);

    /**
     * @brief Destructor.
     */
    ~state();

    /**
     * @brief Return the value at a byte location
     * @param r The row [0..3]
     * @param c The column [0..3]
     * @return The value at the location.
     */
    inline uint8_t byte(uint8_t r, uint8_t c) const;

    /**
     * @brief Return the value at a byte location
     * @param r The row [0..3]
     * @param c The column [0..3]
     * @return The value at the location.
     */
    inline uint8_t& byte(uint8_t r, uint8_t c);

    /**
     * @brief Return a pointer to a byte location.
     * @param r The row [0..3]
     * @param c The column [0..3]
     * @return The pointer to the location.
     */
    inline uint8_t* const byte_ptr(uint8_t r, uint8_t c) const;

    /**
     * @brief Convert the state to an output sequence.
     */
    operator data_t() const;

private:
    /** The two-dimensional state */
    uint8_t *_b = nullptr;

    /* Friends */
    friend std::ostream& operator<<(std::ostream&, const basic_aes::state& s);
};

basic_aes::state::state(const data_t& input)
{
    _b = new uint8_t[16];
    const uint8_t* data = input.data();
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            byte(r, c) = *(data + r + 4*c);
        }
    }
}

basic_aes::state::~state()
{
    if (_b != nullptr) {
        delete[] _b;
    }
}

basic_aes::state::state(const basic_aes::state& rhs)
{
    operator=(rhs);
}

basic_aes::state::state(basic_aes::state&& rhs)
{
    operator=(rhs);
}

basic_aes::state&
basic_aes::state::operator=(const basic_aes::state& rhs)
{
    if (this != &rhs) {
        if (_b != nullptr) {
            delete[] _b;
            _b = nullptr;
        }
        if (rhs._b != nullptr) {
            _b = new uint8_t[16];
            memcpy(_b, rhs._b, 16);
        }
    }
    return *this;
}

basic_aes::state&
basic_aes::state::operator=(basic_aes::state&& rhs)
{
    if (this != &rhs) {
        if (_b != nullptr) {
            delete[] _b;
        }
        _b = rhs._b;
        rhs._b = nullptr;
    }
    return *this;
}

uint8_t
basic_aes::state::byte(uint8_t r, uint8_t c) const
{
    return _b[4*r+c];
}

uint8_t&
basic_aes::state::byte(uint8_t r, uint8_t c)
{
    return _b[4*r+c];
}

uint8_t* const
basic_aes::state::byte_ptr(uint8_t r, uint8_t c) const
{
    return &(_b[4*r+c]);
}

basic_aes::state::operator data_t() const
{
    data_t output(16);
    uint8_t* data = output.data();
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            *(data + r + 4*c) = byte(r, c);
        }
    }
    return output;
}

std::ostream&
operator<<(std::ostream& ostr, const basic_aes::state& s)
{
    auto saved_flags = ostr.flags();
    ostr << std::hex << std::setfill('0');
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            ostr << std::setw(2) << (int)s.byte(r, c) << ' ';
        }
        if (r != 3) {
            ostr << std::endl;
        }
    }
    ostr.flags(saved_flags);
    return ostr;
}

/**
 * @brief AES Round Constants
 */
const uint32_t rcon[10] = {
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x01000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x02000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x04000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x08000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x10000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x20000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x40000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x80000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x1b000000),
    endian::convert(endian::native(), endian_t::big, (uint32_t)0x36000000)
};

/**
 * @brief AES Substitution Box
 */
const uint8_t sbox[256] = {
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

/**
 * @brief AES Inverted Substitution Box
 */
const uint8_t inv_sbox[256] = {
0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

basic_aes::basic_aes(size_t key_size)
    : _key_size(key_size)
    , _Nb(4)
    , _Nk(key_size/32)
    , _Nr(_Nk+6)
{
    LOG(log);
    TRACE(log) << "Key-size : " << _key_size << std::endl
               << "     _Nb : " << _Nb << std::endl
               << "     _Nk : " << _Nk << std::endl
               << "     _Nr : " << _Nr << std::endl;
}

void
basic_aes::encrypt(const data_t& input, data_t& output, const key_t& key)
{
    LOG(log);
    std::vector<uint32_t> w = key_expansion(key);
    uint32_t* round_keys = w.data();
    basic_aes::state state(input);
    state = add_round_key(state, round_keys);
    for (size_t round = 1; round <= _Nr-1; ++round) {
        TRACE(log) << "-------------- Round " << std::dec << round << std::endl;
        state = sub_bytes(state);
        state = shift_rows(state);
        state = mix_columns(state);
        state = add_round_key(state, round_keys + 4*round);
    }
    TRACE(log) << "-------------- Final" << std::dec << std::endl;
    state = sub_bytes(state);
    state = shift_rows(state);
    state = add_round_key(state, round_keys + 4*_Nr);
    output = state;
}

void
basic_aes::decrypt(const data_t& input, data_t& output, const key_t& key)
{
    LOG(log);
    TRACE(log) << "Decipher()" << std::endl;
    std::vector<uint32_t> w = key_expansion(key);
    uint32_t* round_keys = w.data();
    basic_aes::state state(input);
    state = add_round_key(state, round_keys + 4*_Nr);
    for (size_t round = _Nr-1; round >= 1; --round) {
        TRACE(log) << "-------------- Round " << std::dec << round << std::endl;
        state = inv_shift_rows(state);
        state = inv_sub_bytes(state);
        state = add_round_key(state, round_keys + 4*round);
        state = inv_mix_columns(state);
    }
    state = inv_shift_rows(state);
    state = inv_sub_bytes(state);
    state = add_round_key(state, round_keys);
    output = state;
}

std::vector<uint32_t>
basic_aes::key_expansion(const key_t& key)
{
    if (key.size()*8 != _key_size) {
        throw size_error("size of key is not equal to AES key-size");
    }
    std::vector<uint32_t> w(4*(_Nr+1));
    uint8_t i = 0;
    while (i <= _Nk - 1) {
        w[i] = *((uint32_t*)(key.data() + 4*i));
        ++i;
    }
    while (i <= (4*_Nr + 3)) {
        uint32_t temp = w[i-1];
        if (i % _Nk == 0) {
            temp = sub_word(rot_word(temp)) ^ rcon[(i/_Nk) - 1];
        }
        else
        if (_Nk > 6 && i % _Nk == 4) {
            temp = sub_word(temp);
        }
        w[i] = w[i-_Nk] ^ temp;
        ++i;
    }

    LOG(log);
    TRACE(log) << "Round keys:\n";
    for (int i = 0; i < w.size(); ++i) {
        TRACE(log) << std::dec << std::setfill(' ') << std::setw(2) << i
                   << ": ";
        uint8_t* data = (uint8_t*)&(w[i]);
        TRACE(log) << std::hex << std::setfill('0');
        for (int i= 0; i < 4; ++i) {
            TRACE(log) << std::setw(2) << (int)data[i];
        }
        TRACE(log) << std::endl;
    }
    return w;
}

uint32_t
basic_aes::rot_word(uint32_t w)
{
    uint8_t* b = (uint8_t*)&w;
    uint8_t t = b[0];
    b[0] = b[1];
    b[1] = b[2];
    b[2] = b[3];
    b[3] = t;
    return *((uint32_t*)b);
}

uint32_t
basic_aes::sub_word(uint32_t w)
{
    uint8_t* wb = (uint8_t*)&w;
    uint8_t a[4] = {
        sbox[wb[0]],
        sbox[wb[1]],
        sbox[wb[2]],
        sbox[wb[3]],
    };
    return *((uint32_t*)a);
}

basic_aes::state&
basic_aes::add_round_key(basic_aes::state& state, uint32_t* w)
{
    for (uint8_t c = 0; c < 4; ++c) {
        uint8_t* wb = (uint8_t*)(w + c);
        for (uint8_t r = 0; r < 4; ++r) {
            state.byte(r, c) = state.byte(r, c) ^ wb[r];
        }
    }

    LOG(log);
    TRACE(log) << "After AddRoundKey\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::sub_bytes(basic_aes::state& state)
{
    for (uint8_t r = 0; r < 4; ++r) {
        for (uint8_t c = 0; c < 4; ++c) {
            state.byte(r, c) = sbox[state.byte(r, c)];
        }
    }

    LOG(log);
    TRACE(log) << "After SubBytes\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::shift_rows(basic_aes::state& state)
{
    uint8_t t;

    // First row unchanged

    // Second row
    t = state.byte(1, 0);
    state.byte(1, 0) = state.byte(1, 1);
    state.byte(1, 1) = state.byte(1, 2);
    state.byte(1, 2) = state.byte(1, 3);
    state.byte(1, 3) = t;

    // Third row
    t = state.byte(2, 0);
    state.byte(2, 0) = state.byte(2, 2);
    state.byte(2, 2) = t;
    t = state.byte(2, 1);
    state.byte(2, 1) = state.byte(2, 3);
    state.byte(2, 3) = t;

    // Fourth row
    t = state.byte(3, 3);
    state.byte(3, 3) = state.byte(3, 2);
    state.byte(3, 2) = state.byte(3, 1);
    state.byte(3, 1) = state.byte(3, 0);
    state.byte(3, 0) = t;

    LOG(log);
    TRACE(log) << "After ShiftRows\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::mix_columns(basic_aes::state& state)
{
    for (uint8_t c = 0; c < 4; ++c) {
        uint8_t s[4] = {
            state.byte(0, c),
            state.byte(1, c),
            state.byte(2, c),
            state.byte(3, c)
        };
        state.byte(0, c) = gfmul(2, s[0]) ^
                           gfmul(3, s[1]) ^
                           s[2] ^
                           s[3];
        state.byte(1, c) = s[0] ^
                           gfmul(2, s[1]) ^
                           gfmul(3, s[2]) ^
                           s[3];
        state.byte(2, c) = s[0] ^
                           s[1] ^
                           gfmul(2, s[2]) ^
                           gfmul(3, s[3]);
        state.byte(3, c) = gfmul( 3, s[0]) ^
                           s[1] ^
                           s[2] ^
                           gfmul(2, s[3]);
    }

    LOG(log);
    TRACE(log) << "After MixColumns\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::inv_sub_bytes(basic_aes::state& state)
{
    for (uint8_t r = 0; r < 4; ++r) {
        for (uint8_t c = 0; c < 4; ++c) {
            state.byte(r, c) = inv_sbox[state.byte(r, c)];
        }
    }

    LOG(log);
    TRACE(log) << "After InvSubBytes\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::inv_shift_rows(basic_aes::state& state)
{
    uint8_t t;

    // First row unchanged

    // Second row
    t = state.byte(1, 3);
    state.byte(1, 3) = state.byte(1, 2);
    state.byte(1, 2) = state.byte(1, 1);
    state.byte(1, 1) = state.byte(1, 0);
    state.byte(1, 0) = t;

    // Third row
    t = state.byte(2, 3);
    state.byte(2, 3) = state.byte(2, 1);
    state.byte(2, 1) = t;
    t = state.byte(2, 2);
    state.byte(2, 2) = state.byte(2, 0);
    state.byte(2, 0) = t;

    // Fourth row
    t = state.byte(3, 0);
    state.byte(3, 0) = state.byte(3, 1);
    state.byte(3, 1) = state.byte(3, 2);
    state.byte(3, 2) = state.byte(3, 3);
    state.byte(3, 3) = t;

    LOG(log);
    TRACE(log) << "After InvShiftRows\n" << state << std::endl;
    return state;
}

basic_aes::state&
basic_aes::inv_mix_columns(basic_aes::state& state)
{
    for (uint8_t c = 0; c < 4; ++c) {
        uint8_t s[4] = {
            state.byte(0, c),
            state.byte(1, c),
            state.byte(2, c),
            state.byte(3, c)
        };
        state.byte(0, c) = gfmul(14, s[0]) ^
                           gfmul(11, s[1]) ^
                           gfmul(13, s[2]) ^
                           gfmul( 9, s[3]);
        state.byte(1, c) = gfmul( 9, s[0]) ^
                           gfmul(14, s[1]) ^
                           gfmul(11, s[2]) ^
                           gfmul(13, s[3]);
        state.byte(2, c) = gfmul(13, s[0]) ^
                           gfmul( 9, s[1]) ^
                           gfmul(14, s[2]) ^
                           gfmul(11, s[3]);
        state.byte(3, c) = gfmul(11, s[0]) ^
                           gfmul(13, s[1]) ^
                           gfmul( 9, s[2]) ^
                           gfmul(14, s[3]);
    }

    LOG(log);
    TRACE(log) << "After InvMixColumns\n" << state << std::endl;
    return state;
}

#if MUL_TABLES

/* Implementation of GF(2^8) multiplication using log and anti-log tables (in
 * this case generator 3). This allows for quick table lookups at the cost
 * of using more memory. */

const uint8_t log[256] = {
0x00,0x00,0x19,0x01,0x32,0x02,0x1a,0xc6,0x4b,0xc7,0x1b,0x68,0x33,0xee,0xdf,0x03,
0x64,0x04,0xe0,0x0e,0x34,0x8d,0x81,0xef,0x4c,0x71,0x08,0xc8,0xf8,0x69,0x1c,0xc1,
0x7d,0xc2,0x1d,0xb5,0xf9,0xb9,0x27,0x6a,0x4d,0xe4,0xa6,0x72,0x9a,0xc9,0x09,0x78,
0x65,0x2f,0x8a,0x05,0x21,0x0f,0xe1,0x24,0x12,0xf0,0x82,0x45,0x35,0x93,0xda,0x8e,
0x96,0x8f,0xdb,0xbd,0x36,0xd0,0xce,0x94,0x13,0x5c,0xd2,0xf1,0x40,0x46,0x83,0x38,
0x66,0xdd,0xfd,0x30,0xbf,0x06,0x8b,0x62,0xb3,0x25,0xe2,0x98,0x22,0x88,0x91,0x10,
0x7e,0x6e,0x48,0xc3,0xa3,0xb6,0x1e,0x42,0x3a,0x6b,0x28,0x54,0xfa,0x85,0x3d,0xba,
0x2b,0x79,0x0a,0x15,0x9b,0x9f,0x5e,0xca,0x4e,0xd4,0xac,0xe5,0xf3,0x73,0xa7,0x57,
0xaf,0x58,0xa8,0x50,0xf4,0xea,0xd6,0x74,0x4f,0xae,0xe9,0xd5,0xe7,0xe6,0xad,0xe8,
0x2c,0xd7,0x75,0x7a,0xeb,0x16,0x0b,0xf5,0x59,0xcb,0x5f,0xb0,0x9c,0xa9,0x51,0xa0,
0x7f,0x0c,0xf6,0x6f,0x17,0xc4,0x49,0xec,0xd8,0x43,0x1f,0x2d,0xa4,0x76,0x7b,0xb7,
0xcc,0xbb,0x3e,0x5a,0xfb,0x60,0xb1,0x86,0x3b,0x52,0xa1,0x6c,0xaa,0x55,0x29,0x9d,
0x97,0xb2,0x87,0x90,0x61,0xbe,0xdc,0xfc,0xbc,0x95,0xcf,0xcd,0x37,0x3f,0x5b,0xd1,
0x53,0x39,0x84,0x3c,0x41,0xa2,0x6d,0x47,0x14,0x2a,0x9e,0x5d,0x56,0xf2,0xd3,0xab,
0x44,0x11,0x92,0xd9,0x23,0x20,0x2e,0x89,0xb4,0x7c,0xb8,0x26,0x77,0x99,0xe3,0xa5,
0x67,0x4a,0xed,0xde,0xc5,0x31,0xfe,0x18,0x0d,0x63,0x8c,0x80,0xc0,0xf7,0x70,0x07
};

const uint8_t alog[256] = {
0x01,0x03,0x05,0x0f,0x11,0x33,0x55,0xff,0x1a,0x2e,0x72,0x96,0xa1,0xf8,0x13,0x35,
0x5f,0xe1,0x38,0x48,0xd8,0x73,0x95,0xa4,0xf7,0x02,0x06,0x0a,0x1e,0x22,0x66,0xaa,
0xe5,0x34,0x5c,0xe4,0x37,0x59,0xeb,0x26,0x6a,0xbe,0xd9,0x70,0x90,0xab,0xe6,0x31,
0x53,0xf5,0x04,0x0c,0x14,0x3c,0x44,0xcc,0x4f,0xd1,0x68,0xb8,0xd3,0x6e,0xb2,0xcd,
0x4c,0xd4,0x67,0xa9,0xe0,0x3b,0x4d,0xd7,0x62,0xa6,0xf1,0x08,0x18,0x28,0x78,0x88,
0x83,0x9e,0xb9,0xd0,0x6b,0xbd,0xdc,0x7f,0x81,0x98,0xb3,0xce,0x49,0xdb,0x76,0x9a,
0xb5,0xc4,0x57,0xf9,0x10,0x30,0x50,0xf0,0x0b,0x1d,0x27,0x69,0xbb,0xd6,0x61,0xa3,
0xfe,0x19,0x2b,0x7d,0x87,0x92,0xad,0xec,0x2f,0x71,0x93,0xae,0xe9,0x20,0x60,0xa0,
0xfb,0x16,0x3a,0x4e,0xd2,0x6d,0xb7,0xc2,0x5d,0xe7,0x32,0x56,0xfa,0x15,0x3f,0x41,
0xc3,0x5e,0xe2,0x3d,0x47,0xc9,0x40,0xc0,0x5b,0xed,0x2c,0x74,0x9c,0xbf,0xda,0x75,
0x9f,0xba,0xd5,0x64,0xac,0xef,0x2a,0x7e,0x82,0x9d,0xbc,0xdf,0x7a,0x8e,0x89,0x80,
0x9b,0xb6,0xc1,0x58,0xe8,0x23,0x65,0xaf,0xea,0x25,0x6f,0xb1,0xc8,0x43,0xc5,0x54,
0xfc,0x1f,0x21,0x63,0xa5,0xf4,0x07,0x09,0x1b,0x2d,0x77,0x99,0xb0,0xcb,0x46,0xca,
0x45,0xcf,0x4a,0xde,0x79,0x8b,0x86,0x91,0xa8,0xe3,0x3e,0x42,0xc6,0x51,0xf3,0x0e,
0x12,0x36,0x5a,0xee,0x29,0x7b,0x8d,0x8c,0x8f,0x8a,0x85,0x94,0xa7,0xf2,0x0d,0x17,
0x39,0x4b,0xdd,0x7c,0x84,0x97,0xa2,0xfd,0x1c,0x24,0x6c,0xb4,0xc7,0x52,0xf6,0x01
};

uint8_t
basic_aes::gfmul(uint8_t x, uint8_t y)
{
    if (x != 0 && y != 0) {
        return alog[(log[x] + log[y])%255];
    }
    return 0;
}

#else

/* Implementation of GF(2^8) multiplication using the Russian Peasant algorithm
 * for multiplication, with a modulo reduction of 0x1B when the value needs to
 * be reduced (the bit shifted out was set). In this finite field addition and
 * subtraction are XOR operations.
 * This allows for low memory usage but at the cost of more computation. */

uint8_t
basic_aes::gfmul(uint8_t x, uint8_t y)
{
    uint8_t m = 0;
    uint8_t hi_bit_set;
    for(uint8_t cnt = 0; cnt < 8; cnt++) {
        if((y & 1) == 1) {
            m ^= x;
        }
        hi_bit_set = (x & 0x80);
        x <<= 1;
        if(hi_bit_set == 0x80) {
            x ^= 0x1b;
        }
        y >>= 1;
    }
    return m;
}

#endif /* MUL_TABLES */

/* Block-cipher factory registrations */

block_cipher_factory::registrar<AES_128_ECB>
        _aes_128_ecb_registrar("AES-128-ECB");
block_cipher_factory::registrar<AES_192_ECB>
        _aes_192_ecb_registrar("AES-192-ECB");
block_cipher_factory::registrar<AES_256_ECB>
        _aes_256_ecb_registrar("AES-256-ECB");

block_cipher_factory::registrar<AES_128_CBC>
        _aes_128_cbc_registrar("AES-128-CBC");
block_cipher_factory::registrar<AES_192_CBC>
        _aes_192_cbc_registrar("AES-192-CBC");
block_cipher_factory::registrar<AES_256_CBC>
        _aes_256_cbc_registrar("AES-256-CBC");

block_cipher_factory::registrar<AES_128_CFB>
        _aes_128_cfb_registrar("AES-128-CFB");
block_cipher_factory::registrar<AES_192_CFB>
        _aes_192_cfb_registrar("AES-192-CFB");
block_cipher_factory::registrar<AES_256_CFB>
        _aes_256_cfb_registrar("AES-256-CFB");

block_cipher_factory::registrar<AES_128_CTR>
        _aes_128_ctr_registrar("AES-128-CTR");
block_cipher_factory::registrar<AES_192_CTR>
        _aes_192_ctr_registrar("AES-192-CTR");
block_cipher_factory::registrar<AES_256_CTR>
        _aes_256_ctr_registrar("AES-256-CTR");

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
