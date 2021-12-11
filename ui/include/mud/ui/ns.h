#ifndef _MUDLIB_UI_NS_H_
#define _MUDLIB_UI_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(MUDLIB_UI_EXPORTS)
        #define MUDLIB_UI_API __declspec(dllexport)
    #else
        #define MUDLIB_UI_API __declspec(dllimport)
    #endif
#else
    #define MUDLIB_UI_API
#endif

#define BEGIN_MUDLIB_UI_NS                                                     \
    namespace mud {                                                            \
        namespace ui {
#define END_MUDLIB_UI_NS                                                       \
    }                                                                          \
    }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_UI_NS_H_ */
