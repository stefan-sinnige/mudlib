bin_PROGRAMS = sample_udp_client

sample_udp_client_SOURCES = \
    main.cpp

sample_udp_client_CPPFLAGS = \
    -I$(srcdir)/../../../../core/include \
    -I$(srcdir)/../../../../event/include \
    -I$(srcdir)/../../../../io/include 

sample_udp_client_LDADD = \
    $(srcdir)/../../../../core/libmudcore.la \
    $(srcdir)/../../../../event/libmudevent.la\
    $(srcdir)/../../../../io/libmudio.la 

# vi: set ts=4 noexpandtab:

