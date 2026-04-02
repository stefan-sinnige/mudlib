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

#ifndef _MUDLIB_CRYPTO_AES_H_
#define _MUDLIB_CRYPTO_AES_H_

#include <mud/crypto/ns.h>
#include <mud/crypto/block_cipher.h>

BEGIN_MUDLIB_CRYPTO_NS

/**
 * @brief Generic Advanced Encryption System (AES) encryption and decryption
 * @details
 * Reference implementation for AES encryption and decryption for any of the
 * supported key-sizes, in accordance with NIST FIPS 197.
 *
 * The AES algorithm performs operations on a cipher or plain-text of 128 bits
 * (16 byts) that is transformed to a two-dimensional four-by-four array
 * of bytes, known as the @em state. After a series of transformations, the
 * state can be converted to an output array in the reverse.
 */
class basic_aes : public basic_algorithm
{
public:
    /**
     * @brief Constructor.
     * @param key_size
     */
    basic_aes(size_t key_size);

    /**
     * @brief Return the algorithm specifications.
     */
    constexpr std::string_view specs() const override { return "AES"; }

    /**
     * @brief Return the block-size used by the algorithm.
     */
    size_t block_size() const override { return 16; }
     
    /**
     * @brief Return the key-size used by the algorithm.
     */
    size_t key_size() const override { return _key_size; }
     
    /**
     * @brief Encrypt plain-text into cipher-text.
     * @param input The input block (ie plain text).
     * @param output The output block (ie cipher text).
     * @param key The key buffer
     */
    void encrypt(const data_t& input, data_t& output,
                 const key_t& key) override;

    /**
     * @brief Decrypt cipher-text into plain-text.
     * @param input The input block (ie cipher text).
     * @param output The output block (ie plain text).
     * @param key The key buffer
     */
    void decrypt(const data_t& input, data_t& output,
                 const key_t& key) override;

private:
    /**
     * @brief Type of an AES state.
     * @details
     * A state type is used internally in the AES tranformation algorithms and
     * consists of two-dimensional array of 4x4 bytes, where each column is
     * representing a (big-endian) 32-bit word. For example, to use the first
     * section of an input array \f$in_0, in_1, ... in_15\f$:
     * @f[
     * s[r,c] = in[r + 4c]
     * @f]
     */
    class state;
    friend std::ostream& operator<<(std::ostream&, const state& s);

    /**
     * @brief Perform the key expansion.
     * @param key The key of key-size bits.
     * @return The expanded key
     */
    std::vector<uint32_t> key_expansion(const key_t& key);

    /**
     * @brief Apply a round-key to the state
     * @param state The state to transform (s[0..3])
     * @param w The pointer to the round keys to apply (w[0..3])
     * @return Reference to @c state
     */
    state& add_round_key(state& state, uint32_t* w);

    /**
     * @brief Apply the sub-bytes transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& sub_bytes(state& state);

    /**
     * @brief Apply the shift-rows transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& shift_rows(state& state);

    /**
     * @brief Apply the mix-columns transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& mix_columns(state& state);

    /**
     * @brief Apply the inverse sub-bytes transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& inv_sub_bytes(state& state);

    /**
     * @brief Apply the inverse shift-rows transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& inv_shift_rows(state& state);

    /**
     * @brief Apply the inverse mix-columns transformation.
     * @param state The state to transform (s[0..3])
     * @return Reference to @c state
     */
    state& inv_mix_columns(state& state);

    /**
     * @brief Rotate a word (to the left over 8-bits with wrapping.
     * @param w The word to rotate
     * @return The rotated word.
     */
    uint32_t rot_word(uint32_t w);

    /**
     * @brief Apply the substitution box to a word.
     * @param w The word to apply the substitution to,
     * @return The applied substitution.
     */
    uint32_t sub_word(uint32_t w);

    /**
     * @brief Multiplication in Galois-Field GF(2^8)
     * @param x The multiplicant
     * @param y The multiplier
     * @return The Galois-Field GF(2^8) multiplication of @c x times @c y.
     */
    uint8_t gfmul(uint8_t x, uint8_t y);

    /** The key-size in bits. This is either 128, 192 or 256. */
    size_t _key_size;

    /* The number of columns of a state. Fixed to 4. */
    size_t _Nb;

    /* The number of 32-bit words in the key. */
    size_t _Nk;

    /* The number of rounds, either 10, 12 or 14 based on the key size */
    size_t _Nr;
};

/**
 * @brief Advanced Encryption System (AES) encryption and decryption
 * @tparam BlockSize Fixed-length block-size.
 * @details
 * Rijndael Block Cipher was chosen by the National Institute of Standards
 * (NIST) as the adoption for for the Advanced Encryption System (AES) and
 * defined in FIPS PUB 197. The underlying mechanism is a substitution and
 * permutation (SP) network where the plaintext enters through a number of
 * substition and permutation boxes that has been keyed with a symmetric key.
 *
 * AES uses a fixed block-size of 128-bit and three options for key-sizes:
 * 128, 192 and 256 bits. Each key-size defines the total number of
 * transformation where the plain-text passes trough the substition (S) and
 * permutation boxes (P). These total rounds are respectively 10, 12 and 14.
 */
template<size_t KeySize>
class MUDLIB_CRYPTO_API aes: public basic_aes
{
public:
    /**
     * @brief Constructor
     */
    aes();
};

template<size_t KeySize>
aes<KeySize>::aes()
    : basic_aes(KeySize)
{
}

/**
 * @brief AES encryption with ECB mode.
 */
typedef block_cipher<aes<128>, ecb> AES_128_ECB;
typedef block_cipher<aes<192>, ecb> AES_192_ECB;
typedef block_cipher<aes<256>, ecb> AES_256_ECB;

/**
 * @brief AES encryption with CBC mode.
 */
typedef block_cipher<aes<128>, cbc> AES_128_CBC;
typedef block_cipher<aes<192>, cbc> AES_192_CBC;
typedef block_cipher<aes<256>, cbc> AES_256_CBC;

/**
 * @brief AES encryption with CFB mode.
 */
typedef block_cipher<aes<128>, cfb> AES_128_CFB;
typedef block_cipher<aes<192>, cfb> AES_192_CFB;
typedef block_cipher<aes<256>, cfb> AES_256_CFB;

/**
 * @brief AES encryption with CTR mode.
 */
typedef block_cipher<aes<128>, ctr> AES_128_CTR;
typedef block_cipher<aes<192>, ctr> AES_192_CTR;
typedef block_cipher<aes<256>, ctr> AES_256_CTR;

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CRYPTO_AES_H_ */
