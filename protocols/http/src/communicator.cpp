#include "mud/http/communicator.h"

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
    // Expect an HTTP message.
    bool force_close = false;
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
        force_close = true;
    }

    // Create a response from the callback function.
    try {
        _request_impulse->pulse(req, resp);
    } catch (...) {
        resp.version(req.version());
        resp.status_code(mud::http::status_code_e::InternalServerError);
        resp.reason_phrase(mud::http::reason_phrase_e::InternalServerError);
        force_close = true;
    }

    // Set the Connection header field
    //   * Not for HTTP 1.0
    //     * Connection always closed after sending the response
    //   * Use the response setting if it has been defined
    //   * Use the request setting if it has been defined
    //   * Use keep-alive if none is defined in the response or request
    if (resp.version() == mud::http::version_e::HTTP10) {
        force_close = true;
    }
    else
    {
        if (force_close) {
            resp.field<mud::http::connection>(mud::http::connection_e::Close);
        }
        else {
            auto conn = mud::http::connection_e::KeepAlive;
            if (resp.exists<mud::http::connection>()) {
                conn = resp.field<mud::http::connection>();
            }
            else
            if (req.exists<mud::http::connection>()) {
                conn = req.field<mud::http::connection>();
            }
            if (conn == mud::http::connection_e::Close) {
                force_close = true;
            }
            resp.field<mud::http::connection>(conn);
        }
    }

    // Send the response.
    ostr() << resp << std::flush;

    // Close the connection if we need to
    if (force_close)
    {
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
    ostr() << req << std::flush;
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
    // Expect an HTTP message.
    bool force_close = false;
    response resp;
    try {
        istr() >> resp;
        if (!istr().bad()) {
            _response_impulse->pulse(resp);
        }
    } catch (...) {
        force_close = true;
    }

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
