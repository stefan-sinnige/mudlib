SUBDIRS = . test

lib_LTLIBRARIES = libmudcore.la

libmudcore_la_SOURCES = \
    src/endian.cpp \
    src/handle.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/core.h

includemudcoredir = $(includedir)/mud/core
includemudcore_HEADERS = \
    include/mud/core/ns.h \
    include/mud/core/exception.h \
    include/mud/core/handle.h \
    include/mud/core/poly_map.h \
    include/mud/core/poly_vector.h \
    include/mud/core/task.h

libmudcore_la_CPPFLAGS = \
    -I$(srcdir)/include \
	$(PTHREAD_CFLAGS)

# vi: set ts=4 noexpandtab:

