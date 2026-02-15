/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#ifndef _MUDLIB_UI_CONTROL_H_
#define _MUDLIB_UI_CONTROL_H_

#include <any>
#include <map>
#include <mud/ui/ns.h>
#include <string>
#include <typeindex>

BEGIN_MUDLIB_UI_NS

/**
 * Forward declarations.
 */
class event;
namespace x11 {
    class mechanism;
}
namespace win32 {
    class mechanism;
}
namespace cocoa {
    class mechanism;
}

/**
 * @brief A UI control is the base class for anything that is a representation
 * of a graphical element that interacts with the user.
 *
 * An @c control is the base class for all the user interface elements. This
 * includes widgets like buttons, text-boxes, canvases. It also includes
 * elements that are containers for other widgets, including panels and
 * top level windows.
 *
 */
class MUDLIB_UI_API control
{
public:
    /**
     * @brief Creation of a UI control.
     */
    control();

    /**
     * @brief Destructor.
     */
    virtual ~control();

    /**
     * @brief Initialise the control
     *
     * When initialising the control, the implementation specific creation of
     * the control will be invoked. This is called only once upon initialisation
     * of the control.
     *
     * This function will be invoked on the implementation dependent UI
     * thread.
     */
    virtual void initialise() = 0;

    /**
     * Configure a property of the control.
     *
     * @tparam Property The type of the property.
     * @tparam Args     The variadic arguments for the @c Property constructor.
     *
     * @param args [in] The arguments passed on to the constructor pf the @c
     *                  Property.
     *
     * @trow std::logic_error The @c Property is not defined for this control.
     */
    template<typename Property, class... Args>
    control& property(Args... args)
    {
        auto search = _properties.find(std::type_index(typeid(Property)));
        if (search == _properties.end()) {
            throw std::logic_error("No such property");
        }
        auto& prop = std::any_cast<Property&>(search->second);
        prop = Property(std::forward<Args>(args)...);
        return *this;
    }

    /**
     * Direct access to configure a property of the control.
     *
     * @tparam Property The type of the property.
     *
     * @trow std::logic_error The @c Property is not defined for this control.
     */
    template<typename Property>
    Property& property()
    {
        auto search = _properties.find(std::type_index(typeid(Property)));
        if (search == _properties.end()) {
            throw std::logic_error("No such property");
        }
        auto& prop = std::any_cast<Property&>(search->second);
        return prop;
    }

    /**
     * Direct access to query a property of the control.
     *
     * @tparam Property The type of the property.
     *
     * @trow std::logic_error The @c Property is not defined for this control.
     */
    template<typename Property>
    const Property& property() const
    {
        auto search = _properties.find(std::type_index(typeid(Property)));
        if (search == _properties.end()) {
            throw std::logic_error("No such property");
        }
        auto& prop = std::any_cast<const Property&>(search->second);
        return prop;
    }

protected:
    /**
     * @brief Dispatch a UI event.
     * @param event [in] The event details.
     *
     * The UI event that has been received is dispatched to this control for
     * further handling. Depending on the type of event, the control-specific
     * handling routine will be invoked.
     */
    virtual void dispatch(const event& event) = 0;

    /**
     * Initialise the suported properties with default values.
     */
    virtual void default_properties() = 0;

    /** Set of properties */
    std::map<std::type_index, std::any> _properties;

    /** Friend class access */
    friend class application;
    friend class cocoa::mechanism;
    friend class win32::mechanism;
    friend class x11::mechanism;
};

/**
 * Position property.
 */
class position
{
public:
    /**
     * Defining a position coordinate.
     *
     * @param x [in] The X coordinate (0 indicating the left).
     * @param y [in] The Y coordinate (0 indicating the top).
     */
    position(int x, int y) : _x(x), _y(y) {}

    /**
     * Copy a position.
     */
    position(const position& rhs)
    {
        _x = rhs._x;
        _y = rhs._y;
    }

    /**
     * The X-coordinate.
     */
    int x() const { return _x; }

    /**
     * Setting the X-coordinate.
     *
     * @param x [in] The X coordinate (0 indicating the left).
     */
    void x(int value) { _x = value; }

    /**
     * The X-coordinate.
     */
    int y() const { return _y; }

    /**
     * Setting the Y-coordinate.
     *
     * @param x [in] The Y coordinate (0 indicating the left).
     */
    void y(int value) { _y = value; }

private:
    /**
     * Data members.
     */
    int _x;
    int _y;
};

/**
 * Size property.
 */
class size
{
public:
    /**
     * Defining a size.
     *
     * @param width [in] The width of the control.
     * @param height [in] The height of the control.
     */
    size(int width, int height) : _width(width), _height(height){};

    /**
     * Copy a position.
     */
    size(const size& rhs)
    {
        _width = rhs._width;
        _height = rhs._height;
    }

    /**
     * The width of the control.
     */
    int width() const { return _width; }

    /**
     * Setting the width of the control.
     *
     * @param width [in] The width of the control.
     */
    void width(int value) { _width = value; }

    /**
     * The height of the control.
     */
    int height() const { return _height; }

    /**
     * Setting the height of the control.
     *
     * @param height [in] The height of the control.
     */
    void height(int value) { _height = value; }

private:
    /**
     * Data members.
     */
    int _width;
    int _height;
};

/**
 * Text property.
 */
class text
{
public:
    /**
     * Defining the text.
     *
     * @param text [in] The text of the control.
     */
    text(const std::string& text) : _text(text) {}

    /**
     * Copy a position.
     */
    text(const text& rhs) { _text = rhs._text; }

    /**
     * The text value.
     */
    const std::string& value() const { return _text; }

    /**
     * Setting the value of the text.
     *
     * @param height [in] The text of the control.
     */
    void value(const std::string& value) { _text = value; }

private:
    /**
     * Data members.
     */
    std::string _text;
};

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_CONTROL_H_ */
