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
#include "mud/core/endian.h"
#include "mud/crypto/sha.h"
#include "mud/crypto/exception.h"
#include "keccak.h"
#include <bit>
#include <cstring>

using mud::core::endian;
using endian_t = mud::core::endian::endian_t;

/**
 * Internal logging is disabled for security reasons and performance. Only
 * enable it for development purposes.
 */
#define SHA_LOGGING false
#if SHA_LOGGING 
    #warning "CAUTION: SHA logging is enabled"
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
 * SHA algorithm implementation base
 * ========================================================================== */

class sha_algorithm::impl
{
public:
    /**
     * @brief Constructor
     */
    impl() = default;

    /**
     * @brief Destructor.
     */
    virtual ~impl() = default;

    /**
     * @brief Return the hash specifications.
     */
    virtual std::string specs() const = 0;

    /**
     * @brief Update the hash with a chunk of binary input data.
     */
    virtual void update(const mud::core::buffer_view& chunk) = 0;

    /**
     * @brief Close or finalise the hash.
     */
    virtual void close() = 0;

    /**
     * @brief Return the calculated hash value.
     */
    virtual const mud::core::buffer& hash() const = 0;

    /**
     * @brief Return the hash size (in bytes).
     */
    virtual size_t hash_size() const = 0;

    /**
     * @brief Return the block size (in bytes).
     */
    virtual size_t block_size() const = 0;
};

void
sha_algorithm::impl_deleter::operator()(sha_algorithm::impl* ptr) const
{
    delete ptr;
}

/* ==========================================================================
 * SHA-1 algorithm
 * ========================================================================== */

/*
 * SHA-1 Initial Hash Values
 */

const uint32_t sha_1_iv[5] = {
    0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0
};

/*
 * SHA-1 constants
 */
const uint32_t sha_1_k[4] = {
    0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6
};

class sha_algorithm::sha_1_impl: public sha_algorithm::impl
{
public:
    /**
     * The word size (in bytes) is the size of a word. The message block is
     * treated as an array of (big-endian) words of this size.
     */
    static constexpr size_t word_sz = 4;

    /**
     * The block size (in bytes) in the size of a message block over which the
     * hash-value is calculated on.
     */
    static constexpr size_t block_sz = 64;

    /**
     * The length size (in bytes) is the size of the total message length (in
     * bits) that is at the end of the final padded block.
     */
    static constexpr size_t length_sz = 8;

    /**
     * The digest size (in bytes).
     */
    static constexpr size_t digest_sz = 20;

    /**
     * @brief Constructor.
     */
    sha_1_impl();

    /**
     * @brief Return the hash specifications.
     */
    std::string specs() const override;

    /**
     * @brief Update the hash with a chunk of binary input data.
     */
    void update(const mud::core::buffer_view& chunk) override;

    /**
     * @brief Close or finalise the hash.
     */
    void close() override;

    /**
     * @brief Return the calculated hash value.
     */
    const mud::core::buffer& hash() const override { return _hash; }

    /**
     * @brief Return the hash size (in bytes).
     */
    size_t hash_size() const override { return digest_sz; }

    /**
     * @brief Return the block size (in bytes).
     */
    size_t block_size() const override { return block_sz; }

private:
    /**
     * Padding state
     */
    enum class pad_state_t {
        none,       /* No padding applied yet */
        start,      /* Start bit applied, but no length */
        length      /* Start bit and length applied */
    };

    /**
     * @brief Pad the message block.
     * @return @c true if padding has been applied.
     * @note Padding is only applied on the final block.
     */
    bool pad();

    /**
     * @brief Computate the hash on the pre-processed fixed sized message block.
     */
    void computate();

    /**
     * The SHA-1 functions
     */
    static uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t Parity(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

    /**
     * @brief Finalise the hash value.
     */
    void finalise();

    /* The calculated hash */
    mud::core::buffer _hash;

    /* The specification */
    const char* _specs;

    /* Constants */
    const uint32_t* _K;

    /* Current Hash Value */
    uint32_t _H[5];

    /* The message block */
    uint8_t _M[64];

    /* The number of bytes available in the message block */
    size_t _M_sz = 0;

    /* Total message size in bits */
    uint64_t _l = 0;

    /* The pad state */
    pad_state_t _pad_state = pad_state_t::none;
};

sha_algorithm::sha_1_impl::sha_1_impl()
{
    _specs = "SHA-1";
    _K = sha_1_k;
    memcpy((void*)_H, (const void*)sha_1_iv, sizeof(sha_1_iv));
}

std::string
sha_algorithm::sha_1_impl::specs() const
{
    return _specs;
}

void
sha_algorithm::sha_1_impl::update(const mud::core::buffer_view& chunk)
{
    /*
     * Keep on adding the data from the chunk to the message block. When the
     * block is full, computate it to make space for any more data.
     */
    auto src = chunk.data();
    auto sz = chunk.size();
    while (sz > 0) {
        /* Fill up (part of) the block */
        size_t available = (block_sz - _M_sz);
        size_t copy = (sz > available ? available : sz);
        memcpy(&_M[_M_sz], src, copy);
        sz -= copy;
        src += copy;
        _M_sz += copy;

        /* If the block is full, computate it. */
        if (_M_sz == block_sz) {
            computate();
        }
    }

    /* Update the total message length */
    _l += chunk.size() * 8;
}

void
sha_algorithm::sha_1_impl::close()
{
    /* Ensure that the last block is always a padded one. */
    bool padded = false;
    do
    {
        padded = pad();
        computate();
    } while (!padded);

    /* Finalise the hash */
    finalise();
}

bool
sha_algorithm::sha_1_impl::pad()
{
    /* We need at least 64-bits to store the message length, and one bit (as
     * we're byte-based this is actually one byte) to store the '1' padding bit.     * If there is no space left, we need an additional block.
     * We can assume that there is always one byte left (as enforced by the
     * 'update' function that always computates full message blocks) so we can
     * always add the '1' bit (or 0x80 as we're byte based). */

    if (_pad_state == pad_state_t::none) {
        /* Add a '1' bit in the free space and fill it up with '0' */
        _M[_M_sz++] = 0x80;
        _pad_state = pad_state_t::start;
    }
    if (_pad_state == pad_state_t::start) {
        /* Fill the message block up with zero's. */
        memset(&_M[_M_sz], 0, block_sz - _M_sz);
    } 
    if (_M_sz > (block_sz - length_sz)) {
        /* No space left to hold the length. We need an additional block. */
        _M_sz = block_sz;
        return false;
    }
    else {
        /* Add the 64-bit message length in big-endian at the end */
        _pad_state = pad_state_t::length;
        *(uint64_t*)(&_M[block_sz - 8]) = endian::convert(
                endian_t::native, endian_t::big, _l);
        _M_sz = block_sz;
        return true;
    }
}

void
sha_algorithm::sha_1_impl::finalise()
{
    /* Convert the hash values to big-endian */
    uint32_t H[5];
    for (int i = 0; i < 5; ++i) {
        H[i] = endian::convert(endian_t::native, endian_t::big, _H[i]);
    }

    /* As the H array is correctly byte-oriented, we can just do a byte copy */
    _hash = mud::core::buffer(digest_sz);
    memcpy(_hash.data(), H, digest_sz);
}

void
sha_algorithm::sha_1_impl::computate()
{
    /* Expect only a full message block */
    if (_M_sz != block_sz) {
        throw mud::crypto::size_error("unfulfilled SHA message-block");
    }

    /* Display the initial hash value for the first block */
    LOG(log);
    if (_l <= block_sz) {
        TRACE(log) << "Initial hash value:" << std::endl;
        for (int i = 0; i < 5; ++i) {
            TRACE(log) << "  H[" << std::dec << i << "] = " << std::hex
                       << std::setfill('0') << std::setw(8)
                       << _H[i] << std::endl;
        }
    }
  
    /* Prepare the message schedule, converting from big to native endian */
    int t;
    uint32_t W[17];
    for (t = 0; t <= 15; ++t) {
        W[t] = endian::convert(endian_t::big, endian_t::native,
                               *(uint32_t*)(&_M[t*4]));
    }

    /* Display the block contents */
    TRACE(log) << "Block Contents:" << std::endl;
    for (int i = 0; i < 16; ++i) {
        TRACE(log) << "  W["
                   << std::dec << std::setfill(' ') << std::setw(2) << i
                   << "] = " << std::hex
                   << std::setfill('0') << std::setw(8)
                   << W[i] << std::endl;
    }

    /* Initialise the working variables */
    uint32_t a, b, c, d, e;
    a = _H[0];
    b = _H[1];
    c = _H[2];
    d = _H[3];
    e = _H[4];

    /* Eighty computational rounds */
    uint32_t K_t, W_t, T;
    std::function<uint32_t(uint32_t, uint32_t, uint32_t)> f;
    for (t = 0; t <= 79; ++t) {
        /* Get the f and K[t] value for this round */
        if (t <= 19) {
            K_t = _K[0];
            f = Ch;
        }
        else
        if (t <= 39) {
            K_t = _K[1];
            f = Parity;
        }
        else
        if (t <= 59) {
            K_t = _K[2];
            f = Maj;
        }
        else {
            K_t = _K[3];
            f = Parity;
        }

        /* Get the W[t] value for this round. */
        if (t <= 15) {
            W_t = W[t];
        }
        else {
            W[16] = std::rotl(W[16-3] ^ W[16-8] ^ W[16-14] ^ W[16-16], 1);
            W_t = W[16];
            for (int i = 0; i <= 15; ++i) {
                W[i] = W[i+1];
            }
        }

        /* Computation */
        T = std::rotl(a, 5) + f(b, c, d) + e + K_t + W_t;
        e = d;
        d = c;
        c = std::rotl(b, 30);
        b = a;
        a = T;

        /* Log the round result */
        if (t == 0) {
            TRACE(log) << "          A        B        C        D        E"
                       << std::endl;
        }
        TRACE(log) << "t=" << std::dec << std::setfill(' ') << std::setw(2) 
                   << t << ": "
                   << std::hex << std::setfill('0')
                   << std::setw(8) << a << " "
                   << std::setw(8) << b << " "
                   << std::setw(8) << c << " "
                   << std::setw(8) << d << " "
                   << std::setw(8) << e
                   << std::endl;
    }

    /* Computate the intermediate hash */
    _H[0] += a;
    _H[1] += b;
    _H[2] += c;
    _H[3] += d;
    _H[4] += e;

    TRACE(log) << "Hash value:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        TRACE(log) << "H[" << std::dec << i << "] = " << std::hex
                   << std::setfill('0') << std::setw(8)
                   << _H[i] << std::endl;
    }

    /* Reset the block for the next message */
    _M_sz = 0;
}

uint32_t
sha_algorithm::sha_1_impl::Ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

uint32_t
sha_algorithm::sha_1_impl::Parity(uint32_t x, uint32_t y, uint32_t z)
{
    return x ^ y ^ z;
}

uint32_t
sha_algorithm::sha_1_impl::Maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

/* ==========================================================================
 * SHA-2 algorithm
 * Two different implementations: for 32- and 64-bit word sizes.
 * ========================================================================== */

/*
 * SHA-2 Initial Hash Values
 */

const uint32_t sha_224_iv[8] = {
    0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
    0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};
const uint32_t sha_256_iv[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
const uint64_t sha_384_iv[8] = {
    0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17,
    0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511,
    0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};
const uint64_t sha_512_iv[8] = {
    0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b,
    0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f,
    0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};
const uint64_t sha_512_224_iv[8] = {
    0x8c3d37c819544da2, 0x73e1996689dcd4d6, 0x1dfab7ae32ff9c82,
    0x679dd514582f9fcf, 0x0f6d2b697bd44da8, 0x77e36f7304c48942,
    0x3f9d85a86a1d36c8, 0x1112e6ad91d692a1
};
const uint64_t sha_512_256_iv[8] = {
    0x22312194fc2bf72c, 0x9f555fa3c84c64c2, 0x2393b86b6f53b151,
    0x963877195940eabd, 0x96283ee2a88effe3, 0xbe5e1e2553863992,
    0x2b0199fc2c85b8aa, 0x0eb72ddc81c52ca2
};

/*
 * SHA-2 constants
 * These are the first 32 or 64 bits of the fractional parts of the cube roots
 * of the first 64 or 80 prime numbers.
 */

const uint32_t sha_32_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
const uint64_t sha_64_k[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
    0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
    0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
    0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
    0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
    0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
    0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
    0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
    0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
    0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
    0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
    0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
    0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
    0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
    0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
    0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
    0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
    0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
    0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
    0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};

/**
 * @brief The SHA-2 implementation
 * @tparam WordType The word-type, either @c uint32_t or @c uint64_t.
 * @details
 * SHA-2 implements a few algorithms that operate on two different word sizes:
 * 32- and 64-bits.
 */
template<typename WordType>
class sha_algorithm::sha_2_impl: public sha_algorithm::impl
{
public:
    /**
     * The word type is the unsigned type that holds a word.
     */
    typedef WordType word_type_t;

    /**
     * The word size (in bytes) is the size of a word. The message block is
     * treated as an array of (big-endian) words of this size.
     */
    static constexpr size_t word_sz = sizeof(word_type_t);

    /**
     * The block size (in bytes) in the size of a message block over which the
     * hash-value is calculated on.
     */
    static constexpr size_t block_sz = word_sz*16;

    /**
     * The length size (in bytes) is the size of the total message length (in
     * bits) that is at the end of the final padded block.
     */
    static constexpr size_t length_sz = word_sz*2;

    /**
     * @brief Constructor
     * @param digest_bit_size The size of the digest (in bits).
     */
    sha_2_impl(size_t digest_bit_size);

    /**
     * @brief Destructor
     */
    ~sha_2_impl();

    /**
     * @brief Return the hash specifications.
     */
    std::string specs() const override;

    /**
     * @brief Update the hash with a chunk of binary input data.
     */
    void update(const mud::core::buffer_view& chunk) override;

    /**
     * @brief Close or finalise the hash.
     */
    void close() override;

    /**
     * @brief Return the calculated hash value.
     */
    const mud::core::buffer& hash() const override { return _hash; }

    /**
     * @brief Return the hash size (in bytes).
     */
    size_t hash_size() const override { return _digest_size; }

    /**
     * @brief Return the block size (in bytes).
     */
    size_t block_size() const override { return block_sz; }

private:
    /**
     * Padding state
     */
    enum class pad_state_t {
        none,       /* No padding applied yet */
        start,      /* Start bit applied, but no length */
        length      /* Start bit and length applied */
    };

    /**
     * @brief Pad the message block.
     * @return @c true if padding has been applied.
     * @note Padding is only applied on the final block.
     */
    bool pad();

    /**
     * @brief Computate the hash on the pre-processed fixed sized message block.
     */
    void computate();

    /**
     * @brief Finalise the hash value.
     */
    void finalise();

    /**
     * The SHA-2 functions
     */
    word_type_t sigma_0(word_type_t x);
    word_type_t sigma_1(word_type_t x);
    word_type_t Sigma_0(word_type_t x);
    word_type_t Sigma_1(word_type_t x);
    word_type_t Ch(word_type_t x, word_type_t y, word_type_t z);
    word_type_t Maj(word_type_t x, word_type_t y, word_type_t z);

    /* The specification */
    const char* _specs;

    /* Number of rounds */
    size_t _nr_rounds;

    /* The hash */
    mud::core::buffer _hash;

    /* The digest size (in bytes) */
    size_t _digest_size;

    /* Current Hash Value */
    word_type_t _H[8];

    /* Constants */
    const word_type_t* _K;

    /* The message block */
    uint8_t *_M = nullptr;

    /* The number of bytes available in the message block */
    size_t _M_sz = 0;

    /* Total message size in bits */
    struct { 
        uint64_t high;
        uint64_t low;
    } _l = {0, 0};

    /* The pad state */
    pad_state_t _pad_state = pad_state_t::none;
};

template<>
sha_algorithm::sha_2_impl<uint32_t>::sha_2_impl(size_t digest_bit_size)
    : _digest_size(digest_bit_size / 8)
{
    _M = new uint8_t[block_sz];
    if (digest_bit_size == 224) {
        _specs = "SHA-224";
        _K = sha_32_k;
        memcpy((void*)_H, (const void*)sha_224_iv, sizeof(sha_224_iv));
    }
    else
    if (digest_bit_size == 256) {
        _specs = "SHA-256";
        _K = sha_32_k;
        memcpy((void*)_H, (const void*)sha_256_iv, sizeof(sha_256_iv));
    }
    _nr_rounds = 64;
}

template<>
sha_algorithm::sha_2_impl<uint64_t>::sha_2_impl(size_t digest_bit_size)
    : _digest_size(digest_bit_size / 8)
{
    _M = new uint8_t[block_sz];
    if (digest_bit_size == 224) {
        _specs = "SHA-512/224";
        _K = sha_64_k;
        memcpy((void*)_H, (const void*)sha_512_224_iv,
               sizeof(sha_512_224_iv));
    }
    else
    if (digest_bit_size == 256) {
        _specs = "SHA-512/256";
        _K = sha_64_k;
        memcpy((void*)_H, (const void*)sha_512_256_iv,
               sizeof(sha_512_256_iv));
    }
    else
    if (digest_bit_size == 384) {
        _specs = "SHA-384";
        _K = sha_64_k;
        memcpy((void*)_H, (const void*)sha_384_iv, sizeof(sha_384_iv));
    }
    else
    if (digest_bit_size == 512) {
        _specs = "SHA-512";
        _K = sha_64_k;
        memcpy((void*)_H, (const void*)sha_512_iv, sizeof(sha_512_iv));
    }
    _nr_rounds = 80;
}

template<typename WordType>
sha_algorithm::sha_2_impl<WordType>::~sha_2_impl()
{
    delete[] _M;
}

template<typename WordType>
std::string
sha_algorithm::sha_2_impl<WordType>::specs() const
{
    return _specs;
}

template<typename WordType>
void
sha_algorithm::sha_2_impl<WordType>::update(const mud::core::buffer_view& chunk)
{
    /*
     * Keep on adding the data from the chunk to the message block. When the
     * block is full, computate it to make space for any more data.
     */
    auto src = chunk.data();
    auto sz = chunk.size();
    while (sz > 0) {
        /* Fill up (part of) the block */
        size_t available = (block_sz - _M_sz);
        size_t copy = (sz > available ? available : sz);
        memcpy(&_M[_M_sz], src, copy);
        sz -= copy;
        src += copy;
        _M_sz += copy;

        /* If the block is full, computate it. */
        if (_M_sz == block_sz) {
            computate();
        }
    }

    /* Update the total message length */
    uint64_t low = _l.low;
    _l.low += chunk.size() * 8;
    if (_l.low < low) {
        _l.high++;
    }
}

template<typename WordType>
void
sha_algorithm::sha_2_impl<WordType>::close()
{
    /* Ensure that the last block is always a padded one. */
    bool padded = false;
    do
    {
        padded = pad();
        computate();
    } while (!padded);

    /* Finalise the hash */
    finalise();
}

template<typename WordType>
bool
sha_algorithm::sha_2_impl<WordType>::pad()
{
    /* We need at least word_size to store the message length, and one bit (as
     * we're byte-based this is actually one byte) to store the '1' padding bit.     * If there is no space left, we need an additional block.
     * We can assume that there is always one byte left (as enforced by the
     * 'update' function that always computates full message blocks) so we can
     * always add the '1' bit (or 0x80 as we're byte based). */

    if (_pad_state == pad_state_t::none) {
        /* Add a '1' bit in the free space and fill it up with '0' */
        _M[_M_sz++] = 0x80;
        _pad_state = pad_state_t::start;
    }
    if (_pad_state == pad_state_t::start) {
        /* Fill the message block up with zero's. */
        memset(&_M[_M_sz], 0, block_sz - _M_sz);
    } 
    if (_M_sz > (block_sz - length_sz)) {
        /* No space left to hold the length. We need an additional block. */
        _M_sz = block_sz;
        return false;
    }
    else {
        /* Add the message length in big-endian at the end */
        _pad_state = pad_state_t::length;
        if (length_sz == 16) {
            *(uint64_t*)(&_M[block_sz - 16]) = endian::convert(
                    endian_t::native, endian_t::big, _l.high);
        }
        *(uint64_t*)(&_M[block_sz - 8]) = endian::convert(
                endian_t::native, endian_t::big, _l.low);
        _M_sz = block_sz;
        return true;
    }
}
template<typename WordType>
void
sha_algorithm::sha_2_impl<WordType>::computate()
{
    /* Expect only a full message block */
    if (_M_sz != block_sz) {
        throw mud::crypto::size_error("unfulfilled SHA message-block");
    }

    /* Display the initial hash value for the first block */
    LOG(log);
    if (_l.high == 0 && _l.low <= block_sz) {
        TRACE(log) << "Initial hash value:" << std::endl;
        for (int i = 0; i < sizeof(_H)/word_sz; ++i) {
            TRACE(log) << "  H[" << std::dec << i << "] = " << std::hex
                       << std::setfill('0') << std::setw(word_sz*2)
                       << _H[i] << std::endl;
        }
    }

    /* Prepare the message schedule, converting from big to native endian */
    int t;
    word_type_t W[17];
    for (t = 0; t <= 15; ++t) {
        W[t] = endian::convert(endian_t::big, endian_t::native,
                               *(word_type_t*)(&_M[t*word_sz]));
    }

    /* Display the block contents */
    TRACE(log) << "Block Contents:" << std::endl;
    for (int i = 0; i < 16; ++i) {
        TRACE(log) << "  W["
                   << std::dec << std::setfill(' ') << std::setw(2) << i
                   << "] = " << std::hex
                   << std::setfill('0') << std::setw(word_sz*2)
                   << W[i] << std::endl;
    }

    /* Initialise the working variables */
    word_type_t a, b, c, d, e, f, g, h;
    a = _H[0];
    b = _H[1];
    c = _H[2];
    d = _H[3];
    e = _H[4];
    f = _H[5];
    g = _H[6];
    h = _H[7];
    word_type_t K_t, W_t, T_1, T_2;
    for (t = 0; t < _nr_rounds; ++t) {
        /* Get the K[t] value for this round */
        K_t = _K[t];

        /* Get the W[t] value for this round. */
        if (t <= 15) {
            W_t = W[t];
        }
        else {
            W[16] = sigma_1(W[16-2])  + W[16-7] +
                    sigma_0(W[16-15]) + W[16-16];
            W_t = W[16];
            for (int i = 0; i <= 15; ++i) {
                W[i] = W[i+1];
            }
        }

        /* Computation */
        T_1 = h + Sigma_1(e) + Ch(e, f, g) + K_t + W_t;
        T_2 = Sigma_0(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T_1;
        d = c;
        c = b;
        b = a;
        a = T_1 + T_2;

        /* Log the result */
        if (word_sz == 4) {
            if (t == 0) {
                TRACE(log) << "          A        B        C        D"
                           << "        E        F        G        H"
                           << std::endl;
            }
            TRACE(log) << "t=" << std::dec << std::setfill(' ') << std::setw(2)
                       << t << ": "
                       << std::hex << std::setfill('0')
                       << std::setw(8) << a << " "
                       << std::setw(8) << b << " "
                       << std::setw(8) << c << " "
                       << std::setw(8) << d << " "
                       << std::setw(8) << e << " "
                       << std::setw(8) << f << " "
                       << std::setw(8) << g << " "
                       << std::setw(8) << h
                       << std::endl;
        }
        else {
            if (t == 0) {
                TRACE(log) << "           A/E               B/F"
                           << "              C/G              D/H"
                           << std::endl;
            }
            TRACE(log) << "t=" << std::dec << std::setfill(' ') << std::setw(2)
                       << t << ": "
                       << std::hex << std::setfill('0')
                       << std::setw(16) << a << " "
                       << std::setw(16) << b << " "
                       << std::setw(16) << c << " "
                       << std::setw(16) << d << std::endl
                       << "      "
                       << std::setw(16) << e << " "
                       << std::setw(16) << f << " "
                       << std::setw(16) << g << " "
                       << std::setw(16) << h
                       << std::endl;
        }
    }
    /* Computate the intermediate hash */
    _H[0] += a;
    _H[1] += b;
    _H[2] += c;
    _H[3] += d;
    _H[4] += e;
    _H[5] += f;
    _H[6] += g;
    _H[7] += h;

    TRACE(log) << "Hash value:" << std::endl;
    for (int i = 0; i < 8; ++i) {
        TRACE(log) << "H[" << std::dec << i << "] = " << std::hex
                   << std::setfill('0') << std::setw(word_sz*2)
                   << _H[i] << std::endl;
    }

    /* Reset the block for the next message */
    _M_sz = 0;
}

template<typename WordType>
void
sha_algorithm::sha_2_impl<WordType>::finalise()
{
    /* Convert the hash values to big-endian */
    word_type_t H[8];
    for (int i = 0; i < 8; ++i) {
        H[i] = endian::convert(endian_t::native, endian_t::big, _H[i]);
    }

    /* As the H array is correctly byte-oriented, we can just do a byte copy */
    _hash = mud::core::buffer(_digest_size);
    memcpy(_hash.data(), H, _digest_size);
}

template<typename WordType>
WordType
sha_algorithm::sha_2_impl<WordType>::Ch(WordType x, WordType y, WordType z)
{
    return (x & y) ^ (~x & z);
}

template<typename WordType>
WordType
sha_algorithm::sha_2_impl<WordType>::Maj(WordType x, WordType y, WordType z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

template<>
uint32_t
sha_algorithm::sha_2_impl<uint32_t>::sigma_0(uint32_t x)
{
    return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
}

template<>
uint32_t
sha_algorithm::sha_2_impl<uint32_t>::sigma_1(uint32_t x)
{
    return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
}

template<>
uint32_t
sha_algorithm::sha_2_impl<uint32_t>::Sigma_0(uint32_t x)
{
    return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22);
}

template<>
uint32_t
sha_algorithm::sha_2_impl<uint32_t>::Sigma_1(uint32_t x)
{
    return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25);
}

template<>
uint64_t
sha_algorithm::sha_2_impl<uint64_t>::sigma_0(uint64_t x)
{
    return std::rotr(x, 1) ^ std::rotr(x, 8) ^ (x >> 7);
}

template<>
uint64_t
sha_algorithm::sha_2_impl<uint64_t>::sigma_1(uint64_t x)
{
    return std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >> 6);
}

template<>
uint64_t
sha_algorithm::sha_2_impl<uint64_t>::Sigma_0(uint64_t x)
{
    return std::rotr(x, 28) ^ std::rotr(x, 34) ^ std::rotr(x, 39);
}

template<>
uint64_t
sha_algorithm::sha_2_impl<uint64_t>::Sigma_1(uint64_t x)
{
    return std::rotr(x, 14) ^ std::rotr(x, 18) ^ std::rotr(x, 41);
}

/* ==========================================================================
 * SHA-3 algorithm
 * ========================================================================== */

class sha_algorithm::sha_3_impl: public sha_algorithm::impl
{
public:
    /**
     * @brief Constructor.
     * @param digest_bit_size The size of the digest (in bits).
     */
    sha_3_impl(size_t digest_bit_size);

    /**
     * @brief Return the hash specifications.
     */
    std::string specs() const override;

    /**
     * @brief Update the hash with a chunk of binary input data.
     */
    void update(const mud::core::buffer_view& chunk) override;

    /**
     * @brief Close or finalise the hash.
     */
    void close() override;

    /**
     * @brief Return the calculated hash value.
     */
    const mud::core::buffer& hash() const override;

    /**
     * @brief Return the hash size (in bytes).
     */
    size_t hash_size() const override;

    /**
     * @brief Return the block size (in bytes).
     */
    size_t block_size() const override;

private:
    /**
     * @brief Computate the hash on the pre-processed fixed sized message block.
     */
    void computate();

    /* The specification */
    const char* _specs;

    /* The keccak implementation */
    keccak _keccak;
};

sha_algorithm::sha_3_impl::sha_3_impl(size_t digest_bit_size)
    : _keccak(2*digest_bit_size, digest_bit_size,
              keccak::delimited_suffix_t::sha3)
{
    switch (digest_bit_size) {
        case 224:
            _specs = "SHA3-224";
            break;
        case 256:
            _specs = "SHA3-256";
            break;
        case 384:
            _specs = "SHA3-384";
            break;
        case 512:
            _specs = "SHA3-512";
            break;
    }
}

std::string
sha_algorithm::sha_3_impl::specs() const
{
    return _specs;
}

void
sha_algorithm::sha_3_impl::update(const mud::core::buffer_view& chunk)
{
    _keccak.update(chunk);
}

void
sha_algorithm::sha_3_impl::close()
{
    return _keccak.close();
}

const mud::core::buffer&
sha_algorithm::sha_3_impl::hash() const
{
    return _keccak.hash();
}

size_t
sha_algorithm::sha_3_impl::hash_size() const
{
    return _keccak.hash_size();
}

size_t
sha_algorithm::sha_3_impl::block_size() const
{
    return _keccak.block_size();
}

/* ==========================================================================
 * SHA Algorithm
 * ========================================================================== */

sha_algorithm::sha_algorithm(size_t block_size, size_t word_size,
                             size_t digest_size)
    : _block_size(block_size)
    , _word_size(word_size)
    , _digest_size(digest_size)
{
    hash_material_t dummy;
    reset(dummy);
}

std::string
sha_algorithm::specs() const
{
    return _impl->specs();
}

size_t
sha_algorithm::hash_size() const
{
    return _impl->hash_size();
}

size_t
sha_algorithm::block_size() const
{
    return _impl->block_size();
}

void
sha_algorithm::update(const mud::core::buffer& chunk)
{
    _impl->update(chunk.view(chunk.cbegin(), chunk.cend()));
}

void
sha_algorithm::update(const mud::core::buffer_view& chunk)
{
    _impl->update(chunk);
}

void
sha_algorithm::close()
{
    _impl->close();
}

const mud::core::buffer&
sha_algorithm::hash() const
{
    return _impl->hash();
}

void
sha_algorithm::reset(const hash_material_t& /* hashing */)
{
    if (_word_size == 32) {
        if ((_block_size == 512) &&
            (_digest_size == 160)) {
            _impl = std::unique_ptr<impl, impl_deleter>(
                new sha_1_impl());
        }
        else
        if ((_block_size == 512) &&
            (_digest_size == 224 || _digest_size == 256)) {
            _impl = std::unique_ptr<impl, impl_deleter>(
                new sha_2_impl<uint32_t>(_digest_size));
        }
    }
    else
    if (_word_size == 64) {
        if ((_block_size == 1024) &&
            (_digest_size == 224 || _digest_size == 256 ||
             _digest_size == 384 || _digest_size == 512)) {
            _impl = std::unique_ptr<impl, impl_deleter>(
                new sha_2_impl<uint64_t>(_digest_size));
        }
        else
        if ((_block_size == 1152 && _digest_size == 224) ||
            (_block_size == 1088 && _digest_size == 256) ||
            (_block_size == 832  && _digest_size == 384) ||
            (_block_size == 576  && _digest_size == 512)) {
            _impl = std::unique_ptr<impl, impl_deleter>(
                new sha_3_impl(_digest_size));
        }
    }
    if (!_impl) {
        throw crypto_error("Unsupported SHA function");
    }
}

/* SHA Factory Registrations */

hash_factory::registrar<sha_1>       _sha_1_registrar("SHA-1");
hash_factory::registrar<sha_224>     _sha_224_registrar("SHA-224");
hash_factory::registrar<sha_256>     _sha_256_registrar("SHA-256");
hash_factory::registrar<sha_384>     _sha_384_registrar("SHA-384");
hash_factory::registrar<sha_512>     _sha_512_registrar("SHA-512");
hash_factory::registrar<sha_512_224> _sha_512_224_registrar("SHA-512/224");
hash_factory::registrar<sha_512_256> _sha_512_256_registrar("SHA-512/256");
hash_factory::registrar<sha3_224>    _sha3_224_registrar("SHA3-224");
hash_factory::registrar<sha3_256>    _sha3_256_registrar("SHA3-256");
hash_factory::registrar<sha3_512>    _sha3_512_registrar("SHA3-512");

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
