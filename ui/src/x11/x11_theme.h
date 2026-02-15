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

#ifndef _MUD_UI_X11_THEME_H_
#define _MUD_UI_X11_THEME_H_

#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <map>
#include <mud/ui/ns.h>

BEGIN_MUDLIB_UI_NS

namespace x11 {

    class theme
    {
    public:
        /**
         * Themed states.
         */
        enum class state_t
        {
            PASSIVE, /**< Control is not activated. */
            ACTIVE,  /**< Control is activated. */
            __END__
        };

        /**
         * Themed items.
         */
        enum class item_t
        {
            WINDOW,     /**< The background color of the window. */
            BACKGROUND, /**< The background color of the control. */
            BORDER,     /**< The border color and width of the control. */
            FONT,       /**< The font and color. */
            __END__
        };

        /**
         * @brief Constructor.
         */
        theme();

        /**
         * @brief Destructor
         */
        ~theme();

        /**
         * Return the global instance of the current X11 theme.
         */
        static theme& instance();

        /**
         * Get the graphics context for an item of a control in a certain state.
         */
        GC gc(item_t item, state_t state) const;

        /**
         * Get the color for an item of a control in a certain state,
         */
        XColor color(item_t item, state_t state) const;

        /**
         * Get the color map
         */
        Colormap colormap() const;

        /**
         * Get the font to use for all control textx.
         */
        XftFont* font() const;

        /**
         * Get the font height of the letter 'T'.
         */
        unsigned int font_height() const;

    private:
        /** The X11 colormap (not to be confused with _color_map). */
        Colormap _colormap;

        /** The color map table */
        typedef std::map<item_t, std::map<state_t, XColor>> color_map;
        color_map _color_map;

        /** The graphics context map. */
        typedef std::map<item_t, std::map<state_t, GC>> gc_map;
        gc_map _gc_map;

        /** The font to use */
        XftFont* _font;

        /** The associated font-height. */
        unsigned int _font_height;
    };

} // namespace x11

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUD_UI_X11_THEME_H_ */
