bin_PROGRAMS = sample_tcp_client

sample_tcp_client_SOURCES = \
    main.cpp

sample_tcp_client_CPPFLAGS = \
    -I$(srcdir)/../../../../core/include \
    -I$(srcdir)/../../../../event/include \
    -I$(srcdir)/../../../../io/include 

sample_tcp_client_LDADD = \
    $(srcdir)/../../../../core/libmudcore.la \
    $(srcdir)/../../../../event/libmudevent.la\
    $(srcdir)/../../../../io/libmudio.la 

# vi: set ts=4 noexpandtab:

