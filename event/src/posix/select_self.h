#ifndef _MUDLIB_EVENT_SELECT_SELF_H_
#define _MUDLIB_EVENT_SELECT_SELF_H_

#include <mud/event/ns.h>
#include <mud/event/event.h>

BEGIN_MUDLIB_EVENT_NS

/**
 * @brief A class to implement a self signalling resource for @c select
 * interruption.
 */

class select_self
{
public:
    /**
     * @brief Constructor, creating a self-signalling resource.
     */
    select_self();

    /**
     * @brief Destructor.
     */
    ~select_self();

    /**
     * The handle to detect a signal being sent.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /*
     * Send a signal to the resource.
     */
    void send();

    /*
     * Receive a signal.
     */
    void receive();

private:
    /**
     * Platform specific implementation.
     */
    class impl;
    struct impl_deleter {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_SELECT_SELF_H_ */
