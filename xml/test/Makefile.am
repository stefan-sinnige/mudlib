check_PROGRAMS = mudxml_test
TESTS = $(check_PROGRAMS)

mudxml_test_SOURCES = \
    attribute_test.cpp \
    cdata_section_test.cpp \
    char_data_test.cpp \
    comment_test.cpp \
    declaration_test.cpp \
    document_test.cpp \
    element_test.cpp \
    main.cpp \
    namespace_test.cpp \
    processing_instruction_test.cpp \
    query_test.cpp \
    reader_test.cpp \
    writer_test.cpp

mudxml_test_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../core/include \
    -I$(srcdir)/../../test/include

mudxml_test_LDADD = \
    ../libmudxml.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la

# vi: set ts=4 noexpandtab:

