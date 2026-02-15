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

#include "x11/x11_theme.h"
#include "mud/ui/exception.h"
#include "x11/x11_application.h"
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>

BEGIN_MUDLIB_UI_NS

/* static */
x11::theme&
x11::theme::instance()
{
    static x11::theme _instance;
    return _instance;
}

x11::theme::theme()
{
    // Get the display / screen
    x11::application& application = x11::application::instance();
    Display* dpy = application.display().get();
    int scr = DefaultScreen(dpy);

    // Color map and color table
    XVisualInfo* visualinfo;
    int nr;
    visualinfo = XGetVisualInfo(dpy, 0, NULL, &nr);
    _colormap = XCreateColormap(dpy, DefaultRootWindow(dpy), visualinfo->visual,
                                AllocNone);
    XColor color;

    // Window background colours
    XParseColor(dpy, _colormap, "rgb:f5/f5/f5", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::WINDOW][state_t::PASSIVE] = color;

    // Control background colours
    XParseColor(dpy, _colormap, "rgb:d3/d3/d3", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::BACKGROUND][state_t::PASSIVE] = color;
    XParseColor(dpy, _colormap, "rgb:a9/a9/a9", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::BACKGROUND][state_t::ACTIVE] = color;

    // Border colours
    XParseColor(dpy, _colormap, "rgb:00/00/00", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::BORDER][state_t::PASSIVE] = color;
    XParseColor(dpy, _colormap, "rgb:00/00/00", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::BORDER][state_t::ACTIVE] = color;

    // Font colours
    XParseColor(dpy, _colormap, "rgb:ff/ff/ff", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::FONT][state_t::PASSIVE] = color;
    XParseColor(dpy, _colormap, "rgb:ff/ff/ff", &color);
    XAllocColor(dpy, _colormap, &color);
    _color_map[item_t::FONT][state_t::ACTIVE] = color;
    XFree(visualinfo);

    // Create the feature GC's
    unsigned long gc_mask;
    XGCValues gc_values;

    // Window
    memset(&gc_values, 0, sizeof(XGCValues));
    gc_mask = GCForeground | GCBackground;
    gc_values.foreground = _color_map[item_t::WINDOW][state_t::PASSIVE].pixel;
    gc_values.background = _color_map[item_t::WINDOW][state_t::PASSIVE].pixel;
    _gc_map[item_t::WINDOW][state_t::PASSIVE] =
        XCreateGC(dpy, DefaultRootWindow(dpy), gc_mask, &gc_values);

    // Control background
    memset(&gc_values, 0, sizeof(XGCValues));
    gc_mask = GCForeground | GCBackground | GCLineWidth;
    gc_values.foreground =
        _color_map[item_t::BACKGROUND][state_t::PASSIVE].pixel;
    gc_values.background =
        _color_map[item_t::BACKGROUND][state_t::PASSIVE].pixel;
    _gc_map[item_t::BACKGROUND][state_t::PASSIVE] =
        XCreateGC(dpy, DefaultRootWindow(dpy), gc_mask, &gc_values);
    memset(&gc_values, 0, sizeof(XGCValues));
    gc_mask = GCForeground | GCBackground | GCLineWidth;
    gc_values.foreground =
        _color_map[item_t::BACKGROUND][state_t::ACTIVE].pixel;
    gc_values.background =
        _color_map[item_t::BACKGROUND][state_t::ACTIVE].pixel;
    _gc_map[item_t::BACKGROUND][state_t::ACTIVE] =
        XCreateGC(dpy, DefaultRootWindow(dpy), gc_mask, &gc_values);

    // Lookup the most appropriate font
    _font = XftFontOpen(dpy, scr, XFT_FAMILY, XftTypeString, "Helvetica neue",
                        XFT_SIZE, XftTypeDouble, 11.0, NULL);
    if (_font == nullptr) {
        throw std::runtime_error("No appropriate font found.");
    }
    XGlyphInfo extents;
    XftTextExtentsUtf8(dpy, _font, (const XftChar8*)"T", 1, &extents);
    _font_height = extents.height;
}

x11::theme::~theme()
{
    // Get the display
    x11::application& application = x11::application::instance();
    Display* dpy = application.display().get();

    // Free all graphics contexts
    for (auto& item_entry : _gc_map) {
        for (auto& state_entry : item_entry.second) {
            if (state_entry.second != nullptr) {
                XFreeGC(dpy, state_entry.second);
            }
        }
    }

    // Free all color allocations and color map
    for (auto& item_entry : _color_map) {
        for (auto& state_entry : item_entry.second) {
            XFreeColors(dpy, _colormap, &(state_entry.second.pixel), 1, 0);
        }
    }
    XFreeColormap(dpy, _colormap);

    // Close the font
    if (_font != nullptr) {
        XftFontClose(dpy, _font);
    }
}

Colormap
x11::theme::colormap() const
{
    return _colormap;
}

XColor
x11::theme::color(x11::theme::item_t item, x11::theme::state_t state) const
{
    auto item_entry = _color_map.find(item);
    if (item_entry != _color_map.end()) {
        auto state_entry = item_entry->second.find(state);
        if (state_entry != item_entry->second.end()) {
            return state_entry->second;
        }
    }
    throw std::logic_error("No such theme color");
}

GC
x11::theme::gc(x11::theme::item_t item, x11::theme::state_t state) const
{
    auto item_entry = _gc_map.find(item);
    if (item_entry != _gc_map.end()) {
        auto state_entry = item_entry->second.find(state);
        if (state_entry != item_entry->second.end()) {
            return state_entry->second;
        }
    }
    throw std::logic_error("No such theme feature");
}

XftFont*
x11::theme::font() const
{
    return _font;
}

unsigned int
x11::theme::font_height() const
{
    return _font_height;
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
