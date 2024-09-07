#ifndef _MUDLIB_TLS_NS_H_
#define _MUDLIB_TLS_NS_H_

#if defined(_WIN32)
    #if defined(MUDLIB_TLS_EXPORTS)
        #define MUDLIB_TLS_API __declspec(dllexport)
    #else
        #define MUDLIB_TLS_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_TLS_API
#endif

#define BEGIN_MUDLIB_TLS_NS                                                   \
    namespace mud {                                                            \
        namespace tls {
#define END_MUDLIB_TLS_NS                                                     \
    }                                                                          \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TLS_NS_H_ */
