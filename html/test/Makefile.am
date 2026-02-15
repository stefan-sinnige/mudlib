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

check_PROGRAMS = mudhtml_test
TESTS = $(check_PROGRAMS)

mudhtml_test_SOURCES = \
    attribute_test.cpp \
    cdata_section_test.cpp \
    char_data_test.cpp \
    char_reference_test.cpp \
    comment_test.cpp \
    declaration_test.cpp \
    document_test.cpp \
    element_test.cpp \
    main.cpp \
    processing_instruction_test.cpp \
    raw_data_test.cpp \
    reader_test.cpp \
    writer_test.cpp

mudhtml_test_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../core/include \
    -I$(srcdir)/../../test/include

mudhtml_test_LDADD = \
    ../libmudhtml.la \
    ../../core/libmudcore.la \
    ../../test/libmudtest.la

# vi: set ts=4 noexpandtab:

