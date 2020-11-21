#include "mud/io/kernel_event_loop.h"
#include "mud/io/pipe.h"
#include <errno.h>
#include <sys/select.h>
#include <mutex>
#include <system_error>
#include <list>

BEGIN_MUDLIB_IO_NS

/**
 * @brief An implementation class of an @c mud::io::kernel_event_loop for POSIX
 * systems.
 *
 * The I/O event-loop for POSIX systems uses the @c select to perform the
 * multiplexing of I/O @c handle's. Intra-pipe activity, like termination
 * requests and re-registration of event handlers, is accomplished through
 * a self-pipe.
 */
class kernel_event_loop::impl
{
public:
    /**
     * Default constructor.
     */
    impl();

    /**
     * Destructor.
     */
    virtual ~impl();

    /**
     * Register an event handler with the loop.
     */
    void register_handler(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            mud::io::kernel_event_loop::event_handler handler);

    /**
     * Deregister an event handler associated to a handle from the loop.
     */
    void deregister_handler(
            const std::unique_ptr<mud::io::kernel_handle>& handle);

    /**
     * Run the loop, waiting for all registered @c handle and invoke the
     * associated @event_handler when needed.
     */
    void loop();

    /*
     * Request to terminate the run @c loop.
     */
    void terminate();

    /**
     * Non-copyable
     */
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

private:
    /*
     * Request to a no-operation. This will effectively stop the @c select
     * and re-multiplex.
     */
    void nop();

    /**
     * Multiplex the registered events into an @fd_set
     */
    void multiplex(
            fd_set& readfds,   /**< [out] The set of file descriptors to wait
                                          for a read. */
            fd_set& writefds,  /**< [out] The set of file descriptors to wait
                                          for a write. */
            fd_set& exceptfds, /**< [out] The set of file descriptors to wait
                                          for an exceptional condition. */
            int& maxfd         /**< [out] The maximum file descriptor handle
                                          number */
    );

    /**
     * Demultiplex the @fd_set and dispatch the events handlers on those that
     * fired.
     */
    void demultiplex(
            const fd_set& readfds,  /**< [out] The set of file descriptors to
                                         wait for a read. */
            const fd_set& writefds, /**< [out] The set of file descriptors to
                                         wait for a write. */
            const fd_set& exceptfds /**< [out] The set of file descriptors to
                                         wait for an exceptional condition. */
    );

    /**
     * Handle commands send to the event-loop from the self-pipe.
     */
    void command_handler();

    /**
     * @brief The event-handler object.
     *
     * This class maintains the ownership of the @c mud::io::event_handler
     * handle when it is used with the event-loop. When there is no event
     * raised, the handle would form part of the multiplexed @c ::select
     * and the event-loop is the owner of the handle. When an event is raised,
     * the @c mud::io::event_handler will temporarily be handed the ownership
     * of the handle until the event callback is finisghed. During the time
     * the callback is invoked, the @c mud::io::kernel_event_loop::impl will not
     * make the handle part of the mulitplexed @c ::select.
     */
    class handler_object
    {
    public:
        /**
         * Constructor, registering the event-handler and getting ownership
         * of the handle.
         */
        handler_object(
                const std::unique_ptr<mud::io::kernel_handle>& handle,
                kernel_event_loop::event_handler handler);

        /**
         * Destructor
         */
        ~handler_object();

        /**
         * Return the reference to the handle.
         */
        const std::unique_ptr<mud::io::kernel_handle>& handle() const;

        /**
         * Invoke the registered event-handler. While the event-handler is
         * run, it will have the ownership of the handle.
         */
        void call();

    private:
        /** The handle */
        const std::unique_ptr<mud::io::kernel_handle>& _handle;

        /** The registered handler */
        kernel_event_loop::event_handler _handler;
    };

    /*
     * Find a handler-object for a particular handle. Returns the iterator
     * to the object in the @c _handlers container, or @c end() if not found.
     */
    std::list<handler_object>::iterator find(
            const std::unique_ptr<mud::io::kernel_handle>& handle);

    /**
     * Internal commands.
     */
    typedef unsigned char command_type;
    const command_type NOP          = '\0';
    const command_type TERMINATION  = 'T';

    /** Command to execute */
    command_type _cmd;

    /** Self-pipe for internal communications. */
    mud::io::pipe _self;

    /** Registered event handlers. */
    std::list<handler_object> _handlers;

    /** Flag to indicate if the loop is running. */
    std::atomic<bool> _running;
};

kernel_event_loop::impl::impl()
    : _cmd(NOP), _running(false)
{
    /* Always register the self-pipe */
    _handlers.push_back(handler_object(
                    _self.read_handle(),
                    std::bind(&kernel_event_loop::impl::command_handler, this)));
}

kernel_event_loop::impl::~impl()
{
    /* Make sure the loop is terminated */
    terminate();

    /* Spin-lock until the running flag is false */
    while (_running.load() == true);
}

void
kernel_event_loop::impl::register_handler(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        mud::io::kernel_event_loop::event_handler handler)
{
    std::list<handler_object>::iterator found = find(handle);
    if (found != _handlers.end())
    {
        _handlers.erase(found);
    }
    _handlers.push_back(handler_object(handle, handler));
    nop();
}

void
kernel_event_loop::impl::deregister_handler(
        const std::unique_ptr<mud::io::kernel_handle>& handle)
{
    std::list<handler_object>::iterator found = find(handle);
    if (found != _handlers.end())
    {
        _handlers.erase(found);
        nop();
    }
}

void
kernel_event_loop::impl::loop()
{
    /* If we're already running, bail out. */
    if (_running.exchange(true) == true)
    {
        return;
    }

    /* Loop until we're told to stop */
    while (_cmd != TERMINATION)
    {
        fd_set readfds, writefds, exceptfds;
        int maxfd;

        multiplex(readfds, writefds, exceptfds, maxfd);
        if (::select(maxfd+1, &readfds, &writefds, &exceptfds, nullptr) == -1)
        {
            throw std::system_error(errno, std::system_category(), "select");
        }
        demultiplex(readfds, writefds, exceptfds);
    }

    /* Reset the command such that we can re-use the loop again. */
    _cmd = NOP;
    _running.store(false);
}

void
kernel_event_loop::impl::terminate()
{
    _self.ostr() << TERMINATION << std::flush;
}

void
kernel_event_loop::impl::nop()
{
    _self.ostr() << NOP << std::flush;
}

void
kernel_event_loop::impl::multiplex(
        fd_set& readfds,
        fd_set& writefds,
        fd_set& exceptfds,
        int& maxfd)
{
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    maxfd = -1;

    /* @TODO determine if Read/Write/Except is needed */

    /* Add all registered event handlers */
    for (auto& obj: _handlers)
    {
        if (obj.handle() != nullptr)
        {
            FD_SET(*(obj.handle()), &readfds);
            if (*(obj.handle()) > maxfd) {
                maxfd = *(obj.handle());
            }
        }
    }
}

void
kernel_event_loop::impl::demultiplex(
        const fd_set& readfds,
        const fd_set& writefds,
        const fd_set& exceptfds)
{
    /* Check all excepts */

    /* Check all reads */
    for (auto& obj: _handlers)
    {
        if (obj.handle() != nullptr)
        {
            if (FD_ISSET(*(obj.handle()), &readfds))
            {
                obj.call();
            }
        }
    }

    /* Check all writes */
}

void
kernel_event_loop::impl::command_handler()
{
    command_type cmd;
    _self.istr() >> cmd;
    _cmd = cmd;
}

std::list<kernel_event_loop::impl::handler_object>::iterator
kernel_event_loop::impl::find(const std::unique_ptr<mud::io::kernel_handle>&
        handle)
{
    return std::find_if(
                    _handlers.begin(), _handlers.end(),
    [&handle](const handler_object& x) {
        return x.handle() == handle;
    });
}

/** The implementation of the handler-object. */

kernel_event_loop::impl::handler_object::handler_object(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        mud::io::kernel_event_loop::event_handler handler)
    : _handle(handle), _handler(handler)
{
}

kernel_event_loop::impl::handler_object::~handler_object()
{
}

const std::unique_ptr<mud::io::kernel_handle>&
kernel_event_loop::impl::handler_object::handle() const
{
    return _handle;
}

void
kernel_event_loop::impl::handler_object::call()
{
    _handler();
}

/** The explicit implementation for POSIX event loops. */

kernel_event_loop::kernel_event_loop()
{
    _impl = std::unique_ptr<impl>(new impl());
}

kernel_event_loop::~kernel_event_loop()
{
}

void
kernel_event_loop::register_handler(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        mud::io::kernel_event_loop::event_handler handler)
{
    _impl->register_handler(handle, handler);
}

void
kernel_event_loop::deregister_handler(
        const std::unique_ptr<mud::io::kernel_handle>& handle)
{
    _impl->deregister_handler(handle);
}

void
kernel_event_loop::loop()
{
    _impl->loop();
}

void
kernel_event_loop::terminate()
{
    _impl->terminate();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

