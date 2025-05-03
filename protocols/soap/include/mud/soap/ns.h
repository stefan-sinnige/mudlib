#ifndef _MUDLIB_SOAP_NS_H_
#define _MUDLIB_SOAP_NS_H_

#if defined(_WIN32)
    #if defined(MUDLIB_SOAP_EXPORTS)
        #define MUDLIB_SOAP_API __declspec(dllexport)
    #else
        #define MUDLIB_SOAP_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_SOAP_API
#endif

#define BEGIN_MUDLIB_SOAP_NS                                                   \
    namespace mud {                                                            \
        namespace soap {
#define END_MUDLIB_SOAP_NS                                                     \
    }                                                                          \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_SOAP_NS_H_ */
