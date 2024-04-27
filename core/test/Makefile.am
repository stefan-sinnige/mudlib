check_PROGRAMS = mudcore_test
TESTS = $(check_PROGRAMS)

mudcore_test_SOURCES = \
    main.cpp \
    endian_test.cpp \
    handle_test.cpp \
    optional_test.cpp \
    poly_map_test.cpp \
    poly_vector_test.cpp \
    select_signal_test.cpp \
    task_test.cpp

mudcore_test_CPPFLAGS = \
    -I$(srcdir)/../include \
	-I$(srcdir)/../../test/include \
	$(PTHREAD_CFLAGS)

mudcore_test_LDADD = \
    ../libmudcore.la \
    ../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

