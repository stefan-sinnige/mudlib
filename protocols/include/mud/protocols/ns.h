#ifndef _MUDLIB_PROTOCOLS_NS_H_
#define _MUDLIB_PROTOCOLS_NS_H_

#if defined(_WIN32)
    #if defined(MUDLIB_PROTOCOLS_EXPORTS)
        #define MUDLIB_PROTOCOLS_API __declspec(dllexport)
    #else
        #define MUDLIB_PROTOCOLS_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_PROTOCOLS_API
#endif

#define BEGIN_MUDLIB_PROTOCOLS_NS                                                   \
    namespace mud {                                                            \
        namespace protocols {
#define END_MUDLIB_PROTOCOLS_NS                                                     \
    }                                                                          \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_PROTOCOLS_NS_H_ */
