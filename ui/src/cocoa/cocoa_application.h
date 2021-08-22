#ifndef _MUD_UI_COCOA_APPLICATION_H_
#define _MUD_UI_COCOA_APPLICATION_H_

#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/application.h>
#include <mud/event/event_mechanism.h>

BEGIN_MUDLIB_UI_NS

namespace cocoa {

class application: public mud::ui::application
{
public:
    /**
     * @brief Constructor.
     */
    application();

    /**
     * @brief Destructor
     */
    ~application();

    /**
     * Return the global instance of the cocoa application.
     */
    static application& instance();

    /**
     * @brief Initialise the application
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void initialise() override;

    /**
     * @brief Finalise the application
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    virtual void finalise() override;

    /**
     * @brief Send a wake-up event
     *
     * This will send a custom NSEvent to wake-up from the blocking event
     * loop in order to process a non-NSEvent signal.
     */
    void wakeup();
};

} // namespace cocoa

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_APPLICATION_H_ */

