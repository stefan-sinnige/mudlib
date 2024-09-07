#include "mud/core/impulse.h"
#include "mud/core/notifier.h"

BEGIN_MUDLIB_CORE_NS

basic_notifier::basic_notifier(basic_impulse* impls)
    : _impulse(impls)
{
}

void
basic_notifier::detach(basic_notifier* unused)
{
    if (_impulse != nullptr) {
        _impulse->detach(this);
    }
}

void
basic_notifier::detach(basic_impulse* unused)
{
    _impulse = nullptr;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
