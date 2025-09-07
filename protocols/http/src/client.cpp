#include "mud/http/client.h"
#include "mud/http/communicator.h"
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
    /** Callback function when a response has been received */
    void on_response(const response& resp);

    /** The connector that establishes a connection */
    mud::io::tcp::connector _connector;

    /** The end-communicator to interact with the TCP socket */
    mud::io::tcp::communicator _tcp_communicator;

    /** The communicator to handle the HTTP protocol */
    mud::http::communicator::client _http_communicator;

    /** The request */
    mud::http::request _request;

    /** The promise to the response */
    std::promise<mud::http::response> _response;

    /** The handler when a connection has been established */
    void on_connect(mud::io::tcp::socket&);

    /** The handler when a message has been received */
    void on_receive(mud::io::tcp::socket&);
};

client::impl::impl()
    : _http_communicator(_tcp_communicator)
{
    _connector.connect_impulse()->attach(this, &client::impl::on_connect);
    //_http_communicator.receive_impulse()->attach(this, &client::impl::on_receive);
    _http_communicator.response_impulse()->attach(this, &client::impl::on_response);
}

client::impl::~impl() {}

void
client::impl::on_connect(mud::io::tcp::socket& socket)
{
    _http_communicator.open(std::move(socket));
    _http_communicator.request(_request);
}

void
client::impl::on_receive(mud::io::tcp::socket& socket)
{
}

std::future<mud::http::response>
client::impl::request(const mud::io::tcp::endpoint& endpoint,
                      const mud::http::request& req)
{
    // Save the request to send
    _request = req;

    _response = std::promise<mud::http::response>();
    if (!_http_communicator.connected()) {
      // No connection established yet.
      _connector.open(endpoint);
    }
    else {
      // Use existing connection.
      _http_communicator.request(_request);
    }
    return _response.get_future();
}

void
client::impl::on_response(const mud::http::response& resp)
{
    _response.set_value(resp);
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
