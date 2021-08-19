#include <mud/event/event_mechanism.h>

/* Creation of the event-mechanism factory */
template<>
mud::event::event_mechanism_factory&
mud::event::event_mechanism_factory::instance()
{
    static mud::event::event_mechanism_factory _instance;
    return _instance;
}


/* vi: set ai ts=4 expandtab: */
