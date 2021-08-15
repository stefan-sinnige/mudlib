#ifndef _MUDLIB_UI_OBJECT_H_
#define _MUDLIB_UI_OBJECT_H_

#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

/**
 * @brief AUI object is the base class for anything that is a representation
 * of a graphical element that interacts with the user.
 *
 * An @c object is the base class for all the user interface elements. This
 * includes widgets like buttons, text-boxes, canvases. It also includes
 * elements that are containers for other widgets, including panels and
 * top level windows.
 *
 */
class MUDLIB_UI_API object
{
public:
    /**
     * Constructor.
     */
    object();

    /**
     * Destructor.
     */
    virtual ~object();
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_OBJECT_H_ */

