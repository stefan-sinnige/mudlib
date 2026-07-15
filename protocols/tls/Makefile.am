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

lib_LTLIBRARIES = libmudtls.la

libmudtls_la_SOURCES = \
    src/communicator.cpp \
    src/dissect.cpp \
    src/extension.cpp \
    src/message.cpp

includemuddir = $(includedir)/mud/protocols
includemud_HEADERS = \
    include/mud/protocols/tls.h

includemudtlsdir = $(includedir)/mud/tls/protocols/tls
includemudtls_HEADERS = \
    include/mud/protocols/tls/common.h \
    include/mud/protocols/tls/communicator.h \
    include/mud/protocols/tls/dissect.h \
    include/mud/protocols/tls/exception.h \
    include/mud/protocols/tls/extension.h \
    include/mud/protocols/tls/message.h \
    include/mud/protocols/tls/ns.h \
    include/mud/protocols/tls/session.h

libmudtls_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir) \
    -I$(srcdir)/include \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../io/include \
    -I$(srcdir)/../../core/include \
    -I$(srcdir)/../../crypto/include

# vi: set ts=4 noexpandtab:

