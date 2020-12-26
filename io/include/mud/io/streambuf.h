#ifndef _MUDLIB_IO_STREAMBUF_H_
#define _MUDLIB_IO_STREAMBUF_H_

#include <mud/io/ns.h>
#include <mud/io/kernel_handle.h>
#include <memory>
#include <streambuf>

BEGIN_MUDLIB_IO_NS

/**
 * @brief Stream buffer for generic I/O operations.
 *
 * This class defines the stream buffer for generic (but system implementation
 * dependent) I/O operations.
 */
class basic_streambuf : public std::streambuf
{
public:
    /**
     * Constructor, passing a handle to use.
     * @param [in] handle  The kernel handle to use.
     * @param [in] bufsize The initial buffer size.
     * @param [in] putbacksize The size of the putback buffer.
     */
    basic_streambuf(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            size_t bufsize = 10,
            size_t putbacksize = 4);

    /**
     * Destructor.
     */
    virtual ~basic_streambuf();

    /**
     * Read new characters into the buffer.
     */
    virtual int underflow();

    /**
     * Write the characters from the buffer.
     */
    virtual int overflow(int c);

    /**
     * Synchronise data.
     */
    virtual int sync();

    /**
     * Non-copyable.
     */
    basic_streambuf(const basic_streambuf&) = delete;
    basic_streambuf& operator=(const basic_streambuf&) = delete;

protected:
    /**
     * Low-level buffer reading and writing. This can be device specific.
     */
    virtual ssize_t read(void* buffer, size_t count) = 0;
    virtual ssize_t write(const void* buffer, size_t count) = 0;

    /**
     * Return to the resource handle.
     */
    const std::unique_ptr<mud::io::kernel_handle>& handle() const;

private:
    const std::unique_ptr<mud::io::kernel_handle>& _handle; /**< Handle */
    const size_t _bufsize;     /**< Size of the buffer */
    const size_t _putbacksize; /**< Size of the putback buffer */
    char* _buffer;             /**< The buffer. */
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_STREAMBUF_H_ */

