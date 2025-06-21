bin_PROGRAMS = sample_log

sample_log_SOURCES = \
    main.cpp

sample_log_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir)/../../../core/include \
    $(PTHREAD_CFLAGS)

sample_log_LDADD = \
    $(srcdir)/../../../core/libmudcore.la \
    $(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

# vi: set ts=4 noexpandtab:

