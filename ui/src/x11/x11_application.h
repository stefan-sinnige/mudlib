#ifndef _MUD_UI_X11_APPLICATION_H_
#define _MUD_UI_X11_APPLICATION_H_

#include <X11/Xlib.h>
#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/application.h>
#include <mud/event/event_mechanism.h>

BEGIN_MUDLIB_UI_NS

namespace x11 {

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
     * Return the global instance of the X11 application.
     */
    static application& instance();

    /**
     * Return the X11 display
     */
    const std::shared_ptr<Display>& display() const;

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

private:
    /* X11 variables */
    std::shared_ptr<Display> _display;
};

} // namespace x11

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_APPLICATION_H_ */

