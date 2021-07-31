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
     * @brief The signals to react upon. Multiple signals can be provided using
     * bitwise OR'ing.
     */
    enum class signal_type : int32_t
    {
        NONE     = 0,         /**< No mask */
        READY    = ~0,        /**< Any event (non-descriptive) */
        READING  = (1 <<  1), /**< Reading */
        WRITING  = (1 <<  2)  /**< Writing */
    };

    /**
     * @brief Return type of the event function.
     */
    enum class return_type
    {
        REMOVE   = 0,         /**< Remove the registration */
        CONTINUE = 1          /**< Continue with same registration */
    };

    /**
     * @brief Type definition of the event function
     */
    typedef std::function<return_type(void)> function_type;

    /**
     * @brief Constructor, defining an event, the signal and the handler.
     * @param handle [in] The handle to examine.
     * @param mask [in] The signal mask to register.
     * @param handler [in] The callable handler function
     */
    event(const std::unique_ptr<mud::core::handle>& handle, signal_type mask,
            function_type&& handler);

    /**
     * @brief Constructor for lookup purposes only.
     * @param handle [in] The handle to look up.
     */
    event(const std::unique_ptr<mud::core::handle>& handle);

    /**
     * @brief Copy constructor.
     */
    event(const event& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~event();

    /**
     * @brief Equality operator, based on the handle only.
     * @param[in] rhs The event to compare against.
     */
    bool operator==(const event& rhs) const;

    /**
     * @brief Inequality operator, based on the handle only.
     * @param[in] rhs The event to compare against.
     */
    bool operator!=(const event& rhs) const;

    /**
     * @brief Return the handle to examine.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * @brief The signal mask.
     */
    signal_type mask() const;

    /**
     * @brief The function handler.
     */
    function_type handler() const;

    /**
     * Not copy-assignable
     */
    event& operator=(const event& rhs) = delete;

    /**
     * Not moveable
     */
    event(event&& rhs) = delete;
    event& operator=(event&& rhs) = delete;

private:
    /** The handle to examine. */
    const std::unique_ptr<mud::core::handle>& _handle;

    /** The signal mask to register */
    signal_type _mask;

    /** The function handler. */
    function_type _fn;
};

/**
 * Bit-wise OR of event signals.
 */
event::signal_type operator|(event::signal_type lhs, event::signal_type rhs);

/**
 * Bit-wise AND of event signals.
 */
event::signal_type operator&(event::signal_type lhs, event::signal_type rhs);

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_EVENT_H_ */
