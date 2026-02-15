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

BISONFLAGS=--html=src/$(basename $(?F)).html
FLEXFLAGS=

lib_LTLIBRARIES = libmudxml.la

BUILT_SOURCES = \
    src/scanner.cpp \
    src/parser.cpp

libmudxml_la_SOURCES = \
    src/attribute.cpp \
    src/cdata_section.cpp \
    src/char_data.cpp \
    src/char_reference.cpp \
    src/comment.cpp \
    src/declaration.cpp \
    src/document.cpp \
    src/dom.cpp \
    src/element.cpp \
    src/node.cpp \
    src/namespace.cpp \
    src/processing_instruction.cpp \
    src/query.cpp \
    src/reader.cpp \
    src/writer.cpp \
    $(BUILT_SOURCES)

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/xml.h

includemudxmldir = $(includedir)/mud/xml
includemudxml_HEADERS = \
    include/mud/xml/attribute.h \
    include/mud/xml/cdata_section.h \
    include/mud/xml/char_data.h \
    include/mud/xml/char_reference.h \
    include/mud/xml/comment.h \
    include/mud/xml/declaration.h \
    include/mud/xml/document.h \
    include/mud/xml/dom.h \
    include/mud/xml/element.h \
    include/mud/xml/exception.h \
    include/mud/xml/namespace.h \
    include/mud/xml/ns.h \
    include/mud/xml/node.h \
    include/mud/xml/processing_instruction.h \
    include/mud/xml/query.h

libmudxml_la_CPPFLAGS = \
    -UVERSION \
    -I$(srcdir)/include \
    -I$(srcdir)/../core/include

src/parser.cpp src/parser.hpp: src/parser.y
	$(BISON) $(BISONFLAGS) $?

src/scanner.cpp: src/scanner.l
	$(FLEX) $(FLEXFLAGS) $?

# vi: set ts=4 noexpandtab:

