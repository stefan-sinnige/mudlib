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

#include "mud/protocols/tls/message.h"
#include <stdexcept>

BEGIN_MUDLIB_TLS_NS

/* SSL 3.0 was given version 0x0300, and TLS 3.0 started off as SSL 3.1, was
 * given the next one up. Note that the version number in the message are
 * deprecated in favour of the 'Supported Version' extension. The legacy version
 * numbers are largely hard-coded now. */
const uint16_t tls_10_protocol_version = 0x0301;
const uint16_t tls_11_protocol_version = 0x0302;
const uint16_t tls_12_protocol_version = 0x0303;
const uint16_t tls_13_protocol_version = 0x0304;

handshake::handshake(type_t type)
  : _msg_type(type)
  , _length(0)
{
}

/* ====================================================================== 
 * Client Hello
 * ====================================================================== */

client_hello::client_hello()
    : handshake(handshake::type_t::client_hello)
    , _protocol_version(tls_12_protocol_version)
{
}

/* ====================================================================== 
 * Server Hello
 * ====================================================================== */

server_hello::server_hello()
    : handshake(handshake::type_t::server_hello)
    , _protocol_version(tls_12_protocol_version)
{
}

/* ====================================================================== 
 * Streaming operators
 * ====================================================================== */

std::ostream&
operator<<(std::ostream& ostr, const handshake& msg)
{
    return ostr;
}

std::istream&
operator>>(std::istream& istr, handshake& msg)
{
    return istr;
}

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
