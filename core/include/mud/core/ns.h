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

#ifndef _MUDLIB_CORE_NS_H_
#define _MUDLIB_CORE_NS_H_

#if defined(_WIN32)
    #if defined(MUDLIB_CORE_EXPORTS)
        #define MUDLIB_CORE_API __declspec(dllexport)
        #define MUDLIB_CORE_API_EXPORT __declspec(dllexport)
    #else
        #define MUDLIB_CORE_API __declspec(dllimport)
        #define MUDLIB_CORE_API_EXPORT __declspec(dllexport)
    #endif
#else
    #define MUDLIB_CORE_API
    #define MUDLIB_CORE_API_EXPORT
#endif

#define BEGIN_MUDLIB_CORE_NS                                                   \
    namespace mud {                                                            \
        namespace core {
#define END_MUDLIB_CORE_NS                                                     \
    }                                                                          \
    }

#ifdef MUDLIB_LIBRARY
    #include "mud/core/internal/log.h"
#else
    #include <iostream>
    #define LOG(obj)
    #define TRACE(obj)  if (true) {} else std::cout
    #define DEBUG(obj)  if (true) {} else std::cout
    #define INFO(obj)   if (true) {} else std::cout
    #define WARN(obj)   if (true) {} else std::cout
    #define ERROR(obj)  if (true) {} else std::cout
    #define FATAL(obj)  if (true) {} else std::cout
    #define TYPEINFO(t) ""
#endif

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_NS_H_ */
