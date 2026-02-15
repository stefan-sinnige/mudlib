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

lib_LTLIBRARIES = libmudcore.la

libmudcore_la_SOURCES = \
    src/endian.cpp \
	src/event.cpp \
    src/event_loop.cpp \
    src/event_mechanism.cpp \
    src/handle.cpp \
    src/json.cpp \
    src/log.cpp \
    src/message.cpp \
    src/posix/select_mechanism.cpp \
	src/timer.cpp \
    src/timer_dispatcher.cpp \
    src/uri.cpp \
    src/utf.cpp \
    src/uuid.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/core.h

includemudcoredir = $(includedir)/mud/core
includemudcore_HEADERS = \
    include/mud/core/ns.h \
    include/mud/core/endian.h \
    include/mud/core/event.h \
    include/mud/core/event_loop.h \
    include/mud/core/event_mechanism.h \
    include/mud/core/exception.h \
    include/mud/core/factory.h \
    include/mud/core/handle.h \
    include/mud/core/json.h \
    include/mud/core/log.h \
    include/mud/core/message.h \
    include/mud/core/object.h \
    include/mud/core/optional.h \
    include/mud/core/poly_map.h \
    include/mud/core/poly_vector.h \
    include/mud/core/task.h \
    include/mud/core/timer.h \
    include/mud/core/uri.h \
    include/mud/core/utf.h \
    include/mud/core/uuid.h

libmudcore_la_CPPFLAGS = \
    -I$(srcdir)/include \
    -I$(srcdir)/src \
    $(PTHREAD_CFLAGS)

# vi: set ts=4 noexpandtab:

