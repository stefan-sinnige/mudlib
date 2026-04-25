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
#include "mud/crypto/hmac.h"
#include "mud/crypto/exception.h"

/**
 * Internal logging is disabled for security reasons and performance. Only
 * enable it for development purposes.
 */
#define HMAC_LOGGING false
#if HMAC_LOGGING 
    #warning "CAUTION: HMAC logging is enabled"
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
 * Basic HMAC
 * ========================================================================== */

basic_hmac::basic_hmac(const mac_material_t& keying)
    : basic_mac(keying)
{
}

/* ==========================================================================
 * MAC Factory Registration
 * ========================================================================== */

mac_factory::registrar<hmac<sha_1>>   _hmac_sha_1_registrar("HMAC-SHA-1");
mac_factory::registrar<hmac<sha_224>> _hmac_sha_224_registrar("HMAC-SHA-224");
mac_factory::registrar<hmac<sha_256>> _hmac_sha_256_registrar("HMAC-SHA-256");
mac_factory::registrar<hmac<sha_384>> _hmac_sha_384_registrar("HMAC-SHA-384");
mac_factory::registrar<hmac<sha_512>> _hmac_sha_512_registrar("HMAC-SHA-512");

END_MUDLIB_CRYPTO_NS

/* vi: set ai ts=4 expandtab: */
