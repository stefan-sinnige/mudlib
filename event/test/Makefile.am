check_PROGRAMS = mudevent_test
TESTS = $(check_PROGRAMS)

mudevent_test_SOURCES = \
    main.cpp \
    event_test.cpp \
    event_loop_test.cpp \
    test_mechanism.cpp

mudevent_test_CPPFLAGS = \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../core/include \
	-I$(srcdir)/../../test/include

mudevent_test_LDADD = \
    ../libmudevent.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la \
	$(PTHREAD_CFLAGS)

# vi: set ts=4 noexpandtab:

