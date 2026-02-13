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
    include/mud/core/uuid.h

libmudcore_la_CPPFLAGS = \
    -I$(srcdir)/include \
    -I$(srcdir)/src \
    $(PTHREAD_CFLAGS)

# vi: set ts=4 noexpandtab:

