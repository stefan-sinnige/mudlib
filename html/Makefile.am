SUBDIRS = . test

lib_LTLIBRARIES = libmudhtml.la

BUILT_SOURCES = \
    src/scanner.cpp \
    src/parser.cpp

libmudhtml_la_SOURCES = \
    src/attribute.cpp \
    src/cdata_section.cpp \
    src/char_data.cpp \
    src/char_reference.cpp \
    src/comment.cpp \
    src/declaration.cpp \
    src/document.cpp \
    src/element.cpp \
    src/entities.cpp \
    src/entities_scanner_table.cpp \
    src/processing_instruction.cpp \
    src/raw_data.cpp \
    src/reader.cpp \
    src/writer.cpp \
    $(BUILT_SOURCES)

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/html.h

includemudhtmldir = $(includedir)/mud/html
includemudhtml_HEADERS = \
    include/mud/html/attribute.h \
    include/mud/html/cdata_section.h \
    include/mud/html/char_data.h \
    include/mud/html/char_reference.h \
    include/mud/html/comment.h \
    include/mud/html/declaration.h \
    include/mud/html/document.h \
    include/mud/html/element.h \
    include/mud/html/node.h \
    include/mud/html/ns.h \
    include/mud/html/processing_instruction.h \
    include/mud/html/raw_data.h \
    include/mud/html/reader.h \
    include/mud/html/writer.h

libmudhtml_la_CPPFLAGS = \
    -I$(srcdir)/include \
	-I$(srcdir)/../core/include

src/parser.cpp src/parser.hpp: src/parser.y
	$(BISON) $?

src/scanner.cpp: src/scanner.l
	$(FLEX) $?

# vi: set ts=4 noexpandtab:

