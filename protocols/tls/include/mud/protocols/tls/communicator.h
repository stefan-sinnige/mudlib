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

#ifndef _MUDLIB_TLS_COMMUNICATOR_H_
#define _MUDLIB_TLS_COMMUNICATOR_H_

#include <future>
#include <mud/protocols/tls/ns.h>
#include <mud/crypto/sha.h>
#include <mud/io/tcp.h>
#include <mud/protocols/communicator.h>

BEGIN_MUDLIB_TLS_NS

namespace communicator {

/**
 * @brief Base communicator for the TLS communication protocol.
 * @details
 * TLS (Transport Layer Security) protocol is a commonly used cryptographic
 * protocol to add a security layer between two communicating entities. The
 * protocol is designed that is can act as an envelope for another higher
 * level protocol as it is agnostic of the binary data it secures. 
 *
 * This conforms with RFC 8446
 */
class MUDLIB_TLS_API basic_tls
{
public:
private:
    /* The hand-shake hash that is used for key calculation */
    crypto::sha_384 handshake_hash;
};

/**
 * @brief The server-side TLS communication protocol.
 * @tparam Device The device type
 * @details
 * TLS (Transport Layer Security) protocol is a commonly used cryptographic
 * protocol to add a security layer between two communicating entities. The
 * protocol is designed that is can act as an envelope for another higher
 * level protocol as it is agnostic of the binary data it secures. 
 *
 * This conforms with RFC 8446
 */
template<typename Device>
class MUDLIB_TLS_API server:
    public mud::protocols::layered_communicator<Device>,
    public basic_tls
{
public:
    /**
     * @brief Create a server-side TLS communicator.
     * @param lower the communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layer_communicator.
     */
    server(mud::protocols::communicator<Device>& lower)
        : mud::protocols::layered_communicator<Device>(lower)
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~server() = default;
};

/**
 * @brief The client-side TLS communication protocol.
 * @tparam Device The device type
 * @details
 * TLS (Transport Layer Security) protocol is a commonly used cryptographic
 * protocol to add a security layer between two communicating entities. The
 * protocol is designed that is can act as an envelope for another higher
 * level protocol as it is agnostic of the binary data it secures. 
 *
 * This conforms with RFC 8446
 */
template<typename Device>
class MUDLIB_TLS_API client:
    public mud::protocols::layered_communicator<Device>,
    public basic_tls
{
public:
    /**
     * @brief Create a clienbt-side TLS communicator.
     * @param lower the communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layer_communicator.
     */
    client(mud::protocols::communicator<Device>& lower)
        : mud::protocols::layered_communicator<Device>(lower)
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~client() = default;
};

}; // namespace communicator

END_MUDLIB_TLS_NS

#endif /* _MUDLIB_TLS_COMMUNICATOR_H_ */
