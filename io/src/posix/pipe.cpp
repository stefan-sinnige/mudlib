#include "mud/core/exception.h"
#include "mud/io/pipe.h"
#include "mud/io/streambuf.h"
#include <functional>
#include <system_error>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std::placeholders;

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with pipes. The read and write operations
 * to be used are the POSIX 'read' and 'write'.
 */

namespace _pipe {

class streambuf: public mud::io::basic_streambuf
{
public:
    /* Constructor for UDP specific stream-buffer.
     * @param [in] handle  The kernel handle to use.
     * @param [in] bufsize The initial buffer size.
     * @param [in] putbacksize The size of the putback buffer.
     */
    streambuf(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            size_t bufsize = 10,
            size_t putbacksize = 4);

    /* Pipe specific read and write functions. */
    ssize_t read(void* buffer, size_t count) override;
    ssize_t write( const void* buffer, size_t count) override;
};

streambuf::streambuf(
        const std::unique_ptr<mud::io::kernel_handle>& handle,
        size_t bufsize,
        size_t putbacksize)
    : mud::io::basic_streambuf(
              handle,
              bufsize,
              putbacksize)
{
}

ssize_t
streambuf::read(void* buf, size_t count)
{
    return ::read(*handle(), buf, count);
}

ssize_t
streambuf::write(const void* buf, size_t count)
{
    return ::write(*handle(), buf, count);
}

} // namespace _pipe

/**
 * @brief Implementation class for a POSIX compliant @c pipe.
 */

class pipe::impl
{
public:
    /**
     * Constructor.
     */
    impl();

    /**
     * Move constructor.
     */
    impl(impl&&);

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The stream for reading.
     */
    std::istream& istr();

    /**
     * The stream for writing.
     */
    std::ostream& ostr();

    /**
     * Read a character.
     */
    char read();

    /**
     * The read handle.
     */
    const std::unique_ptr<mud::io::kernel_handle>& read_handle() const;

    /**
     * The write handle.
     */
    const std::unique_ptr<mud::io::kernel_handle>& write_handle() const;

private:
    /** The stream for reading. */
    std::istream _istr;

    /** The stream for writing. */
    std::ostream _ostr;

    /** The stream buffer for reading */
    std::unique_ptr<_pipe::streambuf> _read_buffer;

    /** The stream buffer for writing */
    std::unique_ptr<_pipe::streambuf> _write_buffer;

    /** The read handle */
    std::unique_ptr<mud::io::kernel_handle> _read_handle;

    /** The write handle */
    std::unique_ptr<mud::io::kernel_handle> _write_handle;
};

pipe::impl::impl()
    : _istr(nullptr), _ostr(nullptr)
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
    _read_handle  = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(int(pfd[0])));
    _write_handle = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(int(pfd[1])));

    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer  = std::unique_ptr<_pipe::streambuf>(
                    new _pipe::streambuf(_read_handle));
    _write_buffer = std::unique_ptr<_pipe::streambuf>(
                    new _pipe::streambuf(_write_handle));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());
}

pipe::impl::~impl()
{
    if (_read_handle != nullptr) {
        ::close(*_read_handle);
        _read_handle.reset(nullptr);
    }
    if (_write_handle != nullptr) {
        ::close(*_write_handle);
        _write_handle.reset(nullptr);
    }
}

std::istream&
pipe::impl::istr()
{
    return _istr;
}

std::ostream&
pipe::impl::ostr()
{
    return _ostr;
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::impl::read_handle()  const
{
    return _read_handle;
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::impl::write_handle()  const
{
    return _write_handle;
}

void
pipe::impl_deleter::operator()(pipe::impl* ptr) const
{
    delete ptr;
}

/** The explicit specialisation for POSIX pipes. */

pipe::pipe()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

pipe::~pipe()
{
}

std::istream&
pipe::istr()
{
    return _impl->istr();
}

std::ostream&
pipe::ostr()
{
    return _impl->ostr();
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::read_handle() const
{
    return _impl->read_handle();
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::write_handle() const
{
    return _impl->write_handle();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

