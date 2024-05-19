#include "mud/http/server.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_HTTP_NS

class server::communicator : public mud::io::tcp::communicator
{
public:
    /**
     * @brief Constructor.
     * @param[in] event_loop  The event-loop to register the socket to.
     */
    communicator(
        server::on_request_func func,
        mud::event::event_loop& event_loop = mud::event::event_loop::global());

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
     * @brief Return the connected state.
     */
    bool connected() const { return _connected; }

    /**
     * Non-copyable.
     */
    communicator(const communicator&) = delete;
    communicator& operator=(const communicator&) = delete;

private:
    /**
     * Check if there is anything available to read (as expected). Set the
     * @c connected state accordingly.
     * @return True if there is data available.
     */
    bool data_available();

    /**
     * Generic TCP receive handler.
     */
    void on_receive();

    /**
     * The handler for HTTP requests.
     */
    server::on_request_func _on_request_func;

    /**
     * The connected state.
     */
    bool _connected;
};

server::communicator::communicator(server::on_request_func func,
                                   mud::event::event_loop& event_loop)
  : mud::io::tcp::communicator(event_loop), _on_request_func(func),
    _connected(true)
{
    mud::io::tcp::communicator::on_receive(
        std::bind(&server::communicator::on_receive, this));
}

bool
server::communicator::data_available()
{
    // If not connected, return.
    if (!_connected) {
        return false;
    }

    // Attempt to read one character.
    if (istr().get() == std::char_traits<char>::eof()) {
        close();
        _connected = false;
    } else {
        istr().unget();
    }
    return _connected;
}

void
server::communicator::on_receive()
{
    // Check if there is something available. If there is nothing, assume
    // that the connection can be closed.
    if (!data_available()) {
        close();
        return;
    }

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
        if (!force_close && _on_request_func != nullptr) {
            resp = _on_request_func(req);
        }
        else {
            throw std::runtime_error("No HTTP response function");
        }
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
        _connected = false;
    }
}

/**
 * @brief Implementation of the HTTP Server.
 */
class server::impl
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
     * Start the server.
     */
    void start(const mud::io::tcp::endpoint& endpoint);

    /**
     * Stop the server.
     */
    void stop();

    /*
     * The handler when a request has been received.
     */
    void on_request(on_request_func func);

private:
    /** The event-loop */
    mud::event::event_loop& _event_loop;

    /** The TCP acceptor to listen for incoming connections */
    mud::io::tcp::acceptor _acceptor;

    /** The list of HTTP communicators. */
    std::list<server::communicator> _communicators;

    /* The request handler. */
    on_request_func _on_request_func;

    /** The handler when accepting new connections. */
    void on_accept(mud::io::tcp::socket&&);
};

server::impl::impl(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _acceptor(_event_loop), _on_request_func(nullptr)
{
    _acceptor.on_accept(
        std::bind(&server::impl::on_accept, this, std::placeholders::_1));
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
server::impl::on_request(on_request_func func)
{
    _on_request_func = func;
}

void
server::impl::on_accept(mud::io::tcp::socket&& socket)
{
    // Add a communicator handling this new connection
    auto comm = _communicators.emplace(_communicators.end(), _on_request_func,
                                       _event_loop);
    comm->open(std::move(socket));

    // Cleanup any disconnected communicators
    for (auto iter = _communicators.begin(); iter != _communicators.end();
         /* iterator moved inside loop */) {
        if (!iter->connected()) {
            iter = _communicators.erase(iter);
        } else {
            ++iter;
        }
    }
}

void
server::impl_deleter::operator()(server::impl* ptr) const
{
    delete ptr;
}

server::server(mud::event::event_loop& event_loop)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(event_loop));
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

void
server::on_request(on_request_func func)
{
    _impl->on_request(func);
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
