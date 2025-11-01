#include <mud/event/event.h>

BEGIN_MUDLIB_EVENT_NS

event::event()
  : _id(true)
{}

event::event(std::shared_ptr<mud::core::handle> handle, signal_type mask,
             function_type&& handler)
  : _handle(handle), _mask(mask), _fn(handler)
{}

event::~event() {}

bool
event::operator==(const event& rhs) const
{
    return _id == rhs._id;
}

bool
event::operator!=(const event& rhs) const
{
    return !operator==(rhs);
}

const mud::core::uuid&
event::id() const
{
    return _id;
}

std::shared_ptr<mud::core::handle>
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
