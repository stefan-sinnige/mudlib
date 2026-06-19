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
#include "mud/crypto/hkdf.h"
#include "mud/crypto/exception.h"

/**
 * Internal logging is disabled for security reasons and performance. Only
 * enable it for development purposes.
 */
#define HKDF_LOGGING false
#if HKDF_LOGGING 
    #warning "CAUTION: HKDF logging is enabled"
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
 * The general-purpose HKDF
 * ========================================================================== */

hkdf::hkdf(const kdf_material_t& keying)
    : basic_kdf(keying)
{
    LOG(log);
    TRACE(log) << "HMAC = " << keying.mac_algorithm() << std::endl;
    TRACE(log) << "IKM  = " << keying.ikm() << std::endl;
    TRACE(log) << "Salt = " << keying.salt() << std::endl;

    /* Create the HMAC algorithm */
    auto mac = mac_factory::instance().create(
            keying.mac_algorithm(),
            keying.mac_material());
    if (!mac) {
        throw crypto_error("Unsupported hmac function");
    }
    _hmac = std::unique_ptr<basic_hmac>(static_cast<basic_hmac*>(
            mac.release()));

    /* If salt is not defined, use a string of hash-length */
    mud::core::buffer salt;
    if (keying.salt().size() == 0) {
        salt = mud::core::buffer(_hmac->hash_size());
    }
    else {
        salt = keying.salt();
    }
    
    /* Generate the pseudorandom key using the HMAC with the salt as the
     * key and the IKM as the data. */
    mac_material_t prk_material;
    prk_material.key(salt);
    _hmac->reset(prk_material);
    _hmac->update(keying.ikm());
    _hmac->close();
    _prk = _hmac->tag();
    TRACE(log) << "PRK  = " << _prk << std::endl;
}

hkdf::~hkdf()
{
}

std::string
hkdf::specs() const
{
    return "HKDF";
}

key_t
hkdf::key(size_t ex_key_size /* = 0 */, const mud::core::buffer& ex_info /* = mud::core::buffer() */)
{
    /* If no explicit key-size is defined, use the keying material size */
    size_t key_size = ex_key_size;
    if (ex_key_size == 0) {
        key_size = keying().key_size();
    }

    /* If no explicit info is defined, use the keying material information */
    mud::core::buffer info = ex_info;
    if (info.size() == 0) {
        info = keying().info();
    }

    LOG(log);
    TRACE(log) << "Info = " << info << std::endl;
    TRACE(log) << "L    = " << key_size << std::endl;

    /* The key material */
    mac_material_t key_material;
    key_material.key(_prk);

    /* While we need more data to fill the key with the next hmac result. */
    key_t key;
    uint8_t count = 0x00;
    mud::core::buffer T;
    while (key.size() < key_size) {
        /* Create a new tag, using the previous tag, info and count */
        _hmac->reset(key_material);
        _hmac->update(T);
        _hmac->update(info);
        _hmac->update(mud::core::buffer(&(++count), 1));
        _hmac->close();
        T = _hmac->tag();

        /* Append the tag T to the key */
        key.insert(key.end(), T.begin(), T.end());
    }
    key.resize(key_size);
    TRACE(log) << "OKM  = " << key << std::endl;
    return key;
}

/* ==========================================================================
 * KDF Factory Registration
 * ========================================================================== */

kdf_factory::registrar<hkdf>       _hkdf_registrar("HKDF");

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
