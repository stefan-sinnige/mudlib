check_PROGRAMS = mudhtml_test
TESTS = $(check_PROGRAMS)

mudhtml_test_SOURCES = \
    attribute_test.cpp \
    cdata_section_test.cpp \
    char_data_test.cpp \
    char_reference_test.cpp \
    comment_test.cpp \
    declaration_test.cpp \
    document_test.cpp \
    element_test.cpp \
    main.cpp \
    processing_instruction_test.cpp \
    raw_data_test.cpp \
    reader_test.cpp \
    writer_test.cpp

mudhtml_test_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../core/include \
    -I$(srcdir)/../../test/include

mudhtml_test_LDADD = \
    ../libmudhtml.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la

# vi: set ts=4 noexpandtab:

