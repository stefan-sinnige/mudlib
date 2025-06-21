AC_PREREQ([2.69])
AC_INIT([mudlib],[1.0.0])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

# Add flag to indicate we are building the mudlib library
AC_DEFINE([MUDLIB_LIBRARY])

# We only use C++17 (or above) features
AX_CXX_COMPILE_STDCXX(17, , mandatory)

# Check of pthread
AX_PTHREAD()

# Limit the lex/yacc only to flex and bison (not using AC_PROG_YACC/AC_PROG_LEX)
# as we're using bison/flex specific directives.
AC_CHECK_PROGS(FLEX, flex)
AC_MSG_CHECKING([$BISON version])
FLEX_VERSION=$("$FLEX" --version | head -1 | rev | cut -d ' ' -f 1 | rev)
AX_COMPARE_VERSION([$FLEX_VERSION], [ge], [2.6],[],[
    AC_MSG_ERROR([$FLEX_VERSION found, should be at least 2.6])])
AC_MSG_RESULT($FLEX_VERSION)

AC_CHECK_PROGS(BISON, bison)
AC_MSG_CHECKING([$BISON version])
BISON_VERSION=$("$BISON" --version | head -1 | rev | cut -d ' ' -f 1 | rev)
AX_COMPARE_VERSION([$BISON_VERSION], [ge], [3.8],[],[
    AC_MSG_ERROR([$BISON_VERSION found, should be at least 3.8])])
AC_MSG_RESULT($BISON_VERSION)

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
    mudlib.pc
    protocols/Makefile protocols/test/Makefile
    protocols/http/Makefile protocols/http/test/Makefile
    protocols/soap/Makefile protocols/soap/test/Makefile
    protocols/tls/Makefile protocols/tls/test/Makefile
    samples/Makefile
    samples/core/log/Makefile
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
