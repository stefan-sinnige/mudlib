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
class streambuf : public std::streambuf
{
public:
    /**
     * Constructor, passing a handle to use.
     * @param [in] handle  The kernel handle to use.
     * @param [in] bufsize The initial buffer size.
     * @param [in] putbacksize The size of the putback buffer.
     */
    streambuf(
            const std::unique_ptr<mud::io::kernel_handle>& handle,
            size_t bufsize = 10,
            size_t putbacksize = 4);

    /**
     * Destructor.
     */
    ~streambuf();

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
    streambuf(const streambuf&) = delete;
    streambuf& operator=(const streambuf&) = delete;

private:
    /**
     * Platform specific implementation.
     */
    void* _impl;
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_STREAMBUF_H_ */

