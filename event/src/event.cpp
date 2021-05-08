#include <mud/event/event.h>

BEGIN_MUDLIB_EVENT_NS

event::event(const std::unique_ptr<mud::core::handle>& handle,
        handler_fn handler, signal_t mask)
    : _handle(handle), _handler(handler), _mask(mask)
{
}

event::event(const std::unique_ptr<mud::core::handle>& handle)
    : _handle(handle), _handler(nullptr), _mask(signal_t::NONE)
{
}

event::event(const event& rhs)
    : _handle(rhs._handle), _handler(rhs._handler), _mask(rhs._mask)
{
}

event::~event()
{
}

bool
event::operator==(const event& rhs) const
{
    return _handle == rhs._handle;
}

bool
event::operator!=(const event& rhs) const
{
    return !operator==(rhs);
}

const std::unique_ptr<mud::core::handle>&
event::handle() const
{
    return _handle;
};

event::signal_t
event::mask() const
{
    return _mask;
}

void
event::call()
{
    _handler(*this);
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
