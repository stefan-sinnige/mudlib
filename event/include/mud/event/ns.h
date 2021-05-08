#ifndef _MUDLIB_EVENT_NS_H_
#define _MUDLIB_EVENT_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(MUDLIB_EVENT_EXPORTS)
        #define MUDLIB_EVENT_API __declspec(dllexport)
    #else
        #define MUDLIB_EVENT_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_EVENT_API
#endif

#define BEGIN_MUDLIB_EVENT_NS namespace mud { namespace event {
#define END_MUDLIB_EVENT_NS   } }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_EVENT_NS_H_ */
