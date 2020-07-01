#include "mud/core/exception.h"
#include "mud/io/pipe.h"
#include "mud/io/streambuf.h"
#include <system_error>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with pipes. The read and write operations
 * to be used are the POSIX 'read' and 'write'.
 */

class pipe_read
{
public:
    ssize_t
    operator()(int fd, void* buf, size_t count) {
        return ::read(fd, buf, count);
    }
};

class pipe_write
{
public:
    ssize_t
    operator()(int fd, const void* buf, size_t count) {
        return ::write(fd, buf, count);
    }
};

typedef mud::io::basic_streambuf<pipe_read, pipe_write> pipe_streambuf;

/**
 * @brief Implementation class for a POSIX compliant @c pipe.
 */

class posix_pipe
{
public:
    /**
     * Constructor.
     */
    posix_pipe();

    /**
     * Destructor.
     */
    ~posix_pipe();

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
    std::unique_ptr<pipe_streambuf> _read_buffer;

    /** The stream buffer for writing */
    std::unique_ptr<pipe_streambuf> _write_buffer;

    /** The read handle */
    std::unique_ptr<mud::io::kernel_handle> _read_handle;

    /** The write handle */
    std::unique_ptr<mud::io::kernel_handle> _write_handle;
};

posix_pipe::posix_pipe()
    : _istr(nullptr), _ostr(nullptr)
{
    /* Create the pipe */
    int pfd[2];
    if (::pipe(pfd) == -1) {
        throw std::system_error(errno, std::system_category(), "creating pipe");
    }
    if (::fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1) {
        throw std::system_error(errno, std::system_category(),
                "set pipe non-blocking");
    }
    /* Ensure non-blocking I/O */

    /* Set the ownership of the pipe handles */
    _read_handle  = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(int(pfd[0])));
    _write_handle = std::unique_ptr<mud::io::kernel_handle>(
                    new mud::io::kernel_handle(int(pfd[1])));

    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer  = std::unique_ptr<pipe_streambuf>(
                    new pipe_streambuf(_read_handle));
    _write_buffer = std::unique_ptr<pipe_streambuf>(
                    new pipe_streambuf(_write_handle));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());
}

posix_pipe::~posix_pipe()
{
    if (_read_handle != nullptr) {
        ::close(*_read_handle);
    }
    if (_write_handle != nullptr) {
        ::close(*_write_handle);
    }
}

std::istream&
posix_pipe::istr()
{
    return _istr;
}

std::ostream&
posix_pipe::ostr()
{
    return _ostr;
}

const std::unique_ptr<mud::io::kernel_handle>&
posix_pipe::read_handle()  const
{
    return _read_handle;
}

const std::unique_ptr<mud::io::kernel_handle>&
posix_pipe::write_handle()  const
{
    return _write_handle;
}

/** The explicit specialisation for POSIX pipes. */
pipe::pipe()
{
    _impl = new posix_pipe();
}

pipe::~pipe()
{
    delete static_cast<posix_pipe*>(_impl);
}

std::istream&
pipe::istr()
{
    return static_cast<posix_pipe*>(_impl)->istr();
}

std::ostream&
pipe::ostr()
{
    return static_cast<posix_pipe*>(_impl)->ostr();
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::read_handle() const
{
    return static_cast<posix_pipe*>(_impl)->read_handle();
}

const std::unique_ptr<mud::io::kernel_handle>&
pipe::write_handle() const
{
    return static_cast<posix_pipe*>(_impl)->write_handle();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

