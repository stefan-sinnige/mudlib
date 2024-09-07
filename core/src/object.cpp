#include "mud/core/object.h"

BEGIN_MUDLIB_CORE_NS

object::object(const object& other)
{
}

object::object(object&& other)
{
    _notifiers = std::move(other._notifiers);
    for (auto notifier: _notifiers) {
        notifier->move_object(&other, this);
    }
}

object&
object::operator=(const object& other)
{
    if (this != &other)
    {
    }
    return *this;
}

object&
object::operator=(object&& other)
{
    if (this != &other)
    {
        _notifiers = std::move(other._notifiers);
        for (auto notifier: _notifiers) {
            notifier->move_object(&other, this);
        }
    }
    return *this;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
