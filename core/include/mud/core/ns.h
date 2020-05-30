#ifndef _MUDLIB_CORE_NS_H_
#define _MUDLIB_CORE_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(EXPORTS)
        #define MUDLIB_CORE_API __declspec(dllexport)
    #else
        #define MUDLIB_CORE_API __declspec(dllimport)
    #endif
#else
    #define MUDCORE_API
#endif

#define BEGIN_MUDLIB_CORE_NS namespace mud { namespace core {
#define END_MUDLIB_CORE_NS   } }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_NS_H_ */
