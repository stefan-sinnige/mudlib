#ifndef _MUDIO_EVENT_LOOP_TEST_H_
#define _MUDIO_EVENT_LOOP_TEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class KernelEventLoopTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(KernelEventLoopTest);
    CPPUNIT_TEST(TypeTraits);
    CPPUNIT_TEST(ThreadTerminate);
    CPPUNIT_TEST(HandlerTerminate);
    CPPUNIT_TEST(HandlerRegistration);
    CPPUNIT_TEST(HandlerDeregistration);
    CPPUNIT_TEST(HandlerDoubleRegistration);
    CPPUNIT_TEST_SUITE_END();

public:
    /* Routines called before and after every run */
    void setUp() override;
    void tearDown() override;

    /* Test cases */
    void TypeTraits();
    void ThreadTerminate();
    void HandlerTerminate();
    void HandlerRegistration();
    void HandlerDeregistration();
    void HandlerDoubleRegistration();
};

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDIO_EVENT_LOOP_TEST_H_ */
