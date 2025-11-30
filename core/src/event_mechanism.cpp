#include <mud/core/event_mechanism.h>
#include "timer_dispatcher.h"

mud::core::event_mechanism::event_mechanism(
        const std::shared_ptr<mud::core::simple_task_queue>& queue,
        const std::shared_ptr<mud::core::timer_dispatcher>& timers)
    : _queue(queue), _timers(timers)
{
}

/* Creation of the event-mechanism factory */
template<>
mud::core::event_mechanism_factory&
mud::core::event_mechanism_factory::instance()
{
    static mud::core::event_mechanism_factory _instance;
    return _instance;
}

/* vi: set ai ts=4 expandtab: */
