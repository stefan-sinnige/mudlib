#include "mud/io/pipe.h"
#include "mud/core/exception.h"
#include "mud/io/streambuf.h"
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <system_error>
#include <unistd.h>

using namespace std::placeholders;

BEGIN_MUDLIB_IO_NS

/**
 * Define the stream buffer to use with pipes. The read and write operations
 * to be used are the POSIX 'read' and 'write'.
 */

namespace _pipe {

    class streambuf : public mud::io::basic_streambuf
    {
    public:
        /* Constructor for UDP specific stream-buffer.
         * @param [in] handle  The handle to use.
         * @param [in] bufsize The initial buffer size.
         * @param [in] putbacksize The size of the putback buffer.
         */
        streambuf(const std::shared_ptr<mud::core::handle>& handle,
                  size_t bufsize = 10, size_t putbacksize = 4);

        /* Pipe specific read and write functions. */
        ssize_t read(void* buffer, size_t count) override;
        ssize_t write(const void* buffer, size_t count) override;
    };

    streambuf::streambuf(const std::shared_ptr<mud::core::handle>& handle,
                         size_t bufsize, size_t putbacksize)
      : mud::io::basic_streambuf(handle, bufsize, putbacksize)
    {}

    ssize_t streambuf::read(void* buf, size_t count)
    {
        int hndl = mud::core::internal_handle<int>(handle());
        return ::read(hndl, buf, count);
    }

    ssize_t streambuf::write(const void* buf, size_t count)
    {
        int hndl = mud::core::internal_handle<int>(handle());
        return ::write(hndl, buf, count);
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
    const std::shared_ptr<mud::core::handle>& read_handle() const;

    /**
     * The write handle.
     */
    const std::shared_ptr<mud::core::handle>& write_handle() const;

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
    std::shared_ptr<mud::core::handle> _read_handle;

    /** The write handle */
    std::shared_ptr<mud::core::handle> _write_handle;
};

pipe::impl::impl() : _istr(nullptr), _ostr(nullptr)
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
    _read_handle = std::make_shared<mud::core::select_handle>(int(pfd[0]));
    _write_handle = std::make_shared<mud::core::select_handle>(int(pfd[1]));

    /* Logging */
    LOG(log);
    INFO(log) << "Creating read pipe fd: " << pfd[0] << std::endl;
    INFO(log) << "Creating write pipe fd: " << pfd[1] << std::endl;

    /* Create the stream buffers and assign them to the input and output
     * stream objects. */
    _read_buffer =
        std::unique_ptr<_pipe::streambuf>(new _pipe::streambuf(_read_handle));
    _write_buffer =
        std::unique_ptr<_pipe::streambuf>(new _pipe::streambuf(_write_handle));
    _istr.rdbuf(_read_buffer.get());
    _ostr.rdbuf(_write_buffer.get());
}

pipe::impl::~impl()
{
    if (_read_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_read_handle));
    }
    if (_write_handle != nullptr) {
        ::close(mud::core::internal_handle<int>(_write_handle));
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

const std::shared_ptr<mud::core::handle>&
pipe::impl::read_handle() const
{
    return _read_handle;
}

const std::shared_ptr<mud::core::handle>&
pipe::impl::write_handle() const
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

pipe::~pipe() {}

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

const std::shared_ptr<mud::core::handle>&
pipe::read_handle() const
{
    return _impl->read_handle();
}

const std::shared_ptr<mud::core::handle>&
pipe::write_handle() const
{
    return _impl->write_handle();
}

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */
