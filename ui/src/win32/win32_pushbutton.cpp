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

#include "win32/win32_pushbutton.h"
#include "mud/ui/application.h"
#include "mud/ui/event.h"
#include "mud/ui/pushbutton.h"
#include "mud/ui/task.h"
#include "win32/win32_window.h"

BEGIN_MUDLIB_UI_NS

pushbutton::impl::impl(pushbutton& pb, window& parent)
  : win32::control(pb), _pushbutton(pb), _parent(parent)
{
    window::impl::get(_parent)->add_control(pb);
}

pushbutton::impl::~impl() {}

void
pushbutton::impl::initialise()
{
    // Get the parent window containing the push button.
    HWND parent = window::impl::get(_parent)->NativeControl();

    // Create the pushbutton
    HWND wnd =
        CreateWindow("Button", _pushbutton.property<text>().value().c_str(),
                     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                     _pushbutton.property<position>().x(),
                     _pushbutton.property<position>().y(),
                     _pushbutton.property<size>().width(),
                     _pushbutton.property<size>().height(), parent,
                     (HMENU)100, /* Should be unique ID */
                     (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), NULL);
    if (wnd == NULL) {
        throw std::system_error(::GetLastError(), std::system_category(),
                                "CrateWindowEx");
    }

    NativeControl(wnd);
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
