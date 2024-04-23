bin_PROGRAMS = sample_http_server

sample_http_server_SOURCES = \
    main.cpp

sample_http_server_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir)/../../../../core/include \
    -I$(srcdir)/../../../../event/include \
    -I$(srcdir)/../../../../io/include \
    -I$(srcdir)/../../../../protocols/http/include \
	$(PTHREAD_CFLAGS)

sample_http_server_LDADD = \
    $(srcdir)/../../../../core/libmudcore.la \
    $(srcdir)/../../../../event/libmudevent.la\
    $(srcdir)/../../../../io/libmudio.la \
    $(srcdir)/../../../../protocols/http/libmudhttp.la \
	$(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

