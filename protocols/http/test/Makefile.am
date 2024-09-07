check_PROGRAMS = mudhttp_test
TESTS = $(check_PROGRAMS)

mudhttp_test_SOURCES = \
    main.cpp \
    message_test.cpp \
    message_10_test.cpp \
    message_11_test.cpp \
    http_test.cpp \
    http_10_test.cpp \
    http_11_test.cpp \
	mock.cpp

mudhttp_test_CPPFLAGS = \
    -UVERSION \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../include \
	-I$(srcdir)/../../../io/include \
	-I$(srcdir)/../../../event/include \
	-I$(srcdir)/../../../core/include \
	-I$(srcdir)/../../../test/include \
	$(PTHREAD_CFLAGS)

mudhttp_test_LDADD = \
    ../libmudhttp.la \
    ../../../io/libmudio.la \
    ../../../event/libmudevent.la \
    ../../../core/libmudcore.la \
    ../../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

