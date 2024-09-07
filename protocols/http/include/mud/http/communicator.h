#ifndef _MUDLIB_HTTP_COMMUNICATOR_H_
#define _MUDLIB_HTTP_COMMUNICATOR_H_

#include <mud/http/ns.h>
#include <mud/http/request.h>
#include <mud/http/response.h>
#include <mud/io/tcp.h>
#include <mud/protocols/communicator.h>

BEGIN_MUDLIB_HTTP_NS

namespace communicator {

/**
 * @brief The server-side HTTP communication protocol.
 *
 * @details
 * HTTP (Hypertext Transfer Protocol) is a highly adopted communication protocol
 * and primarily used in the internet domain. The protocol is request-response
 * based where a client initiates a TCP connection to a server and issues a
 * request to the server. The server will then respond to that request.
 */
class MUDLIB_HTTP_API server:
    public mud::protocols::layered_communicator<mud::io::tcp::socket>
{
public:
    /**
     * @brief The type of the @c impulse when an HTTP request has been received
     * from the device.
     */
    typedef std::shared_ptr<mud::core::impulse<
                const mud::http::request&, mud::http::response&>>
            request_impulse_type;

    /**
     * @brief Create a service-side HTTP communicator.
     *
     * @details
     * Create a communicator that specifically handles the HTTP communication
     * on the server-side.
     *
     * @param lower The communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layered_communicator.
     */
    server(communicator<mud::io::tcp::socket>& lower);

    /**
     * @brief Destructor.
     */
    virtual ~server() = default;

    /**
     * @brief Return the connected state
     *
     * @return True if the connection is available.
     */
    bool connected() const;

    /**
     * @brief The impulse when a request message has been received.
     *
     * @details
     * The pulse is issued when an HTTP request has been received from one
     * of its connected peers. The response to be returned can be manipulated
     * by one of its attached effectors.
     */
    request_impulse_type request_impulse() {
        return _request_impulse;
    }

protected:
    /**
     * @brief Handle the HTTP message.
     *
     * @details
     * Process an HTTP message as received by the device.
     *
     * @param device The device that has something to read.
     */
    virtual void on_receive(mud::io::tcp::socket& device) override;

    /**
     * @brief Handle a connected peer.
     *
     * @details
     * The connection to the peer is available.
     *
     * @param device The device that is opened.
     */
    virtual void on_connect(mud::io::tcp::socket& device) override;

    /**
     * @brief Handle a disconnected peer.
     *
     * @details
     * The connection to the peer is no longer available.
     *
     * @param device The device that was closed.
     */
    virtual void on_disconnect(mud::io::tcp::socket& device) override;

private:
    /** The request impulse. */
    request_impulse_type _request_impulse;

    /** The connected state */
    bool _connected;
};

/**
 * @brief The client-side HTTP communication protocol.
 *
 * @details
 * HTTP (Hypertext Transfer Protocol) is a highly adopted communication protocol
 * and primarily used in the internet domain. The protocol is request-response
 * based where a client initiates a TCP connection to a server and issues a
 * request to the server. The server will then respond to that request.
 */
class MUDLIB_HTTP_API client:
    public mud::protocols::layered_communicator<mud::io::tcp::socket>
{
public:
    /**
     * @brief The type of the @c impulse when an HTTP response has been received
     * from the device.
     */
    typedef std::shared_ptr<mud::core::impulse<const mud::http::response&>>
            response_impulse_type;

    /**
     * @brief Create a service-side HTTP communicator.
     *
     * @details
     * Create a communicator that specifically handles the HTTP communication
     * on the client-side.
     *
     * @param lower The communicator on the next (lower) level. This can be an
     * @c end_communicator or another @c layered_communicator.
     */
    client(communicator<mud::io::tcp::socket>& lower);

    /**
     * @brief Destructor.
     */
    virtual ~client() = default;

    /**
     * @brief Send a request to the server.
     *
     * @details
     * Send the specified HTTP request to the server. Any response will be
     * received through the @c on_receive handler.
     *
     * @param req The request to send.
     */
    void request(const mud::http::request& req);

    /**
     * @brief Return the connected state
     *
     * @return True if the connection is available.
     */
    bool connected() const;

    /**
     * @brief The impulse when a reponse message has been received.
     *
     * @details
     * After a @c request has been sent, the response is received through the
     * @c response_impulse asynchroneoulsy.
     */
    response_impulse_type response_impulse() {
        return _response_impulse;
    }

protected:
    /**
     * @brief Handle the HTTP message.
     *
     * @details
     * Process an HTTP message as received by the device.
     *
     * @param device The device that has something to read.
     */
    virtual void on_receive(mud::io::tcp::socket& device) override;

    /**
     * @brief Handle a connected peer.
     *
     * @details
     * The connection to the peer is available.
     *
     * @param device The device that is opened.
     */
    virtual void on_connect(mud::io::tcp::socket& device) override;

    /**
     * @brief Handle a disconnected peer.
     *
     * @details
     * The connection to the peer is no longer available.
     *
     * @param device The device that was closed.
     */
    virtual void on_disconnect(mud::io::tcp::socket& device) override;

private:
    /** The response impulse. */
    response_impulse_type _response_impulse;

    /** The connected state */
    bool _connected;
};

}; // namespace communicator

END_MUDLIB_HTTP_NS

#endif /* _MUDLIB_HTTP_COMMUNICATOR_H_ */
