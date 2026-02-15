/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#ifndef _MUDLIB_CORE_EVENT_H_
#define _MUDLIB_CORE_EVENT_H_

#include <memory>
#include <mud/core/handle.h>
#include <mud/core/uuid.h>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief An event that can be used in an event-loop.
 *
 * @details
 * An event is an abstraction of anything that can signal a certain condition.
 * This can be any externally triggered action (e.g. the moving of a mouse, the
 * receiving of a web-call from another machine), or some condition internally
 * (a timer or a file being updated).
 *
 * Events are designed to be used by the @c event_loop such that it can be
 * waited upon and when the condition is met, the event shall be triggered using
 * the publish/subscribe notification mechanism. Although this can allow
 * multiple subscribers to be notified, it is expected that only one would
 * actually access the resource and take appropriate action. For example, when
 * a TCP socket is triggered that there is some data available on the socket, it
 * is expected that only subscriber to that event will handle the reading of the
 * socket.
 *
 * As the event is associated to the publish-subscribe mechanism, a particular
 * event for a certain resource will be identified by its @em topic
 * identification.
 *
 * The event will remain active in the loop as long as there are active
 * subscribers attached to the event. When there are no subscribers attached,
 * the event-loop shall remove the event.
 */
class MUDLIB_CORE_API event
{
public:
    /**
     * @brief The signals to react upon. Multiple signals can be provided using
     * bitwise OR'ing.
     */
    enum class signal_type : int32_t
    {
        NONE = 0,           /**< No mask */
        READY = ~0,         /**< Any event (non-descriptive) */
        READING = (1 << 1), /**< Reading */
        WRITING = (1 << 2)  /**< Writing */
    };

    /**
     * @brief Create a default, yet unusable, event.
     */
    event() : _topic(true) {}

    /**
     * @brief Constructor.
     * @param topic [in] The topic identification associated to this event.
     * @param handle [in] The handle to a resource.
     * @param mask [in] The signal mask to register.
     * @details
     * Create an event that is associated to a resource handle and notifies
     * and subscriber through the notification using the @c topic ID.
     */
    event(const mud::core::uuid& topic, 
          std::shared_ptr<mud::core::handle> handle,
          signal_type mask);

    /**
     * @brief Copy constructor.
     * @param rhs The event to copy.
     */
    event(const event& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The event to move.
     * @details
     * Functionally, the same as the copy constructor. The @c rhs remain
     * unaffected.
     */
    event(event&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~event() = default;

    /**
     * @brief Copy assignment.
     * @param rhs The event to copy.
     */
    event& operator=(const event& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The event to move.
     * @details
     * Functionally, the same as the copy constructor. The @c rhs remain
     * unaffected.
     */
    event& operator=(event&& rhs);

    /**
     * @brief Equality operator.
     * @param[in] rhs The event to compare against (by ID).
     */
    bool operator==(const event& rhs) const;

    /**
     * @brief Inequality operator.
     * @param[in] rhs The event to compare against (by ID).
     */
    bool operator!=(const event& rhs) const;

    /**
     * @brief The event topic ID.
     */
    const mud::core::uuid& topic() const;

    /**
     * @brief Set the handle to examine.
     * @param handle The handle to set.
     */
    void handle(std::shared_ptr<mud::core::handle> handle);

    /**
     * @brief Return the handle to examine.
     */
    std::shared_ptr<mud::core::handle> handle() const;

    /**
     * @brief The signal mask.
     */
    signal_type mask() const;

    /**
     * @brief Publish a notification that the event has triggered.
     */
    void publish() const;

private:
    /** The topic ID. */
    mud::core::uuid _topic;

    /** The handle to examine. */
    std::shared_ptr<mud::core::handle> _handle;

    /** The signal mask to register */
    signal_type _mask;
};

/**
 * Bit-wise OR of event signals.
 */
event::signal_type
operator|(event::signal_type lhs, event::signal_type rhs);

/**
 * Bit-wise AND of event signals.
 */
event::signal_type
operator&(event::signal_type lhs, event::signal_type rhs);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_EVENT_H_ */
