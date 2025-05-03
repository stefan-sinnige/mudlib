#include <mud/event/event_mechanism.h>
#include "timer_dispatcher.h"

mud::event::event_mechanism::event_mechanism(
        const std::shared_ptr<mud::core::simple_task_queue>& queue,
        const std::shared_ptr<mud::event::timer_dispatcher>& timers)
    : _queue(queue), _timers(timers)
{
}

/* Creation of the event-mechanism factory */
template<>
mud::event::event_mechanism_factory&
mud::event::event_mechanism_factory::instance()
{
    static mud::event::event_mechanism_factory _instance;
    return _instance;
}

/* vi: set ai ts=4 expandtab: */
