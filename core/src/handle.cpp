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

#include <mud/core/handle.h>
#include <mud/core/exception.h>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Type conversion to the internal type representation
 */

template<>
int
internal_handle<int>(std::shared_ptr<handle> handle)
{
    if ((handle->type() != handle::type_t::SELECT) &&
        (handle->type() != handle::type_t::__TEST)) {
        throw std::invalid_argument("Handle of incorrect type");
    }
    select_handle* h = static_cast<select_handle*>(handle.get());
    return *h;
}

END_MUDLIB_CORE_NS

/*
 * Include platform specific handle implementations.
 */
#if defined(_WIN32)
  #include "win32/windows_handle.cpp"
  #include "posix/select_socket.cpp"
#else
  // We can either use a pipe or a socket:
  #include "posix/select_pipe.cpp"
  // #include "posix/select_socket.cpp"
#endif

/* vi: set ai ts=4 expandtab: */
