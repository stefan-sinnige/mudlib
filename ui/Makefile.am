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

lib_LTLIBRARIES = libmudui.la

libmudui_la_SOURCES = \
    src/application.cpp \
    src/control.cpp \
    src/event.cpp \
    src/handle.cpp \
    src/mechanism.cpp \
    src/pushbutton.cpp \
    src/task.cpp \
    src/window.cpp

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/ui.h

includemuduidir = $(includedir)/mud/ui
includemudui_HEADERS = \
    include/mud/ui/ns.h \
    include/mud/ui/application.h \
    include/mud/ui/exception.h \
    include/mud/ui/task.h

libmudui_la_CPPFLAGS = \
    -x objective-c++ \
    -I$(srcdir)/include \
    -I$(srcdir)/../core/include \
    -I$(srcdir)/../event/include

libmudui_la_LDFLAGS = \
    -framework Cocoa

# CPPFLAGS TODO:
#    APPLE:
#        -x objective-c++
#    WIN32:
#    X11:

# LDADD TODO:
#     APPLE:
#         -framework Cocoa
#     WIN32:
#         user32 comctl32
#     X11:
#         X11 Xft

# vi: set ts=4 noexpandtab:

