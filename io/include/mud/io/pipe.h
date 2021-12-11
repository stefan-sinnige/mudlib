#ifndef _MUDLIB_IO_PIPE_H_
#define _MUDLIB_IO_PIPE_H_

#include <istream>
#include <memory>
#include <mud/core/handle.h>
#include <mud/io/ns.h>
#include <ostream>

BEGIN_MUDLIB_IO_NS

/**
 * @brief Class that defines a uni-directional data channel.
 *
 * A uni-directional pipe has two ends: one that writes data at one end (also
 * known as the source) and the other end that reads the data (also known as
 * the sink).
 */
class MUDLIB_IO_API pipe
{
public:
    /**
     * @brief Default constructor.
     *
     * Creating a pipe with a read and a write handle
     */
    pipe();

    /**
     * @brief Move constructor.
     *
     * Move the implementation of the pipe to this object.
     */
    pipe(pipe&&) = default;

    /**
     * @brief Destructor.
     */
    virtual ~pipe();

    /**
     * @brief Get the stream object to read from the pipe.
     * @return The stream object.
     */
    std::istream& istr();

    /**
     * @brief Get the stream object to write to the pipe.
     * @return The stream object.
     */
    std::ostream& ostr();

    /**
     * The handle associated to the read end (sink) of the pipe.
     * @return The handle to the resource.
     */
    const std::unique_ptr<mud::core::handle>& read_handle() const;

    /**
     * The handle associated to the write end (source) of the pipe.
     * @return The handle to the resource.
     */
    const std::unique_ptr<mud::core::handle>& write_handle() const;

    /**
     * Non-copyable
     */
    pipe(const pipe&) = delete;
    pipe& operator=(const pipe&) = delete;

private:
    /**
     * Platform specific implementation.
     */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_PIPE_H_ */
