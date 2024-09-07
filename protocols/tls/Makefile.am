SUBDIRS = . test

lib_LTLIBRARIES = libmudtls.la

libmudtls_la_SOURCES = \
    src/client.cpp \
    src/message.cpp \
    src/server.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/tls.h

includemudtlsdir = $(includedir)/mud/tls
includemudtls_HEADERS = \
    include/mud/tls/client.h \
    include/mud/tls/message.h \
    include/mud/tls/server.h

libmudtls_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir) \
    -I$(srcdir)/include \
	-I$(srcdir)/../../io/include \
	-I$(srcdir)/../../event/include \
	-I$(srcdir)/../../core/include

# vi: set ts=4 noexpandtab:

