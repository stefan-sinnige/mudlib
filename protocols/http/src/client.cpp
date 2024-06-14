#include "mud/http/client.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_HTTP_NS

class client::communicator : public mud::io::tcp::communicator
{
public:
    /** Function definition for the @c on_respone handler. The response message
     * as received is passed in. */
    typedef std::function<void(const mud::http::response&)> on_response_func;

    /**
     * @brief Constructor.
     * @param[in] event_loop  The event-loop to register the socket to.
     */
    communicator(on_response_func func, mud::event::event_loop& event_loop =
                                            mud::event::event_loop::global());

    /**
     * @brief Move constructor.
     # @param[in] rhs  The object to move from.
     */
    communicator(communicator&& rhs) = default;

    /**
     * @brief Move assignment.
     # @param[in] rhs  The object to move from.
     */
    communicator& operator=(communicator&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~communicator() = default;

    /**
     * @brief Send a request.
     */
    void request(const mud::http::request& req);

    /**
     * Check if there is anything available to read (as expected).
     * @return True if there is data available.
     */
    bool data_available();

    /**
     * Non-copyable.
     */
    communicator(const communicator&) = delete;
    communicator& operator=(const communicator&) = delete;

private:
    /**
     * Generic TCP receive handler.
     */
    void on_receive();

    /**
     * The handler for HTTP requests.
     */
    on_response_func _on_response_func;
};

client::communicator::communicator(on_response_func func,
                                   mud::event::event_loop& event_loop)
  : mud::io::tcp::communicator(event_loop), _on_response_func(func)
{
    mud::io::tcp::communicator::on_receive(
        std::bind(&client::communicator::on_receive, this));
}

bool
client::communicator::data_available()
{
    // If not connected, return.
    if (!connected()) {
        return false;
    }

    // Attempt to read one character.
    if (istr().get() == std::char_traits<char>::eof()) {
        close();
        return false;
    } else {
        istr().unget();
        return true;
    }
}

void
client::communicator::request(const mud::http::request& req)
{
    ostr() << req << std::flush;
}

void
client::communicator::on_receive()
{
    // Check if there is something available. If there is nothing available,
    // the connection can be closed.
    if (!data_available()) {
        close();
        return;
    }

    // Expect an HTTP message.
    bool force_close = false;
    response resp;
    try {
        istr() >> resp;
        if (!istr().bad()) {
            if (_on_response_func != nullptr) {
                _on_response_func(resp);
            }
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

/**
 * @brief Implementation of the HTTP Client.
 */
class client::impl
{
public:
    /**
     * Constructor
     */
    impl(mud::event::event_loop& event_loop);

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

    /** The event-loop */
    mud::event::event_loop& _event_loop;

    /** The connector that establishes a connection */
    mud::io::tcp::connector _connector;

    /** The communicator once a connection has been established */
    client::communicator _communicator;

    /** The request */
    mud::http::request _request;

    /** The promise to the response */
    std::promise<mud::http::response> _response;

    /** The handler when a connection has been established */
    void on_connect(mud::io::tcp::socket&&);
};

client::impl::impl(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _connector(_event_loop),
    _communicator(
        std::bind(&client::impl::on_response, this, std::placeholders::_1),
        _event_loop)
{
    _connector.on_connect(
        std::bind(&client::impl::on_connect, this, std::placeholders::_1));
}

client::impl::~impl() {}

void
client::impl::on_connect(mud::io::tcp::socket&& socket)
{
    _communicator.open(std::move(socket));
    _communicator.request(_request);
}

std::future<mud::http::response>
client::impl::request(const mud::io::tcp::endpoint& endpoint,
                      const mud::http::request& req)
{
    // Get the request to send and ensure it is normalised.
    _request = req;
    _request.normalise();

    _response = std::promise<mud::http::response>();
    if (!_communicator.connected()) {
      // No connection established yet.
      _connector.open(endpoint);
    }
    else {
      // Use existing connection.
      _communicator.request(_request);
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

client::client(mud::event::event_loop& event_loop)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(event_loop));
}

std::future<mud::http::response>
client::request(const mud::io::tcp::endpoint& endpoint,
                const mud::http::request& req)
{
    return _impl->request(endpoint, req);
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
