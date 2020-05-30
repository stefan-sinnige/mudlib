#ifndef _MUDIO_PIPE_TEST_H_
#define _MUDIO_PIPE_TEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class PipeTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PipeTest);
    CPPUNIT_TEST(TypeTraits);
    CPPUNIT_TEST(WriteReadFormattedCharacter);
    CPPUNIT_TEST(WriteReadFormattedInteger);
    CPPUNIT_TEST(WriteReadFormattedString);
    CPPUNIT_TEST(WriteReadFormattedCombination);
    CPPUNIT_TEST(WriteReadUnformattedDataBlock);
    CPPUNIT_TEST(NonBlockingNothingToRead);
    CPPUNIT_TEST_SUITE_END();
public:
    void TypeTraits();
    void WriteReadFormattedCharacter();
    void WriteReadFormattedInteger();
    void WriteReadFormattedString();
    void WriteReadFormattedCombination();
    void WriteReadUnformattedDataBlock();
    void NonBlockingNothingToRead();
};

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDIO_PIPE_TEST_H_ */
