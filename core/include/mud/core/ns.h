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
