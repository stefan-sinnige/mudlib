#ifndef _MUDIO_HANDLE_TEST_H_
#define _MUDIO_HANDLE_TEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class KernelHandleTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(KernelHandleTest);
    CPPUNIT_TEST(KernelHandleTypeTraits);
    CPPUNIT_TEST(BasicKernelHandleIntDefaultConstructor);
    CPPUNIT_TEST_SUITE_END();
public:
    void KernelHandleTypeTraits();
    void BasicKernelHandleIntDefaultConstructor();
};

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDIO_HANDLE_TEST_H_ */
