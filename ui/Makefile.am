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
    -I$(srcdir)/include \
    -I$(srcdir)/../core/include \
    -I$(srcdir)/../event/include

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

