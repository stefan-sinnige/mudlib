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

#include "x11/x11_pushbutton.h"
#include "mud/ui/application.h"
#include "mud/ui/event.h"
#include "mud/ui/pushbutton.h"
#include "mud/ui/task.h"
#include "x11/x11_application.h"
#include "x11/x11_theme.h"
#include "x11/x11_window.h"

BEGIN_MUDLIB_UI_NS

pushbutton::impl::impl(pushbutton& pb, window& parent)
  : x11::control(pb), _pushbutton(pb), _parent(parent)
{
    window::impl::get(_parent)->add_control(pb);
}

pushbutton::impl::~impl() {}

void
pushbutton::impl::initialise()
{
    x11::application& application = x11::application::instance();
    int scr = DefaultScreen(application.display().get());
    Display* dpy = application.display().get();
    Window parent = window::impl::get(_parent)->NativeControl();

    // Get the gaphics context for thw window
    GC gc = x11::theme::instance().gc(x11::theme::item_t::WINDOW,
                                      x11::theme::state_t::PASSIVE);
    XGCValues gc_values;
    XGetGCValues(dpy, gc, GCForeground | GCBackground, &gc_values);

    // Draw the window
    Window btn =
        ::XCreateSimpleWindow(dpy, parent, _pushbutton.property<position>().x(),
                              _pushbutton.property<position>().y(),
                              _pushbutton.property<size>().width(),
                              _pushbutton.property<size>().height(), 0,
                              gc_values.foreground, gc_values.background);
    XSelectInput(dpy, btn,
                 ExposureMask | KeyPressMask | ButtonPressMask |
                     ButtonReleaseMask);
    XMapWindow(dpy, btn);
    XFlush(dpy);
    NativeControl(btn);
}

void
pushbutton::impl::expose()
{
    x11::application& application = x11::application::instance();
    int scr = DefaultScreen(application.display().get());
    Display* dpy = application.display().get();
    Window btn = NativeControl();

    // Draw the rectangle and the text
    draw_rectangle(dpy, scr, btn);
    draw_text(dpy, scr, btn);
}

void
pushbutton::impl::draw_rectangle(Display* dpy, int scr, Drawable win)
{
    int width = _pushbutton.property<size>().width();
    int height = _pushbutton.property<size>().height();

    int cornerwidth, cornerheight;
    cornerwidth = cornerheight = 7;

    // Get the gaphics context for the button background
    GC gc = x11::theme::instance().gc(x11::theme::item_t::BACKGROUND,
                                      x11::theme::state_t::PASSIVE);

    // Create the four cornered filled corners.

    int double_cornerwidth = cornerwidth << 1;
    int double_cornerheight = cornerheight << 1;
    if (double_cornerwidth > width) {
        double_cornerwidth = cornerwidth = 0;
    }
    if (double_cornerheight > height) {
        double_cornerheight = cornerheight = 0;
    }

    XArc corners[4];
    corners[0].x = 0;
    corners[0].y = 0;
    corners[0].width = double_cornerwidth;
    corners[0].height = double_cornerheight;
    corners[0].angle1 = 180 * 64;
    corners[0].angle2 = -90 * 64;

    corners[1].x = width - double_cornerwidth - 1;
    corners[1].y = 0;
    corners[1].width = double_cornerwidth;
    corners[1].height = double_cornerheight;
    corners[1].angle1 = 90 * 64;
    corners[1].angle2 = -90 * 64;

    corners[2].x = width - double_cornerwidth - 1;
    corners[2].y = height - double_cornerheight - 1;
    corners[2].width = double_cornerwidth;
    corners[2].height = double_cornerheight;
    corners[2].angle1 = 0;
    corners[2].angle2 = -90 * 64;

    corners[3].x = 0;
    corners[3].y = height - double_cornerheight - 1;
    corners[3].width = double_cornerwidth;
    corners[3].height = double_cornerheight;
    corners[3].angle1 = 270 * 64;
    corners[3].angle2 = -90 * 64;

    XFillArcs(dpy, win, gc, corners, 4);

    // Create the three rectangles

    XRectangle rectangles[3];
    rectangles[0].x = cornerwidth;
    rectangles[0].y = 0;
    rectangles[0].width = width - double_cornerwidth;
    rectangles[0].height = height;

    rectangles[1].x = 0;
    rectangles[1].y = cornerheight;
    rectangles[1].width = cornerwidth;
    rectangles[1].height = height - double_cornerheight;

    rectangles[2].x = width - cornerwidth;
    rectangles[2].y = cornerheight;
    rectangles[2].width = cornerwidth;
    rectangles[2].height = height - double_cornerheight;

    XFillRectangles(dpy, win, gc, rectangles, 3);
}

void
pushbutton::impl::draw_text(Display* dpy, int scr, Drawable win)
{
    const std::string& txt = _pushbutton.property<text>().value();
    int width = _pushbutton.property<size>().width();
    int height = _pushbutton.property<size>().height();

    // Get the gaphics context for the font and the font itself
    GC gc = x11::theme::instance().gc(x11::theme::item_t::BACKGROUND,
                                      x11::theme::state_t::PASSIVE);
    XftFont* xft_font = x11::theme::instance().font();

    // Get the width of the text if drawn with the font and calculate the
    // text (x, y) coordinates for a centred text.
    XGlyphInfo extents;
    XftTextExtentsUtf8(dpy, xft_font, (const XftChar8*)txt.c_str(), txt.size(),
                       &extents);
    int tx = (width - extents.width) / 2;
    int ty = height - (height - x11::theme::instance().font_height()) / 2;

    // Draw the text
    XColor color;
    XftColor xft_color;
    xft_color.pixel = color.pixel;
    xft_color.color.red = color.red;
    xft_color.color.green = color.green;
    xft_color.color.blue = color.blue;
    xft_color.color.alpha = 0xffff;
    XftDraw* xft_draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, scr),
                                      x11::theme::instance().colormap());
    XftDrawStringUtf8(xft_draw, &xft_color, xft_font, tx, ty,
                      (XftChar8*)txt.c_str(), txt.size());
    XftDrawDestroy(xft_draw);
}

void
pushbutton::impl_deleter::operator()(pushbutton::impl* ptr) const
{
    delete ptr;
}

/** The explicit specialisation of a UI pushbutton */

pushbutton::pushbutton(window& parent)
{
    default_properties();
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, parent));
}

pushbutton::~pushbutton() {}

void
pushbutton::initialise()
{
    _impl->initialise();
}

void
pushbutton::dispatch(const mud::ui::event& event)
{
    switch (event.type()) {
        case event::type_t::EXPOSE:
            _impl->expose();
            break;
        case mud::ui::event::type_t::MOUSE:
            if (_mouse_event_fn != nullptr) {
                auto& ev = static_cast<const mud::ui::event::mouse&>(event);
                _mouse_event_fn(ev);
            }
            break;
        default:
            /* Not handled */
            break;
    }
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
