#include "mud/tls/server.h"
#include <list>
#include <memory>
#include <mutex>

BEGIN_MUDLIB_TLS_NS

class server::communicator : public mud::io::tcp::communicator
{
public:
    /**
     * @brief Constructor.
     * @param[in] event_loop  The event-loop to register the socket to.
     */
    communicator(
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
     * Non-copyable.
     */
    communicator(const communicator&) = delete;
    communicator& operator=(const communicator&) = delete;

private:
    /**
     * Generic TCP receive handler.
     */
    void on_receive();
};

server::communicator::communicator(mud::event::event_loop& event_loop)
  : mud::io::tcp::communicator(event_loop)
{
    mud::io::tcp::communicator::on_receive(
        std::bind(&server::communicator::on_receive, this));
}

void
server::communicator::on_receive()
{
}

/**
 * @brief Implementation of the TLS Server.
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

private:
    /** The event-loop */
    mud::event::event_loop& _event_loop;

    /** The TCP acceptor to listen for incoming connections */
    mud::io::tcp::acceptor _acceptor;

    /** The list of TLS communicators. */
    std::list<server::communicator> _communicators;

    /** The handler when accepting new connections. */
    void on_accept(mud::io::tcp::socket&&);
};

server::impl::impl(mud::event::event_loop& event_loop)
  : _event_loop(event_loop), _acceptor(_event_loop)
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
server::impl::on_accept(mud::io::tcp::socket&& socket)
{
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

END_MUDLIB_TLS_NS

/* vi: set ai ts=4 expandtab: */
