#ifndef _MUDLIB_UI_EVENT_H_
#define _MUDLIB_UI_EVENT_H_

#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

/*
 * Forward declaration
 */
class control;

/**
 * @brief A UI event is the base class for anything that is a representation
 * of a user interface related event.
 *
 */
class MUDLIB_UI_API event
{
public:
    /*
     * Forward declarations of all UI events
     */
    class none;
    class expose;
    class mouse;

    /**
     * @brief Enumeration of event types
     */
    enum class type_t {
        EXPOSE,     /**< Exposure event */
        MOUSE       /**< Mouse event */
    };

    /**
     * @brief Destructor.
     */
    virtual ~event();

    /**
     * @brief The type of the event.
     */
    type_t type() const;

    /**
     * @brief The control as the target for the event.
     */
    mud::ui::control& control() const;

private:
    /**
     * @brief Creation of a UI event.
     * @param ctrl [in] The associated control.
     */
    event(type_t type, mud::ui::control& ctrl);

    /**
     * The event type.
     */
    type_t _type;

    /** Reference to the control */
    mud::ui::control& _ctrl;
};

inline event::type_t
event::type() const
{
    return _type;
}

inline mud::ui::control&
event::control() const
{
    return _ctrl;
}

/**
 * @brief A UI event that represents an exposure event.
 *
 * An exposure event occurs when (part of) a window or a control becomes
 * exposed and needs to be redrawn. This usually involves re-drawing the
 * part of the control that becomes exposed, or the entire control.
 *
 * Not all UI implementations support the exposure event.
 */
class MUDLIB_UI_API event::expose: public event
{
public:
    /**
     * @brief Creation of a nil event.
     * @param ctrl [in] The associated control.
     */
    expose(mud::ui::control& ctrl);

    /**
     * @brief Destructor
     */
    ~expose();

private:
};

/**
 * @brief A UI event that represents a mouse event.
 *
 * A mouse event occurs when a mouse is clicked and/or moved within the
 * application's windows.
 */
class MUDLIB_UI_API event::mouse: public event
{
public:
    /**
     * @brief Creation of a nil event.
     * @param ctrl [in] The associated control.
     */
    mouse(mud::ui::control& ctrl);

    /**
     * @brief Destructor
     */
    ~mouse();

private:
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_EVENT_H_ */

