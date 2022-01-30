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
    void start(const std::string& host, uint16_t port);

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
server::impl::start(const std::string& host, uint16_t port)
{
    _acceptor.open(io::tcp::endpoint(host, port));
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
    comm->on_receive([comm, this]() mutable {
        try {
            message request;
            comm->istr() >> request;
            if (!comm->istr().fail()) {
                if (_on_request_func != nullptr) {
                    message response = _on_request_func(request);
                    comm->ostr() << response << std::flush;
                }
            }
        } catch (...) {
            message response;
            response.type(mud::http::message::type_t::RESPONSE);
            response.field<mud::http::version>(mud::http::version::HTTP10);
            response.field<mud::http::status_code>(
                mud::http::status_code::BadRequest);
            response.field<mud::http::reason_phrase>(
                mud::http::reason_phrase::BadRequest);
            comm->ostr() << response << std::flush;
        }
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
server::start(const std::string& host, uint16_t port)
{
    _impl->start(host, port);
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
