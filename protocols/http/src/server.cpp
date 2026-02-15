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

#include "mud/http/server.h"
#include "mud/http/communicator.h"
#include "remediator.h"
#include <list>
#include <memory>

BEGIN_MUDLIB_HTTP_NS

/**
 * @brief Function pointer to a request handler.
 */
typedef std::function<mud::http::response(const mud::http::request&)>
        request_handler;

/**
 * @brief Connection to a peer connection (HTTP client).
 */
class peer: public mud::core::object
{
public:
    /*
     * @brief Create a new peer connection communicator.
     *
     * @details
     * The connection has alaready been established.
     *
     * @param socket The socket connected to the peer. The socket shall move
     * its ownership to this instance.
     * @param handler The handler to invoke when an HTTP message has been
     * received.
     */
    peer(mud::io::tcp::socket& socket, request_handler handler);

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
    virtual ~peer();

private:
    /** The handler when an request has been received. */
    void on_requested(const mud::core::message& msg);

    /** The handler for processing the request and determining a response. */
    request_handler _handler;

    /** The TCP end communicator. */
    std::unique_ptr<mud::io::tcp::communicator> _tcp;

    /** The HTTP communicator (server side) */
    std::unique_ptr<mud::http::communicator::server> _http;
};

peer::peer(mud::io::tcp::socket& socket, request_handler handler)
    : _handler(handler)
{
    _tcp = std::make_unique<mud::io::tcp::communicator>();
    _http = std::make_unique<mud::http::communicator::server>(*_tcp);
    attach(_http->received(), &peer::on_requested);
    _http->open(std::move(socket));
}

peer::~peer()
{
    detach();
}

void
peer::on_requested(const mud::core::message& msg)
{
    // Logging
    LOG(log);
    DEBUG(log) << "HTTTP request from "
               << _http->device().destination_endpoint().address().str() << ":"
               << _http->device().destination_endpoint().port() << std::endl;

    // Read the HTTP request and get the response
    request req;
    response resp;
    try {
        // Read the request
        _http->istr() >> req;
        if (_http->istr().fail()) {
            throw std::runtime_error("HTTP connection error");
        }
        TRACE(log) << "Message details: " << std::endl << req << std::endl;

        // Generate the response
        if (_handler) {
            resp = _handler(req);
        }
        else {
            resp.version(req.version());
        resp.status_code(mud::http::status_code_e::NotImplemented);
        resp.reason_phrase(mud::http::reason_phrase_e::NotImplemented);
        }
    } catch (...) {
        resp.version(req.version());
        resp.status_code(mud::http::status_code_e::BadRequest);
        resp.reason_phrase(mud::http::reason_phrase_e::BadRequest);
    }

    // Remediate the response
    remediator(resp.version().value()).remediate(req, resp);

    // Send the response.
    DEBUG(log) << "HTTTP response to "
               << _http->device().destination_endpoint().address().str() << ":"
               << _http->device().destination_endpoint().port() << std::endl;
    TRACE(log) << "Message details: " << std::endl << resp << std::endl;
    _http->ostr() << resp << std::flush;

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
        _http->close();
    }
}

/**
 * @brief Implementation of the HTTP Server.
 */
class server::impl: public mud::core::object
{
public:
    /**
     * Constructor
     * @param handler The handler to invoke when an HTTP message has been
     * received.
     */
    impl(request_handler handler);

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
     * @brief Retun the listening end-point.
     */
    const mud::io::tcp::endpoint& endpoint() const;

private:
    /** The handler when accepting new connections. */
    void on_accepted(const mud::core::message& msg);

    /** The listening endpoint */
    mud::io::tcp::endpoint _endpoint;

    /** The TCP acceptor to listen for incoming connections */
    mud::io::tcp::acceptor _acceptor;

    /** The list of HTTP peer communicators. */
    std::list<peer> _communicators;

    /** The handler for processing the request and determining a response. */
    request_handler _handler;
};

server::impl::impl(request_handler handler)
    : _handler(handler)
{
    attach(_acceptor.accepted(), &server::impl::on_accepted);
}

server::impl::~impl()
{
    detach();
    stop();
}

void
server::impl::start(const mud::io::tcp::endpoint& endpoint)
{
    _endpoint = endpoint;
    _acceptor.open(_endpoint);
}

void
server::impl::stop()
{
    _acceptor.close();
    _communicators.clear();
}

const mud::io::tcp::endpoint& 
server::impl::endpoint() const
{
    return _endpoint;
}

void
server::impl::on_accepted(const mud::core::message& msg)
{
    // Add a peer-communicator handling this new connection
    auto socket = _acceptor.connection();
    if (socket.handle()) {
        peer comm(socket, _handler);
        _communicators.push_back(std::move(comm)); 
    }
}

void
server::impl_deleter::operator()(server::impl* ptr) const
{
    delete ptr;
}

server::server()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(
        std::bind(&server::request, this, std::placeholders::_1)));
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

const mud::io::tcp::endpoint&
server::endpoint() const
{
    return _impl->endpoint();
}

mud::http::response
server::request(const mud::http::request& req)
{
    mud::http::response resp;
    return resp;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
