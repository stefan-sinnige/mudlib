#ifndef _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_
#define _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_

#include <functional>
#include <sstream>
#include <mud/core/handle.h>

namespace mock {

class device
{
public:
    /**
     * @brief Type definition for a callback when something has been received.
     */
    typedef std::function<void()> on_ready_read_func;

    /**
     * @brief Create a device placeholder.
     *
     * @details
     * Create a placeholder for an actual device. The placeholder cannot be
     * used as it would not have been associated with an actual device. Only
     * when the device is moved from an actual device can it be utilised.
     */
    device();

    /**
     * @brief Create a mock device, utilising the provided buffer.
     *
     * @details
     * Create a mock device that uses the external string buffers for its
     * input/output streams.
     *
     * @param istr The buffer that the device uses as an input stream.
     * @param ostr The buffer that the device uses as an output stream.
     * @param recv_signal Signal to notify the device that there is something
     * to read.
     */
    device(std::istream& istr, std::ostream& ostr);

    /**
     * @brief Move constructor.
     *
     * @param other The object to move from.
     */
    device(device&& other);

    /**
     * @brief Move assignment.
     *
     * @param other The object to assign from.
     */
    device& operator=(device&& other);

    /** Destructor */
    virtual ~device();

    /**
     * Non-copyable
     */
    device(const device&) = delete;
    device& operator=(const device&) = delete;

    /**
     * @brief Get the input stream.
     * @return The input stream.
     */
    std::istream& istr();

    /**
     * @brief Get the output stream.
     * @return The output stream.
     */
    std::ostream& ostr();

    /**
     * @brief The mock file-descriptor handle.
     */
    std::shared_ptr<mud::core::handle> handle();

    /**
     * @brief Register a handler when a message is ready to be processed.
     *
     * @details
     * When a message has been received and is ready to be processed by a higher
     * communicator the registered handler will be invoked. 
     *
     * A different mechanism is used here that might be tied to an event-handler
     * which can passively wait for an event using a @c select, @c poll, or any
     * other operating system supported method. This is signified by not using
     * the @c object impulse but a simple callback function.
     *
     * @param func The handler function to register.
     */
    void on_ready_read_cb(on_ready_read_func func);

    /** 
     * @brief Return the handler that is registered when a message is ready to
     * a be processed by a higher communication layer.
     * @return The registered hander.
     */
    on_ready_read_func on_ready_read_cb() const;

    /**
     * @brief Mock that the device has something to read and invoke the
     * @c on_ready_read callback.
     */
    void signal_read();

private:
    /** The implementation and its unique reference. */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_DEVICE_H_ */
