#include "kernel_handle_test.h"
#include "mud/io/kernel_handle.h"
#include <type_traits>

CPPUNIT_TEST_SUITE_REGISTRATION(KernelHandleTest);

void
KernelHandleTest::KernelHandleTypeTraits()
{
    // Given A type 'handle'
    // When  I query the tpe information
    // Then  The type is not default constructible
    //  And  the type is not copy constructible
    //  And  the type is not assignable

    bool trait;
    trait = std::is_default_constructible<mud::io::kernel_handle>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
    trait = std::is_copy_constructible<mud::io::kernel_handle>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
    trait = std::is_assignable<
            mud::io::kernel_handle,
            mud::io::kernel_handle>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
}

void
KernelHandleTest::BasicKernelHandleIntDefaultConstructor()
{
    // Given An operating system integer resource with value 10
    // When  A basic_handle is created
    // Then  The basic_handle uses the operating system resource

    int resource = 10;
    mud::io::kernel_handle h(resource);
    CPPUNIT_ASSERT_EQUAL(resource, (int)h);
}

/* vi: set ai ts=4 expandtab: */
