#include "mud/http/server.h"
#include "mud/http/communicator.h"
#include <list>
#include <memory>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief Connection to a peer connection (HTTP client).
 */
class peer: public mud::core::object
{
public:
    /**
     * @brief The type of the @c impulse when an HTTP request has been received
     * from the device.
     */
    typedef std::shared_ptr<mud::core::impulse<
                const mud::http::request&, mud::http::response&>>
            request_impulse_type;

    /*
     * @brief Create a new peer connection communicator.
     *
     * @details
     * The connection has alaready been established.
     *
     * @param socket The socket connected to the peer. The socket shall move
     * its ownership to this instance.
     * @param func The function to invoke when a request has been received.
     */
    peer(mud::io::tcp::socket& socket);

    /*
     * @brief Move constructor
     */
    peer(peer&&) = default;

    /*
     * @brief Move constructor
     */
    peer& operator=(peer&&) = default;

    /*
     * Non-copyable.
     */
    peer(const peer&) = delete;
    peer& operator=(const peer&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~peer() = default;

    /**
     * @brief The impulse when a request message has been received.
     */
    request_impulse_type request_impulse() {
        return _request_impulse;
    }

private:
    /** The handler when an request has been received. */
    void on_request(const mud::http::request& req, mud::http::response& resp);

    /** The TCP end communicator. */
    std::unique_ptr<mud::io::tcp::communicator> _tcp;

    /** The HTTP communicator (server side) */
    std::unique_ptr<mud::http::communicator::server> _http;

    /** The request impulse. */
    request_impulse_type _request_impulse;
};

peer::peer(mud::io::tcp::socket& socket)
{
    _request_impulse = std::make_shared<mud::core::impulse<
            const mud::http::request&, mud::http::response&>>();
    _tcp = std::make_unique<mud::io::tcp::communicator>();
    _http = std::make_unique<mud::http::communicator::server>(*_tcp);
    _http->request_impulse()->attach(this, &peer::on_request);
    _http->open(std::move(socket));
}

void
peer::on_request(const mud::http::request& req, mud::http::response& resp)
{
    _request_impulse->pulse(req, resp);
}

/**
 * @brief Implementation of the HTTP Server.
 */
class server::impl: public mud::core::object
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
     * Constructor
     */
    impl();

    /**
     * Destructor
     */
    ~impl();

    /**
     * Start the server.
     */
    void start(const mud::io::tcp::endpoint& endpoint);

    /**
     * Stop the server.
     */
    void stop();

    /**
     * @brief The impulse when a request message has been received.
     */
    request_impulse_type request_impulse() {
        return _request_impulse;
    }

private:
    /** The handler when accepting new connections. */
    void on_accept(mud::io::tcp::socket&);

    /** The handler when an request has been received. */
    void on_request(const mud::http::request& req, mud::http::response& resp);

    /** The TCP acceptor to listen for incoming connections */
    mud::io::tcp::acceptor _acceptor;

    /** The list of HTTP peer communicators. */
    std::list<peer> _communicators;

    /** The request impulse. */
    request_impulse_type _request_impulse;
};

server::impl::impl()
{
    _request_impulse = std::make_shared<mud::core::impulse<
            const mud::http::request&, mud::http::response&>>();
    _acceptor.accept_impulse()->attach(this, &server::impl::on_accept);
}

server::impl::~impl()
{
    stop();
}

void
server::impl::start(const mud::io::tcp::endpoint& endpoint)
{
    _acceptor.open(endpoint);
}

void
server::impl::stop()
{
    _acceptor.close();
    _communicators.clear();
}

void
server::impl::on_accept(mud::io::tcp::socket& socket)
{
    // Add a communicator handling this new connection
    peer comm(socket);
    comm.request_impulse()->attach(this, &server::impl::on_request);
    _communicators.push_back(std::move(comm)); 
}

void
server::impl::on_request(
   const mud::http::request& req,
   mud::http::response& resp)
{
    _request_impulse->pulse(req, resp);
}

void
server::impl_deleter::operator()(server::impl* ptr) const
{
    delete ptr;
}

server::server()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
    _impl->request_impulse()->attach(this, &server::on_request);
}

void
server::start(const mud::io::tcp::endpoint& endpoint)
{
    _impl->start(endpoint);
}

void
server::stop()
{
    _impl->stop();
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
