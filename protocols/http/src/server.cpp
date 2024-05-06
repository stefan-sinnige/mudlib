#include "mud/http/server.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_HTTP_NS

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

server::impl::~impl() {}

void
server::impl::start(const mud::io::tcp::endpoint& endpoint)
{
    _acceptor.open(endpoint);
}

void
server::impl::stop()
{
    _acceptor.close();
}

void
server::impl::on_request(on_request_func func)
{
    _on_request_func = func;
}

void
server::impl::on_accept(mud::io::tcp::socket&& socket)
{
    auto comm = std::make_shared<io::tcp::communicator>(_event_loop);
    response resp;
    comm->on_receive([comm, resp, this]() mutable {
        try {
            request req;
            comm->istr() >> req;
            if (!comm->istr().fail()) {
                if (_on_request_func != nullptr) {
                    resp = _on_request_func(req);
                }
            }
        } catch (...) {
            resp.field<mud::http::version>(mud::http::version_e::HTTP10);
            resp.field<mud::http::status_code>(
                mud::http::status_code_e::BadRequest);
            resp.field<mud::http::reason_phrase>(
                mud::http::reason_phrase_e::BadRequest);
        }
        comm->ostr() << resp << std::flush;
        comm->close();
        comm.reset();
    });
    comm->open(std::move(socket));
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
