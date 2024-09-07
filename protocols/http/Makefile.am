SUBDIRS = . test

lib_LTLIBRARIES = libmudhttp.la

libmudhttp_la_SOURCES = \
    src/chunk.cpp \
    src/client.cpp \
    src/communicator.cpp \
    src/field.cpp \
    src/message.cpp \
    src/request.cpp \
    src/response.cpp \
    src/server.cpp \
    src/tokenise.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/http.h

includemudhttpdir = $(includedir)/mud/http
includemudhttp_HEADERS = \
    include/mud/http/chunk.h \
    include/mud/http/client.h \
    include/mud/http/communicator.h \
    include/mud/http/field.h \
    include/mud/http/message.h \
    include/mud/http/request.h \
    include/mud/http/response.h \
    include/mud/http/server.h

libmudhttp_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir) \
    -I$(srcdir)/include \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../io/include \
	-I$(srcdir)/../../event/include \
	-I$(srcdir)/../../core/include

# vi: set ts=4 noexpandtab:

