SUBDIRS = . test

lib_LTLIBRARIES = libmudtest.la

libmudtest_la_SOURCES = \
    src/gherkin.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/test.h

includemudtestdir = $(includedir)/mud/test
includemudtest_HEADERS = \
    include/mud/test/ns.h \
    include/mud/test/exception.h \
    include/mud/test/gherkin.h

libmudtest_la_CPPFLAGS = \
    -I$(srcdir)/include

# vi: set ts=4 noexpandtab:

