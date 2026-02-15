#
# ++ start-license-description ++
#
# Copyright (c) 2026 Stefan Sinnige.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ++ end-license-description ++
#

SUBDIRS = . test

lib_LTLIBRARIES = libmudio.la

libmudio_la_SOURCES = \
    src/exception.cpp \
    src/host.cpp \
    src/interface.cpp \
    src/pipe.cpp \
    src/streambuf.cpp \
    src/socket.cpp \
    src/ip.cpp \
    src/tcp.cpp \
    src/udp.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/io.h

includemudiodir = $(includedir)/mud/io
includemudio_HEADERS = \
    include/mud/io/exception.h \
    include/mud/io/host.h \
    include/mud/io/interface.h \
    include/mud/io/ip.h \
    include/mud/io/ns.h \
    include/mud/io/pipe.h \
    include/mud/io/socket.h \
    include/mud/io/streambuf.h \
    include/mud/io/tcp.h \
    include/mud/io/udp.h

libmudio_la_CPPFLAGS = \
    -I$(srcdir)/include \
	-I$(srcdir)/../event/include \
	-I$(srcdir)/../core/include \
	-I$(srcdir)/../protocols/include

# TODO: For windows add
# libmudio_la_LIBADD = -lws2_32

# vi: set ts=4 noexpandtab:

