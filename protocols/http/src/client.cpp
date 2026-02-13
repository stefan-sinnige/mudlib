#include "mud/http/client.h"
#include "mud/http/communicator.h"
#include "remediator.h"
#include <memory>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief Implementation of the HTTP Client.
 */
class client::impl: public mud::core::object
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor
     */
    ~impl();

    /**
     * Supply a request and return a future to a response.
     */
    std::future<mud::http::response> request(
        const mud::io::tcp::endpoint& endpoint, const mud::http::request& req);

private:
    /** Send the request */
    void request();

    /** The connector that establishes a connection */
    mud::io::tcp::connector _connector;

    /** The end-communicator to interact with the TCP socket */
    mud::io::tcp::communicator _tcp;

    /** The communicator to handle the HTTP protocol */
    mud::http::communicator::client _http;

    /** The request */
    mud::http::request _request;

    /** The promise to the response */
    std::promise<mud::http::response> _response;

    /** The handler when a connection has been established */
    void on_connected(const mud::core::message& msg);

    /** The handler when a connection has been established */
    void on_disconnected(const mud::core::message& msg);

    /** The handler when a message has been received */
    void on_received(const mud::core::message& msg);

    /** The connected state */
    bool _connected;
};

client::impl::impl()
    : _http(_tcp)
    , _connected(false)
{
    attach(_connector.connected(), &client::impl::on_connected);
    attach(_http.disconnected(), &client::impl::on_disconnected);
    attach(_http.received(), &client::impl::on_received);
}

client::impl::~impl()
{
    detach();
}

void
client::impl::request()
{
    LOG(log);
    DEBUG(log) << "HTTP request to "
               << _http.device().destination_endpoint().address().str() << ":"
               << _http.device().destination_endpoint().port() << std::endl;
    TRACE(log) << "Message details: " << std::endl << _request << std::endl;
    _http.ostr() << _request << std::flush;
}

void
client::impl::on_connected(const mud::core::message& msg)
{
    _connected = true;
    _http.open(std::move(_connector.connection()));
    request();
}

void
client::impl::on_disconnected(const mud::core::message& msg)
{
    _connected = false;
}

void
client::impl::on_received(const mud::core::message& msg)
{
    // Logging
    LOG(log);
    DEBUG(log) << "HTTTP response from "
               << _http.device().destination_endpoint().address().str() << ":"
               << _http.device().destination_endpoint().port() << std::endl;

    // Expect an HTTP message.
    bool force_close = false;
    response resp;
    try {
        _http.istr() >> resp;
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
        _http.close();
    }

    // Notify that the response is ready
    _response.set_value(resp);
}

std::future<mud::http::response>
client::impl::request(const mud::io::tcp::endpoint& endpoint,
                      const mud::http::request& req)
{
    // Save the request to send
    _request = req;
    remediator(_request.version().value()).remediate(_request);

    _response = std::promise<mud::http::response>();
    if (!_connected) {
      // No connection established yet.
      _connector.open(endpoint);
    }
    else {
      // Use existing connection.
      request();
    }
    return _response.get_future();
}

void
client::impl_deleter::operator()(client::impl* ptr) const
{
    delete ptr;
}

client::client()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

std::future<mud::http::response>
client::request(const mud::io::tcp::endpoint& endpoint,
                const mud::http::request& req)
{
    return _impl->request(endpoint, req);
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
