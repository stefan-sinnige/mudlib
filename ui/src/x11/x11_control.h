#ifndef _MUD_UI_X11_CONTROL_H_
#define _MUD_UI_X11_CONTROL_H_

#include <X11/Xlib.h>
#include <functional>
#include <mud/ui/control.h>
#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

namespace x11 {

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
        void NativeControl(Window ctrl);

        /**
         * Return the Window native object.
         */
        Window NativeControl() const;

        /**
         * @brief Return a reference of the control associated to the native
         * control.
         * @param ctrl [in] The native control
         */
        static std::reference_wrapper<mud::ui::control> find(Window ctrl);

    private:
        /** Reference to the UI Control */
        mud::ui::control& _control;

        /** Reference to the X11 Window */
        Window _wnd;
    };

} // namespace x11

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_CONTROL_H_ */
