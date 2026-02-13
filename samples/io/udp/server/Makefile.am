bin_PROGRAMS = sample_udp_server

sample_udp_server_SOURCES = \
    main.cpp

sample_udp_server_CPPFLAGS = \
    -I$(srcdir)/../../../../core/include \
    -I$(srcdir)/../../../../io/include \
    -I$(srcdir)/../../../../protocols/include \
    $(PTHREAD_CFLAGS)

sample_udp_server_LDADD = \
    $(srcdir)/../../../../core/libmudcore.la \
    $(srcdir)/../../../../io/libmudio.la \
    $(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

