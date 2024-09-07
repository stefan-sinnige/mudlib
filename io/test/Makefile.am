check_PROGRAMS = mudio_test
TESTS = $(check_PROGRAMS)

mudio_test_SOURCES = \
    main.cpp \
    ip_address_test.cpp \
    pipe_test.cpp \
    tcp_test.cpp \
    udp_test.cpp

mudio_test_CPPFLAGS = \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../event/include \
	-I$(srcdir)/../../core/include \
	-I$(srcdir)/../../protocols/include \
	-I$(srcdir)/../../test/include \
	$(PTHREAD_CFLAGS)

mudio_test_LDADD = \
    ../libmudio.la \
    ../../event/libmudevent.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

