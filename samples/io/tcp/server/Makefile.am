bin_PROGRAMS = sample_tcp_server

sample_tcp_server_SOURCES = \
    main.cpp

sample_tcp_server_CPPFLAGS = \
    -I$(srcdir)/../../../../core/include \
    -I$(srcdir)/../../../../event/include \
    -I$(srcdir)/../../../../io/include 

sample_tcp_server_LDADD = \
    $(srcdir)/../../../../core/libmudcore.la \
    $(srcdir)/../../../../event/libmudevent.la\
    $(srcdir)/../../../../io/libmudio.la 

# vi: set ts=4 noexpandtab:

