SUBDIRS = . test

lib_LTLIBRARIES = libmudevent.la

libmudevent_la_SOURCES = \
    src/event.cpp \
    src/event_loop.cpp \
    src/event_mechanism.cpp \
    src/posix/select_mechanism.cpp 

includemuddir = $(includedir)/mud
includemud_HEADERS = \
    include/mud/event.h

includemudeventdir = $(includedir)/mud/event
includemudevent_HEADERS = \
    include/mud/event/ns.h \
    include/mud/event/event.h \
    include/mud/event/event_loop.h \
    include/mud/event/event_mechanism.h

libmudevent_la_CPPFLAGS = \
    -I$(srcdir)/include \
	-I$(srcdir)/../core/include \
	$(PTHREAD_CFLAGS)

# TODO: For windows add
# libmudevent_la_LIBADD = -lws2_32

# vi: set ts=4 noexpandtab:

