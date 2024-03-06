check_PROGRAMS = mudui_test
TESTS = $(check_PROGRAMS)

mudui_test_SOURCES = \
    main.cpp \
    application_test.cpp \
    pushbutton_test.cpp \
    window_test.cpp

mudui_test_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../core/include \
    -I$(srcdir)/../../event/include \
    -I$(srcdir)/../../test/include

mudui_test_LDADD = \
    ../libmudui.la \
    ../../core/libmudcore.la \
    ../../core/libmudevent.la \
    ../../test/libmudtest.la

# LDADD TODO:
#     APPLE:
#     WIN32:
#         -FORCE:MULTIPLE
#         ws2_32
#     X11:

# vi: set ts=4 noexpandtab:

