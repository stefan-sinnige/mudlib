#include <mud/event/event.h>

BEGIN_MUDLIB_EVENT_NS

event::event(const std::unique_ptr<mud::core::handle>& handle, signal_type mask,
        function_type&& handler)
    : _handle(handle), _mask(mask), _fn(handler)
{
}

event::event(const std::unique_ptr<mud::core::handle>& handle)
    : _handle(handle), _mask(signal_type::NONE)
{
}

event::event(const event& rhs)
    : _handle(rhs._handle), _mask(rhs._mask), _fn(rhs._fn)
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

event::signal_type
event::mask() const
{
    return _mask;
}

event::function_type
event::handler() const
{
    return _fn;
}

END_MUDLIB_EVENT_NS

/* vi: set ai ts=4 expandtab: */
