#ifndef _MUD_UI_X11_CONTROL_H_
#define _MUD_UI_X11_CONTROL_H_

#include <functional>
#include <mud/ui/control.h>
#include <mud/ui/ns.h>
#include <windows.h>

BEGIN_MUDLIB_UI_NS

namespace win32 {

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
        void NativeControl(HWND ctrl);

        /**
         * Return the HWND native object.
         */
        HWND NativeControl() const;

        /**
         * @brief Return a reference of the control associated to the native
         * control.
         * @param ctrl [in] The native control
         */
        static std::reference_wrapper<mud::ui::control> find(HWND ctrl);

    private:
        /** Reference to the UI Control */
        mud::ui::control& _control;

        /** Reference to the Win32 Window */
        HWND _wnd;
    };

}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_CONTROL_H_ */
