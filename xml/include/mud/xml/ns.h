#ifndef _MUDLIB_XML_NS_H_
#define _MUDLIB_XML_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(MUDLIB_XML_EXPORTS)
        #define MUDLIB_XML_API __declspec(dllexport)
        #define MUDLIB_XML_API_EXPORT __declspec(dllexport)
    #else
        #define MUDLIB_XML_API __declspec(dllimport)
        #define MUDLIB_XML_API_EXPORT __declspec(dllexport)
    #endif
#else
    #define MUDLIB_XML_API
    #define MUDLIB_XML_API_EXPORT
#endif

#define BEGIN_MUDLIB_XML_NS                                                   \
    namespace mud {                                                           \
        namespace xml {
#define END_MUDLIB_XML_NS                                                     \
        }                                                                     \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_NS_H_ */
