check_PROGRAMS = mudprotocols_test
TESTS = $(check_PROGRAMS)

mudprotocols_test_SOURCES = \
    main.cpp \
    mock_device.cpp \
    mock_communicators.cpp \
    communicator_test.cpp \
    layer_test.cpp

mudprotocols_test_CPPFLAGS = \
    -UVERSION \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../io/include \
	-I$(srcdir)/../../event/include \
	-I$(srcdir)/../../core/include \
	-I$(srcdir)/../../test/include \
	$(PTHREAD_CFLAGS)

mudprotocols_test_LDADD = \
    ../../io/libmudio.la \
    ../../event/libmudevent.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

