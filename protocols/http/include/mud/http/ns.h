#ifndef _MUDLIB_HTTP_NS_H_
#define _MUDLIB_HTTP_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(MUDLIB_HTTP_EXPORTS)
        #define MUDLIB_HTTP_API __declspec(dllexport)
    #else
        #define MUDLIB_HTTP_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_HTTP_API
#endif

#define BEGIN_MUDLIB_HTTP_NS                                                   \
    namespace mud {                                                            \
        namespace http {
#define END_MUDLIB_HTTP_NS                                                     \
    }                                                                          \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTTP_NS_H_ */
