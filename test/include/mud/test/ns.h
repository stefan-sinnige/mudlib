#ifndef _MUDLIB_TEST_NS_H_
#define _MUDLIB_TEST_NS_H_

#if defined(WINDOWS) && defined(NATIVE)
    #if defined(MUDLIB_TEST_EXPORTS)
        #define MUDLIB_TEST_API __declspec(dllexport)
    #else
        #define MUDLIB_TEST_API __declspec(dllimport)
    #endif
#else
    #define MUDTEST_API
#endif

#define BEGIN_MUDLIB_TEST_NS namespace mud { namespace test {
#define END_MUDLIB_TEST_NS   } }

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_TEST_NS_H_ */
