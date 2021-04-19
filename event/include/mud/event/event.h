#ifndef _MUDLIB_EVENT_EVENT_H_
#define _MUDLIB_EVENT_EVENT_H_

#include <functional>
#include <memory>
#include <mud/event/ns.h>
#include <mud/core/handle.h>

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief A event that can be signalled.
 *
 * An event is an abstraction of anything that can signal a certain condition.
 * This can be any externally triggered action (e.g. the moving of a mouse, the
 * receiving of a web-call from another machine), or some condition internally
 * (a timer or a file being updated).
 *
 * Events can be waited upon by an @c event_loop and an associated handler can
 * be called when such an event has been triggered.
 */
class MUDLIB_EVENT_API event
{
public:
    /**
     * Definition for an event-handling routine.
     */
    typedef std::function<void(event&)> handler_fn;

    /**
     * The signals to react upon. Multiple signals can be provided using bit-
     * wise OR'ing.
     */
    enum class signal_t : int32_t
    {
        NONE     = 0,         /**< No mask */
        READY    = ~0,        /**< Any event (non-descriptive) */
        READING  = (1 <<  1), /**< Reading */
        WRITING  = (1 <<  2)  /**< Writing */
    };

    /**
     * @brief Constructor, defining an event, the signal and the handler.
     * @param handle [in] The handle to examine.
     * @param handler [in] The handler to invoke.
     * @param mask [in] The signal mask to register.
     */
    event(const std::unique_ptr<mud::core::handle>& handle,
            handler_fn handler, signal_t mask);

    /**
     * @brief Constructor for lookup purposes.
     * @param handle [in] The handle to look up.
     */
    event(const std::unique_ptr<mud::core::handle>& handle);

    /**
     * @brief Copy constructor.
     * @param[in] rhs The event to copy.
     */
    event(const event& rhs);

    /**
     * Destructor.
     */
    virtual ~event();

    /**
     * Equality operator, based on the handle only.
     * @param[in] rhs The event to compare against.
     */
    bool operator==(const event& rhs) const;

    /**
     * Inequality operator, based on the handle only.
     * @param[in] rhs The event to compare against.
     */
    bool operator!=(const event& rhs) const;

    /**
     * @brief Return the handle to examine.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * The signal mask.
     */
    signal_t mask() const;

    /**
     * @brief Call the event handler.
     */
    void call();

    /**
     * Not assignable.
     */
    event& operator=(const event& rhs) = delete;
private:
    /** The handle to examine. */
    const std::unique_ptr<mud::core::handle>& _handle;

    /** The handler to call when the event has fired. */
    handler_fn _handler;

    /** The signal mask to register */
    signal_t _mask;
};

/**
 * Bit-wise OR of event signals.
 */
event::signal_t operator|(event::signal_t lhs, event::signal_t rhs);

/**
 * Bit-wise AND of event signals.
 */
event::signal_t operator&(event::signal_t lhs, event::signal_t rhs);

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_EVENT_H_ */
