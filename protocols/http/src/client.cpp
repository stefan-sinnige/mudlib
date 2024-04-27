#include "mud/http/client.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_HTTP_NS

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
     *  Supply a request and return a future to a response.
     */
    std::future<mud::http::response> request(
            const mud::io::tcp::endpoint& endpoint,
            const mud::http::request& req);

private:
    /** The event-loop */
    mud::event::event_loop& _event_loop;

    /** The connector that establishes a connection */
    mud::io::tcp::connector _connector;

    /** The request */
    mud::http::request _request;

    /** The promise to the response */
    std::promise<mud::http::response> _response;

    /** The handler when a connection has been established */
    void on_connect(mud::io::tcp::socket&&);
};

client::impl::impl(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _connector(_event_loop)
{
    _connector.on_connect(
        std::bind(&client::impl::on_connect, this, std::placeholders::_1));
}

client::impl::~impl() {}

void
client::impl::on_connect(mud::io::tcp::socket&& socket)
{
    socket.ostr() << _request << std::flush;
    auto comm = std::make_shared<io::tcp::communicator>(_event_loop);
    comm->on_receive([comm, this]() mutable {
        mud::http::response response;
        comm->istr() >> response;
        _response.set_value(response);
        comm->close();
        comm.reset();
    });
    comm->open(std::move(socket));
}

std::future<mud::http::response>
client::impl::request(const mud::io::tcp::endpoint& endpoint,
                      const mud::http::request& req)
{
    _request = req;
    _connector.open(endpoint);
    return _response.get_future();
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
