#ifndef _MUDLIB_CORE_NS_H_
#define _MUDLIB_CORE_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
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

#define BEGIN_MUDLIB_CORE_NS namespace mud { namespace core {
#define END_MUDLIB_CORE_NS   } }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_NS_H_ */
