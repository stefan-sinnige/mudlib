SUBDIRS = . test

lib_LTLIBRARIES = libmudsoap.la

libmudsoap_la_SOURCES = \
    src/message.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/soap.h

includemudsoapdir = $(includedir)/mud/soap
includemudsoap_HEADERS = \
    include/mud/soap/message.h \
    include/mud/soap/ns.h

libmudsoap_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir) \
    -I$(srcdir)/include \
	-I$(srcdir)/../include \
	-I$(srcdir)/../../core/include \
	-I$(srcdir)/../../xml/include

# vi: set ts=4 noexpandtab:

