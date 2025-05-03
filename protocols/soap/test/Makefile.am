check_PROGRAMS = mudsoap_test
TESTS = $(check_PROGRAMS)

mudsoap_test_SOURCES = \
    main.cpp \
    message_test.cpp \
    writer_test.cpp

mudsoap_test_CPPFLAGS = \
    -UVERSION \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../include \
	-I$(srcdir)/../../../core/include \
	-I$(srcdir)/../../../test/include \
	-I$(srcdir)/../../../xml/include \
	$(PTHREAD_CFLAGS)

mudsoap_test_LDADD = \
    ../libmudsoap.la \
    ../../../xml/libmudxml.la \
    ../../../core/libmudcore.la \
    ../../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

