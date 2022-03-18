#ifndef _MUDLIB_HTML_NS_H_
#define _MUDLIB_HTML_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
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
