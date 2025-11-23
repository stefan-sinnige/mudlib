#include "mud/http/communicator.h"
#include "remediator.h"

BEGIN_MUDLIB_HTTP_NS

namespace communicator {

server::server(communicator<mud::io::tcp::socket>& lower)
    : mud::protocols::layered_communicator<mud::io::tcp::socket>(lower)
    , _connected(false)
{
    _request_impulse = std::make_shared<mud::core::impulse<
            const mud::http::request&, mud::http::response&>>();
}

bool
server::connected() const
{
    return _connected;
}

void
server::on_connect(mud::io::tcp::socket& /* unused */socket)
{
    _connected = true;
}

void
server::on_disconnect(mud::io::tcp::socket& /* unused */socket)
{
    _connected = false;
}

void
server::on_receive(mud::io::tcp::socket& /* unused */socket)
{
    // Logging
    LOG(log);
    DEBUG(log) << "HTTTP request from "
               << socket.destination_endpoint().address().str() << ":"
               << socket.destination_endpoint().port() << std::endl;

    // Expect an HTTP message.
    request req;
    response resp;
    try {
        istr() >> req;
        if (istr().fail()) {
            throw std::runtime_error("HTTP connection error");
        }
    } catch (...) {
        resp.version(req.version());
        resp.status_code(mud::http::status_code_e::BadRequest);
        resp.reason_phrase(mud::http::reason_phrase_e::BadRequest);
        return;
    }
    TRACE(log) << "Message details: " << std::endl << req << std::endl;

    // Create a response from the callback function.
    try {
        _request_impulse->pulse(req, resp);
    } catch (...) {
        resp.version(req.version());
        resp.status_code(mud::http::status_code_e::InternalServerError);
        resp.reason_phrase(mud::http::reason_phrase_e::InternalServerError);
        return;
    }

    // Remediate the response
    remediator(resp.version().value()).remediate(req, resp);

    // Send the response.
    DEBUG(log) << "HTTTP response to "
               << socket.destination_endpoint().address().str() << ":"
               << socket.destination_endpoint().port() << std::endl;
    TRACE(log) << "Message details: " << std::endl << req << std::endl;
    ostr() << resp << std::flush;

    // For HTTP/1.0, the server is expected to close the connection. However,
    // this results in an accumulation of TIME_WAIT sockets on the server which
    // may render the server to be inoperable (too many open file descriptors
    // - defined by ulimit). As the socket port number is dynamically assigned
    // when a connection is established, there is no remedy for this.
    // It is strongly advised to move the TIME_WAIT towards the client as it can
    // manage the TIME_WAIT better by reusing the address & port number through
    // a socket-option (SO_REUSEADDR). The side that initiates the closing of
    // the connection will need to manage the TIME_WAIT. For HTTP/1.1 the server
    // will not initiate the close but relies on the client to do so.
    if (resp.version() == mud::http::version_e::HTTP10) {
        close();
    }
}

client::client(communicator<mud::io::tcp::socket>& lower)   
    : mud::protocols::layered_communicator<mud::io::tcp::socket>(lower)
    , _connected(false)
{
    _response_impulse = std::make_shared<
        mud::core::impulse<const mud::http::response&>>();
}

bool
client::connected() const
{
    return _connected;
}

void
client::request(const mud::http::request& req)
{
    // Remediate the request
    mud::http::request request = req;
    remediator(request.version().value()).remediate(request);

    // Logging
    LOG(log);
    DEBUG(log) << "HTTP request to "
               << device().destination_endpoint().address().str() << ":"
               << device().destination_endpoint().port() << std::endl;
    TRACE(log) << "Message details: " << std::endl << request << std::endl;

    ostr() << request << std::flush;
}

void
client::on_connect(mud::io::tcp::socket& /* unused */socket)
{
    _connected = true;
}

void
client::on_disconnect(mud::io::tcp::socket& /* unused */socket)
{
    _connected = false;
}

void
client::on_receive(mud::io::tcp::socket& /* unused */ socket)
{
    // Logging
    LOG(log);
    DEBUG(log) << "HTTTP response from "
               << socket.destination_endpoint().address().str() << ":"
               << socket.destination_endpoint().port() << std::endl;

    // Expect an HTTP message.
    bool force_close = false;
    response resp;
    try {
        istr() >> resp;
        if (!istr().bad()) {
            _response_impulse->pulse(resp);
        }
    } catch (std::exception& ex) {
        force_close = true;
    }
    TRACE(log) << "Message details: " << std::endl << resp << std::endl;

    // Examine any Connection flag to see if we need to stay open
    //   * Not for HTTP 1.0
    //     * Connection always closed after receiving the response
    //   * Use the setting from the response
    //   * Use keep-alive if none is defined in the response
    if (resp.version() == mud::http::version_e::HTTP10) {
        force_close = true;
    }
    else {
        if (resp.exists<mud::http::connection>()) {
            auto conn = resp.field<mud::http::connection>();
            force_close = (conn == mud::http::connection_e::Close);
        }
    }

    // Close the connection if we need to
    if (force_close) {
        close();
    }
}

}; // namespace communicator

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
