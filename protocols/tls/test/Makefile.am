check_PROGRAMS = mudtls_test
TESTS = $(check_PROGRAMS)

mudtls_test_SOURCES = \
    main.cpp \
    message_test.cpp

mudtls_test_CPPFLAGS = \
    -UVERSION \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../../io/include \
	-I$(srcdir)/../../../event/include \
	-I$(srcdir)/../../../core/include \
	-I$(srcdir)/../../../test/include \
	$(PTHREAD_CFLAGS)

mudtls_test_LDADD = \
    ../libmudtls.la \
    ../../../io/libmudio.la \
    ../../../event/libmudevent.la \
    ../../../core/libmudcore.la \
    ../../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

