#ifndef _MUD_UI_COCOA_PUSHBUTTON_H_
#define _MUD_UI_COCOA_PUSHBUTTON_H_

#include <mud/core/handle.h>
#include <mud/ui/ns.h>
#include <mud/ui/pushbutton.h>
#include <mud/event/event_mechanism.h>
#include "win32/win32_control.h"
#include <windows.h>

BEGIN_MUDLIB_UI_NS

class pushbutton::impl : public win32::control
{
public:
    /**
     * @brief Constructor.
     */
    impl(pushbutton& pb, window& parent);

    /**
     * @brief Destructor
     */
    ~impl();

    /**
     * @brief Initialise the pushbutton
     *
     * This function will be invoked on the implementation dependent UI thread.
     */
    void initialise();

private:
    /** Reference to the pushbutton */
    pushbutton& _pushbutton;

    /** Reference to the parent window containing the push-button */
    window& _parent;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_PUSHBUTTON_H_ */

