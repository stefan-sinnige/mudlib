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

#ifndef _MUDLIB_HTML_NS_H_
#define _MUDLIB_HTML_NS_H_

#if defined(_WIN32)
    #if defined(MUDLIB_HTML_EXPORTS)
        #define MUDLIB_HTML_API __declspec(dllexport)
        #define MUDLIB_HTML_API_EXPORT __declspec(dllexport)
    #else
        #define MUDLIB_HTML_API __declspec(dllimport)
        #define MUDLIB_HTML_API_EXPORT __declspec(dllexport)
    #endif
#else
    #define MUDLIB_HTML_API
    #define MUDLIB_HTML_API_EXPORT
#endif

#define BEGIN_MUDLIB_HTML_NS                                                   \
    namespace mud {                                                           \
        namespace html {
#define END_MUDLIB_HTML_NS                                                     \
        }                                                                     \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_NS_H_ */
