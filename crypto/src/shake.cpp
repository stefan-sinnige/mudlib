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
#include "mud/crypto/shake.h"
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
#define SHAKE_LOGGING false
#if SHAKE_LOGGING 
    #warning "CAUTION: SHAKE logging is enabled"
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

class shake_algorithm::impl
{
public:
    /**
     * @brief Constructor.
     * @param capacity The size of the capacity (in bits).
     * @param digest_bit_size The size of the digest (in bits).
     */
    impl(size_t capacity, size_t digest_bit_size);

    /**
     * @brief Return the hash specifications.
     */
    std::string specs() const;

    /**
     * @brief Update the hash with a chunk of binary input data.
     */
    void update(const data_t& chunk);

    /**
     * @brief Close or finalise the hash.
     */
    void close();

    /**
     * @brief Return the calculated hash value.
     */
    const data_t& hash() const;

    /**
     * @brief Return the hash size (in bytes).
     */
    size_t hash_size() const;

    /**
     * @brief Return the block size (in bytes).
     */
    size_t block_size() const;

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

void
shake_algorithm::impl_deleter::operator()(shake_algorithm::impl* ptr) const
{
    delete ptr;
}

shake_algorithm::impl::impl(size_t capacity, size_t digest_bit_size)
    : _keccak(capacity, digest_bit_size, keccak::delimited_suffix_t::shake)
{
    switch (capacity) {
        case 128:
            _specs = "SHAKE-128";
            break;
        case 256:
            _specs = "SHAKE-256";
            break;
    }
}

std::string
shake_algorithm::impl::specs() const
{
    return _specs;
}

void
shake_algorithm::impl::update(const data_t& chunk)
{
    _keccak.update(chunk);
}

void
shake_algorithm::impl::close()
{
    return _keccak.close();
}

const data_t&
shake_algorithm::impl::hash() const
{
    return _keccak.hash();
}

size_t
shake_algorithm::impl::hash_size() const
{
    return _keccak.hash_size();
}

size_t
shake_algorithm::impl::block_size() const
{
    return _keccak.block_size();
}

/* ==========================================================================
 * SHAKE Algorithm
 * ========================================================================== */

shake_algorithm::shake_algorithm(size_t capacity, size_t digest_bit_size)
    : _capacity(capacity)
    , _digest_bit_size(digest_bit_size)
{
    hash_material_t dummy;
    reset(dummy);
}

std::string
shake_algorithm::specs() const
{
    return _impl->specs();
}

size_t
shake_algorithm::hash_size() const
{
    return _impl->hash_size();
}

size_t
shake_algorithm::block_size() const
{
    return _impl->block_size();
}

void
shake_algorithm::update(const data_t& chunk)
{
    _impl->update(chunk);
}

void
shake_algorithm::close()
{
    _impl->close();
}

const data_t&
shake_algorithm::hash() const
{
    return _impl->hash();
}

void
shake_algorithm::reset(const hash_material_t& /* hashing */)
{
    if (_capacity == 256 || _capacity == 512) {
        _impl = std::unique_ptr<impl, impl_deleter>(
                new impl(_capacity, _digest_bit_size));
    }
    if (!_impl) {
        throw crypto_error("Unsupported SHAKE function");
    }
}

/* Hash Factory Registrations */

hash_factory::registrar<shake_128> _shake_128_registrar("SHAKE-128");
hash_factory::registrar<shake_256> _shake_256_registrar("SHAKE-256");

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
