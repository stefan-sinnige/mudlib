#ifndef _MUD_UI_WIN32_APPLICATION_H_
#define _MUD_UI_WIN32_APPLICATION_H_

#include <mud/ui/ns.h>
#include <mud/ui/application.h>

BEGIN_MUDLIB_UI_NS

namespace win32 {

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
     * Return the global instance of the WIN32 application.
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

private:
};

} // namespace x11

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_WIN32_APPLICATION_H_ */

