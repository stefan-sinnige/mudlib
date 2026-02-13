#include <mud/core/event.h>
#include <mud/core/message.h>

BEGIN_MUDLIB_CORE_NS

event::event(const mud::core::uuid& topic,
             std::shared_ptr<mud::core::handle> handle,
             signal_type mask)
  : _topic(topic), _handle(handle), _mask(mask)
{
}

event::event(event&& rhs)
  : _topic(rhs._topic)
  , _handle(rhs._handle)
  , _mask(rhs._mask)
{
}

event&
event::operator=(event&& rhs) {
    if (this != &rhs) {
        _topic = rhs._topic;
        _handle = rhs._handle;
        _mask = rhs._mask;
    }
    return *this;
}

bool
event::operator==(const event& rhs) const
{
    return _topic == rhs._topic;
}

bool
event::operator!=(const event& rhs) const
{
    return !operator==(rhs);
}

const mud::core::uuid&
event::topic() const
{
    return _topic;
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

void
event::publish() const
{
    ::mud::core::message msg(_topic);
    ::mud::core::broker::publish(msg);
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
