SUBDIRS = . test

lib_LTLIBRARIES = libmudio.la

libmudio_la_SOURCES = \
    src/exception.cpp \
    src/pipe.cpp \
    src/streambuf.cpp \
    src/socket.cpp \
    src/posix/ip.cpp \
    src/posix/tcp.cpp \
    src/posix/udp.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/io.h

includemudiodir = $(includedir)/mud/io
includemudio_HEADERS = \
    include/mud/io/exception.h \
    include/mud/io/ip.h \
    include/mud/io/ns.h \
    include/mud/io/pipe.h \
    include/mud/io/streambuf.h \
    include/mud/io/tcp.h \
    include/mud/io/udp.h

libmudio_la_CPPFLAGS = \
    -I$(srcdir)/include \
	-I$(srcdir)/../event/include \
	-I$(srcdir)/../core/include

# TODO: For windows add
# libmudio_la_LIBADD = -lws2_32

# vi: set ts=4 noexpandtab:

