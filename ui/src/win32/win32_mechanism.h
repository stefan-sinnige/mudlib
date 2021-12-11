#ifndef _MUD_UI_WIN32_MECHANISM_H_
#define _MUD_UI_WIN32_MECHANISM_H_

#include <mud/core/handle.h>
#include <mud/event/event_mechanism.h>
#include <mud/ui/ns.h>
#include <utility>
#include <windows.h>

BEGIN_MUDLIB_UI_NS

namespace win32 {

    class mechanism : public mud::event::event_mechanism
    {
    public:
        /* Type definition to the handler type */
        typedef std::function<void(void)> event_handler;

        /* Constructor */
        mechanism(const std::shared_ptr<mud::core::simple_task_queue>& queue);

        /* Destructor */
        ~mechanism();

        /* Register handler */
        void register_handler(mud::event::event&& event) override;

        /* Deregister handler */
        void deregister_handler(mud::event::event&& event) override;

        /* Initiate */
        std::shared_future<void> initiate() override;

        /* Terminate */
        void terminate() override;

        /* Detachable */
        virtual bool detachable() const override;

    private:
        /* Thread function */
        void loop();

        /* Set-up the UI */
        void setup();

        /* Close-down the UI */
        void closedown();

        /* Signal handlers */
        void terminate_signal_handler();
        void task_queue_signal_handler();
        void display_signal_handler();

        /* UI thread */
        std::atomic_bool _running;
        std::promise<void> _promise;
        std::shared_future<void> _future;
        mud::core::windows_handle::signal _terminate_signal;
        std::pair<HANDLE, std::function<void(void)>> _handlers[2];
    };

} // namespace win32

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUD_UI_WIN32_MECHANISM_H_ */
