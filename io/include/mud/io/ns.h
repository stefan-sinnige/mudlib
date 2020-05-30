#ifndef _MUDLIB_IO_NS_H_
#define _MUDLIB_IO_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(EXPORTS)
        #define MUDLIB_IO_API __declspec(dllexport)
    #else
        #define MUDLIB_IO_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_IO_API
#endif

#define BEGIN_MUDLIB_IO_NS namespace mud { namespace io {
#define END_MUDLIB_IO_NS   } }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_NS_H_ */
