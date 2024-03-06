SUBDIRS = . test

lib_LTLIBRARIES = libmudhttp.la

libmudhttp_la_SOURCES = \
    src/message.cpp \
    src/message_deserialise.cpp \
    src/message_serialise.cpp \
    src/server.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/http.h

includemudhttpdir = $(includedir)/mud/http
includemudhttp_HEADERS = \
    include/mud/http/message.h \
    include/mud/http/server.h

libmudhttp_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir)/include \
	-I$(srcdir)/../../io/include \
	-I$(srcdir)/../../event/include \
	-I$(srcdir)/../../core/include

# vi: set ts=4 noexpandtab:

