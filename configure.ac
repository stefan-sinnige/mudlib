AC_PREREQ([2.71])
AC_INIT([mudlib],[1.0.0])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL

# Limit the lex/yacc only to flex and bison (not using AC_PROG_YACC/AC_PROG_LEX)
# as we're using bison/flex specific directives.
AC_CHECK_PROGS(FLEX, flex)
AC_CHECK_PROGS(BISON, bison)

# We only use C++17 (or above) features
AX_CXX_COMPILE_STDCXX(17, , mandatory)

# Documentation programs
#    * Add a '--enable-manual' option to build the manual (PDF)
AC_ARG_ENABLE([manual],
  [AS_HELP_STRING([--enable-manual],
                  [Enable generation of a manual. Default is disabled.])],
  [enable_manual="yes"],
  [enable_manual="no"])
AS_IF([test "x$enable_manual" = "xyes"],[
    AC_CHECK_PROG([PDFLATEX], [pdflatex], [pdflatex], [no])
    AC_CHECK_PROG([BIBER], [biber], [biber], [no])
    AS_IF([test "x$PDFLATEX" = "xno"], [
        AC_MSG_WARN([Building of manual disabled as pdflatex is not found])
        enable_manual="no"])
    AS_IF([test "x$BIBER" = "xno"], [
        AC_MSG_WARN([Building of manual disabled as biber is not found])
        enable_manual="no"])])
AM_CONDITIONAL([MANUAL], [test "x$enable_manual" = "xyes"])

# Create the configuration files. Note that the UI related library and sample
# is not included.
AC_CONFIG_FILES([
    Makefile
    core/Makefile core/test/Makefile
    doc/Makefile
    event/Makefile event/test/Makefile
    html/Makefile html/test/Makefile
    html/src/entities_scanner_generator/Makefile
    io/Makefile io/test/Makefile
    protocols/Makefile
    protocols/http/Makefile protocols/http/test/Makefile
    samples/Makefile
    samples/io/Makefile
    samples/io/tcp/Makefile
    samples/io/tcp/client/Makefile
    samples/io/tcp/server/Makefile
    samples/io/udp/Makefile
    samples/io/udp/client/Makefile
    samples/io/udp/server/Makefile
    samples/protocols/Makefile
    samples/protocols/http/Makefile
    samples/protocols/http/client/Makefile
    samples/protocols/http/server/Makefile
    samples/ui/Makefile
    samples/xml/Makefile
    test/Makefile test/test/Makefile
    xml/Makefile xml/test/Makefile
])
AC_OUTPUT
