#include "pipe_test.h"
#include "mud/io/pipe.h"
#include <string>
#include <type_traits>
#include <unistd.h>

#include "mud/io/streambuf.h"

CPPUNIT_TEST_SUITE_REGISTRATION(PipeTest);

void
PipeTest::TypeTraits()
{
    // Given A type 'pipe'
    // When  I query the tpe information
    // Then  The type is default constructible
    //  And  the type is not copy constructible
    //  And  the type is not assignable

    bool trait;
    trait = std::is_default_constructible<mud::io::pipe>::value;
    CPPUNIT_ASSERT_EQUAL(true, trait);
    trait = std::is_copy_constructible<mud::io::pipe>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
    trait = std::is_assignable<mud::io::pipe, mud::io::pipe>::value;
    CPPUNIT_ASSERT_EQUAL(false, trait);
}

void
PipeTest::WriteReadFormattedCharacter()
{
    // Given A pipe
    // When  I write a character to the pipe
    // Then  I can read the same character

    mud::io::pipe testpipe;
    {
        char ch = 'C';
        testpipe.ostr() << ch << std::flush;
    }
    {
        char ch = 0;
        testpipe.istr() >> ch;
        CPPUNIT_ASSERT_EQUAL('C', ch);
    }
}

void
PipeTest::WriteReadFormattedInteger()
{
    // Given A pipe
    // When  I write an integer to the pipe
    // Then  I can read the same integer

    mud::io::pipe testpipe;
    {
        uint16_t i = 28197;
        testpipe.ostr() << i << std::flush;
    }
    {
        uint16_t i = 0;
        testpipe.istr() >> i;
        CPPUNIT_ASSERT_EQUAL((uint16_t)28197, i);
    }
}

void
PipeTest::WriteReadFormattedString()
{
    // Given A pipe
    // When  I write a string to the pipe
    // Then  I can read the same string

    mud::io::pipe testpipe;
    {
        std::string str = "Hello";
        testpipe.ostr() << str << std::flush;
    }
    {
        std::string str;
        testpipe.istr() >> str;
        CPPUNIT_ASSERT_EQUAL(std::string("Hello"), str);
    }
}

void
PipeTest::WriteReadFormattedCombination()
{
    // Given A pipe
    // When  I write a combination of a character and integer to the pipe
    // Then  I can read the same combination

    mud::io::pipe testpipe;
    {
        char ch = 'F';
        uint32_t  i  = 73618;
        testpipe.ostr() << ch << i << std::flush;
    }
    {
        char ch = 0;
        uint32_t  i  = 0;
        testpipe.istr() >> ch >> i;
        CPPUNIT_ASSERT_EQUAL('F', ch);
        CPPUNIT_ASSERT_EQUAL((uint32_t)73618, i);
    }
}

void
PipeTest::WriteReadUnformattedDataBlock()
{
    // Given A pipe
    // When  I write a binary sequence
    // Then  I can read the same binary sequence

    mud::io::pipe testpipe;
    {
        uint8_t block[] = {0x01, 0x92, 0x00, 0xF4};
        testpipe.ostr().write((const char*)block, sizeof(block)) << std::flush;
    }
    {
        uint8_t block[4];
        memset(block, 0, sizeof(block));
        testpipe.istr().read((char*)block, sizeof(block));
        CPPUNIT_ASSERT_EQUAL((uint8_t)0x01, block[0]);
        CPPUNIT_ASSERT_EQUAL((uint8_t)0x92, block[1]);
        CPPUNIT_ASSERT_EQUAL((uint8_t)0x00, block[2]);
        CPPUNIT_ASSERT_EQUAL((uint8_t)0xF4, block[3]);
    }
}

void
PipeTest::NonBlockingNothingToRead()
{
    // Given A non-blocking pipe
    // When  There is no data available
    // Then  Reading a character will fail

    mud::io::pipe testpipe;
    {
        char ch;
        testpipe.istr().read(&ch, 1);
        CPPUNIT_ASSERT_EQUAL(true, testpipe.istr().eof());
    }
}

/* vi: set ai ts=4 expandtab: */
