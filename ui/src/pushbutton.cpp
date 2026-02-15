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

#include "mud/ui/pushbutton.h"

BEGIN_MUDLIB_UI_NS

void
pushbutton::default_properties()
{
    _properties[std::type_index(typeid(position))] = position(0, 0);
    _properties[std::type_index(typeid(size))] = size(60, 25);
    _properties[std::type_index(typeid(text))] = text("");
}

void
pushbutton::event(pushbutton::mouse_event_func fn)
{
    _mouse_event_fn = fn;
}

END_MUDLIB_UI_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "src/win32/win32_pushbutton.cpp"
#elif defined(__APPLE__)
  #include "src/cocoa/cocoa_pushbutton.cpp"
#else
  #include "src/x11/x11_pushbutton.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
