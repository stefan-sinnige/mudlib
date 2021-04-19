#include "posix/select_self.h"
#include <system_error>
#include <fcntl.h>
#include <unistd.h>

BEGIN_MUDLIB_EVENT_NS

/**
 * Implementation of a @select_self signalling resource.
 */
class select_self::impl
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The handle.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * Send a signal to the resource.
     */
    void send();

    /**
     * Receive a signal.
     */
    void receive();

private:
    /** The read handle */
    std::unique_ptr<mud::core::handle> _read_handle;

    /** The write handle */
    std::unique_ptr<mud::core::handle> _write_handle;
};

void
select_self::impl_deleter::operator()(select_self::impl* ptr) const
{
    delete ptr;
}

select_self::impl::impl()
{
    /* Create the pipe */
    int pfd[2];
    if (::pipe(pfd) == -1) {
        throw std::system_error(errno, std::system_category(), "creating pipe");
    }

    /* Ensure non-blocking I/O */
    if (::fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1) {
        throw std::system_error(errno, std::system_category(),
                "set pipe non-blocking");
    }

    /* Set the ownership of the pipe handles */
    _read_handle  = std::unique_ptr<mud::core::handle>(
                    new mud::core::int_handle(int(pfd[0])));
    _write_handle = std::unique_ptr<mud::core::handle>(
                    new mud::core::int_handle(int(pfd[1])));
}

select_self::impl::~impl()
{
    if (_read_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_read_handle));
        _read_handle.reset(nullptr);
    }
    if (_write_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_write_handle));
        _write_handle.reset(nullptr);
    }
}

const std::unique_ptr<mud::core::handle>&
select_self::impl::handle() const
{
    return _read_handle;
}

void
select_self::impl::send()
{
    char ch = 'N';
    (void) ::write(mud::core::internal_handle<int>(_write_handle), &ch, 1);
}

void
select_self::impl::receive()
{
    char ch;
    (void) ::read(mud::core::internal_handle<int>(_read_handle), &ch, 1);
}

/** The explicit implementation for self signalling resources. */

select_self::select_self()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

select_self::~select_self()
{
}

const std::unique_ptr<mud::core::handle>&
select_self::handle() const
{
    return _impl->handle();
}

void
select_self::send()
{
    _impl->send();
}

void
select_self::receive()
{
    _impl->receive();
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
