#include "cocoa/cocoa_application.h"
#include <mud/core/handle.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Implementation of a signalling resource.
 */
template<>
class cocoa_handle::signal::impl
{
public:
    /**
     * Constructor
     */
    impl();

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The handle.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * Send a signal to the resource.
     */
    void trigger();

    /**
     * Receive a signal.
     */
    bool capture();

private:
    /** The event handle */
    std::unique_ptr<mud::core::handle> _handle;
};

template<>
void
cocoa_handle::signal::impl_deleter::operator()(signal::impl* ptr) const
{
    delete ptr;
}

cocoa_handle::signal::impl::impl()
{
    _handle = std::unique_ptr<mud::core::handle>(new cocoa_handle(nullptr));
}

cocoa_handle::signal::impl::~impl()
{
    _handle.reset(nullptr);
}

const std::unique_ptr<mud::core::handle>&
cocoa_handle::signal::impl::handle() const
{
    return _handle;
}

void
cocoa_handle::signal::impl::trigger()
{
    mud::ui::cocoa::application::instance().wakeup();
}

bool
cocoa_handle::signal::impl::capture()
{
    // Nothing to capture really. The trigger event is used to wake-up the
    // application UI thread loop
    return true;
}

/** The explicit implementation for self signalling resources. */

template<>
cocoa_handle::signal::signal()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

template<>
cocoa_handle::signal::~signal()
{}

template<>
const std::unique_ptr<mud::core::handle>&
cocoa_handle::signal::handle() const
{
    return _impl->handle();
}

template<>
void
cocoa_handle::signal::trigger()
{
    _impl->trigger();
}

template<>
bool
cocoa_handle::signal::capture()
{
    return _impl->capture();
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
