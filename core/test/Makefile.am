check_PROGRAMS = mudcore_test
TESTS = $(check_PROGRAMS)

mudcore_test_SOURCES = \
    main.cpp \
    endian_test.cpp \
	event_test.cpp \
    event_loop_test.cpp \
    json_test.cpp \
    handle_test.cpp \
    log_test.cpp \
    notification_test.cpp \
    optional_test.cpp \
    poly_map_test.cpp \
    poly_vector_test.cpp \
    select_signal_test.cpp \
    task_test.cpp \
	test_mechanism.cpp \
    timer_test.cpp \
    uri_test.cpp \
    uuid_test.cpp

mudcore_test_CPPFLAGS = \
    -I$(srcdir)/../src \
    -I$(srcdir)/../include \
	-I$(srcdir)/../../test/include \
	$(PTHREAD_CFLAGS)

mudcore_test_LDADD = \
    ../libmudcore.la \
    ../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

