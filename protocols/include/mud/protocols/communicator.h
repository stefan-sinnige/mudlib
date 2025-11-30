#ifndef _MUDLIB_PROTOCOLS_COMMUNICATOR_H_
#define _MUDLIB_PROTOCOLS_COMMUNICATOR_H_

#include <functional>
#include <iostream>
#include <mud/protocols/ns.h>
#include <mud/core/event.h>
#include <mud/core/object.h>
#include <mud/core/impulse.h>

BEGIN_MUDLIB_PROTOCOLS_NS

/**
 * @brief The abstact declaration of a communication protocol.
 *
 * @details
 * The definition of the methods that forms a communicator and that would need
 * to be defined in its implementation to cater for specific circumstances
 * related to the protocol it implements and the underlying physical device.
 *
 * The @c communication protocol class defines the minimum functionality that a
 * specific implementation would require. This includes the following concepts:
 *
 *    * Status of the connection or device (for example, open or closed). Any
 *      attaempt to communicate on a closed connection or device would normally
 *      fail.
 *    * Impulses on state changes (connected or disconneted) and if data is
 *      available to be received
 *
 * Commnicators can be layered, similar to the networking OSI layering model,
 * such that all protocol handling of a communication protocol is accomplished
 * by one layer only and will not have any bearing on any lower or higher
 * communication protocol. For example, the HTTP protocol may be using a TLS
 * layer as a lower layer to implements the secure HTTPS protocol, or it may
 * have the TCP communication layer to implement the insecure HTTP protocol.
 * The HTTP communication layer does not require to know what layer it at a
 * lower level as it directly communicates with the abstract @c communication
 * protocol, either the TLS or TCP communication layers.
 *
 * Layering can be implemented directly at the construction level and the
 * constructor which will (by default) setup the impulse from the lower 
 * communicator to their associated handler. These handlers can be overridden
 * by the implementations, or otherwise they will merely trigger the same
 * impulse themselves.
 */
template<typename Device>
class MUDLIB_PROTOCOLS_API communicator: public mud::core::object
{
public:
    /**
     * @brief The type of the @c impulse when something has been received on
     * the device.
     */
    typedef std::shared_ptr<mud::core::impulse<Device&>>
            receive_impulse_type;

    /**
     * @brief The type of the @c impulse when the device or a lower @c 
     * communication signals that the connection is closed.
     */
    typedef std::shared_ptr<mud::core::impulse<Device&>>
            disconnect_impulse_type;

    /**
     * @brief The type of the @c impulse when the device or a lower @c 
     * communication signals that the connection is opened.
     */
    typedef std::shared_ptr<mud::core::impulse<Device&>>
            connect_impulse_type;

    /**
     * @brief Construct a communicator.
     */
    communicator() {
        _receive_impulse = std::make_shared<mud::core::impulse<Device&>>();
        _connect_impulse = std::make_shared<mud::core::impulse<Device&>>();
        _disconnect_impulse = std::make_shared<mud::core::impulse<Device&>>();
    }

    /**
     * @brief Construct a communicator by moving the contents from another
     * communicator.
     *
     * @param other The communicator to move the contents from.
     */
    communicator(communicator&& other) = default;

    /**
     * @brief Initialise a communicator by moving the contents from another
     * communicator.
     *
     * @param other The communicator to move the contents from.
     * @return Reference to this object.
     */
    communicator& operator=(communicator&& other) = default;

    /**
     * @brief Non-copyable.
     */
    communicator(const communicator&) = delete;

    /**
     * @brief Non-assignable.
     */
    communicator& operator=(const communicator&) = delete;

    /**
     * Destructor.
     */
    virtual ~communicator() = default;

    /**
     * @brief Open the communicator with the associated device.
     *
     * @details
     * The communication channel can commence on the specified device. The
     * communicator will become the sole owner of the device.
     *
     * @param device The communication device to use in communication.
     */
    virtual void open(Device&& device) = 0;

    /**
     * @brief Close the communicator.
     *
     * @details
     * The communication channel is to be closed. Depending on the protocol's
     * representation that this @c communicator implements, it may only close
     * the communication on this particular channel. For example, if this
     * channel is an encryption layer, it may use a close/open sequence to
     * establish a new encryption algorithm or key, while keeping the layers
     * on a lower or upper level intact.
     */
    virtual void close() = 0;

    /**
     * @brief Get the stream object to read from the communicator.
     * @return The stream object.
     */
    virtual std::istream& istr() = 0;
    
    /**
     * @brief Get the stream object to write to the communicator.
     * @return The stream object.
     */
    virtual std::ostream& ostr() = 0;

    /**
     * @brief Return the underlying device.
     *
     * @details
     * Return the underlying device, but this is only relevant after the
     * communicator has been opened.
     *
     * @return Return the underlying device.
     */
    virtual Device& device() = 0;

    /**
     * @brief Return the event that should be actioned when the @c Device is
     * being signalled.
     *
     * @details
     * The event that is returned is to be used by an @c event_loop to invoke
     * the @c on_receive handlers on the communicator. As only the @c
     * end_communicator will have direct access to the underlying @c Device,
     * the action will only trigger the @c end_communicator. Any layered
     * communicator on a higher protocol layer will then be invoked as
     * necessary, based on the message being received.
     *
     * @return The event.
     */
    virtual const mud::core::event& event() const = 0;

    /**
     * @brief The impulse when something is ready to be processed by a higher
     * communication layer.
     *
     * @details
     * When a message has been received and is ready to be proceesed by a higher
     * communicator (ie when an enveloped message has been received), the @c
     * receive_impulse will be invoked. This is usually only used internally
     * and set-up by the communication layer, the end-communicator and the
     * device.
     */
    receive_impulse_type receive_impulse() {
        return _receive_impulse;
    }

    /**
     * @brief The impulse when the communication layer (or the underlying
     * device) has been connected.
     *
     * @details
     * When the communication layer is connected to the peer, the connecting
     * impulse a triggered such that the communication layer can take
     * additional action.
     */
    connect_impulse_type connect_impulse() {
        return _connect_impulse;
    }

    /**
     * @brief The impulse when the communication layer (or the underlying
     * device) has been disconnected.
     *
     * @details
     * When the communication layer is no longer connected to the peer, the
     * disconnecting impulse a triggered such that the communication layer can
     * take appropriate action.
     */
    disconnect_impulse_type disconnect_impulse() {
        return _disconnect_impulse;
    }

protected:
    /**
     * @brief Handle the message of a lower communication layer.
     *
     * @details
     * Process a message as triggered by a lower commuication layer. The lower
     * layer will process its own layer specific communication and will trigger
     * a higher layer if this is called for by its communication protocol.
     *
     * This handler is usually attached to the impulse of a lower communicator.
     * The default implementation is to signal the @c receive_impulse.
     *
     * @param device The device that has something to read. The implementation
     * will usually ignore this parameter and rely on the streaming operators
     * instead to communicate with the device.
     */
    virtual void on_receive(Device& device) {
        _receive_impulse->pulse(device);
    }

    /**
     * @brief Handle the connection of a lower communication layer.
     *
     * @details
     * Process the signal that the lower-level communication layer has detected
     * the the connection is connected.
     *
     * The default implementation is to signal the @c connect_impulse.
     *
     * @param device The device that is connected.
     */
    virtual void on_connect(Device& device) {
        _connect_impulse->pulse(device);
    }

    /**
     * @brief Handle the disconnection of a lower communication layer.
     *
     * @details
     * Process the signal that the lower-level communication layer has detected
     * the the connection is closed.
     *
     * The default implementation is to signal the @c disconnect_impulse.
     *
     * @param device The device that has closed.
     */
    virtual void on_disconnect(Device& device) {
        _disconnect_impulse->pulse(device);
    }

private:
    /** The receive impulse. */
    receive_impulse_type _receive_impulse;

    /** The connect impulse. */
    connect_impulse_type _connect_impulse;

    /** The disconnect impulse. */
    disconnect_impulse_type _disconnect_impulse;
};

/**
 * @brief A communicator at the end of the layers.
 *
 * @details
 * A communicator that is created at the lowest layer is not built upon any
 * other @c communicator and is therefore interacting with the communication
 * device at the lowest level.
 *
 * A pure TCP or UDP communicator are examples of an @c end_communicator as
 * they interact directly with their socket communication device without any
 * definition of a protocol.
 */
template<typename Device>
class MUDLIB_PROTOCOLS_API end_communicator: public communicator<Device>
{
};

/**
 * @brief A communicator part of a layer.
 *
 * @details
 * A communicator that is built upon another communicator at a lower level
 * will allow the concept of a protocol layer. The uppper layer would be
 * utilising the streaming functionality of the lower layer.
 *
 * Any communicator the is not an @c end_communicator would be a layered one.
 */
template<typename Device>
class MUDLIB_PROTOCOLS_API layered_communicator: public communicator<Device>
{
public:
    /**
     * @brief Construct a communicator that is built upon another communicator.
     *
     * @details
     * A communicator that is built upon another communicator at a lower level
     * will allow the concept of a protocol layer. The uppper layer would be
     * utilising the streaming functionality of the lower layer.
     *
     * The impulses of the lower layer will be tied to the handlers of this
     * communication layer. This allows any messages of the lower layer that
     * need to be processed by this upper layer to be handled after
     * the lower layer has performed its operation (ie after removing the
     * envelope layer or after any crytptographic operation).
     *
     * @param lower The communicator on the next (lower) level.
     */
    layered_communicator(communicator<Device>& lower) : _lower(lower) {
        _lower.receive_impulse()->attach(this,
                &layered_communicator<Device>::on_receive);
        _lower.connect_impulse()->attach(this,
                &layered_communicator<Device>::on_connect);
        _lower.disconnect_impulse()->attach(this,
                &layered_communicator<Device>::on_disconnect);
    }

    /**
     * @brief Construct a communicator by moving the contents from another
     * communicator.
     *
     * @param other The communicator to move the contents from.
     */
    layered_communicator(layered_communicator&& other) = default;

    /**
     * @brief Initialise a communicator by moving the contents from another
     * communicator.
     *
     * @param other The communicator to move the contents from.
     * @return Reference to this object.
     */
    layered_communicator& operator=(layered_communicator&& other) = default;

    /**
     * @brief Open the communicator with the associated device.
     *
     * @details
     * The communication channel can commence on the specified device. The
     * communicator will become the sole owner of the device.
     *
     * @param device The communication device to use in communication.
     */
    virtual void open(Device&& device) override {
        _lower.open(std::move(device));
    }

    /**
     * @brief Close the communicator.
     *
     * @details
     * The communication channel is to be closed. Depending on the protocol's
     * representation that this @c communicator implements, it may only close
     * the communication on this particular channel. For example, if this
     * channel is an encryption layer, it may use a close/open sequence to
     * establish a new encryption algorithm or key, while keeping the layers
     * on a lower or upper level intact.
     *
     * The default implementation is to close the lower layer.
     */
    virtual void close() override {
        _lower.close();
    }

    /**
     * @brief Get the stream object to read from the communicator.
     *
     * @details
     * This default implementation is merley a pass-through to the lower layer.
     * A communication protocol are likely to define its own definition, to
     * implement protocol specific messages (like message envelopes and
     * cryptographics operations).
     *
     * @return The stream object.
     */
    virtual std::istream& istr() override {
        return _lower.istr();
    }
    
    /**
     * @brief Get the stream object to write to the communicator.
     *
     * @details
     * This default implementation is merley a pass-through to the lower layer.
     * A communication protocol are likely to define its own definition, to
     * implement protocol specific messages (like message envelopes and
     * cryptographics operations).
     *
     * @return The stream object.
     */
    virtual std::ostream& ostr() override {
        return _lower.ostr();
    }

    /**
     * @brief Get the lower communicator.
     *
     * @details
     * Get access to the communicator at the next (lower) layer.
     *
     * @return The lower communicator.
     */
    communicator<Device>& lower() const { return _lower; }

    /**
     * @brief Return the underlying device.
     *
     * @details
     * Return the underlying device, but this is only relevant after the
     * communicator has been opened.
     *
     * @return Return the underlying device.
     */
    virtual Device& device() override {
        return _lower.device();
    }

    /**
     * @brief Return the event that should be actioned when the @c Device is
     * being signalled.
     *
     * @details
     * The event that is returned is to be used by an @c event_loop to invoke
     * the communication handlers for this device. As only the @c
     * end_communicator will have direct access to the underlying @c Device,
     * the action will only trigger the @c end_communicator. Any layered
     * communicator on a higher protocol layer will then be invoked as
     * necessary, based on the message being received.
     *
     * @return The event.
     */
    virtual const mud::core::event& event() const override {
        return _lower.event();
    }
private:
    /**
     * The next communication at a lower level.
     */
    communicator<Device>& _lower;
};

END_MUDLIB_PROTOCOLS_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_PROTOCOLS_COMMUNICATOR_H_ */
