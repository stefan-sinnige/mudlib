#ifndef _MUD_UI_COCOA_CONTROL_H_
#define _MUD_UI_COCOA_CONTROL_H_

#include <mud/ui/ns.h>
#include <mud/ui/control.h>
#include <functional>
#include <Cocoa/Cocoa.h>

BEGIN_MUDLIB_UI_NS

namespace cocoa {

class control
{
public:
    /**
     * @brief Constructor.
     * @param ctrl [in] The associated UI control
     */
    control(mud::ui::control& ctrl);

    /**
     * @brief Destructor
     */
    ~control();

    /**
     * Set the native control object.
     * @param ctrl [in] The native control.
     */
    void NativeControl(NSControl* ctrl);

    /**
     * Return the NSView native object.
     */
    NSControl* NativeControl() const;

    /**
     * @brief Return a reference of the control associated to the native
     * control.
     * @param ctrl [in] The native control
     */
    static std::reference_wrapper<mud::ui::control> find(NSControl* ctrl);

private:
    /** Reference to the UI Control */
    mud::ui::control& _control;

    /** Reference to the Cocoa control */
    NSControl* _ctrl;
};

}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_COCOA_CONTROL_H_ */

