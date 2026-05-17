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

AC_PREREQ([2.69])
AC_INIT([mudlib],[1.0.0])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

# Add flag to indicate we are building the mudlib library
AC_DEFINE([MUDLIB_LIBRARY])

# We only use C++20 (or above) features
AX_CXX_COMPILE_STDCXX(20, , mandatory)

# Check of pthread
AX_PTHREAD()

# Limit the lex/yacc only to flex and bison (not using AC_PROG_YACC/AC_PROG_LEX)
# as we're using bison/flex specific directives.
AC_ARG_WITH([flex],
    [AS_HELP_STRING([--with-flex=PATH],[location of the flex binary])])
AS_IF([test "x$with_flex" = x],
    [AC_CHECK_PROGS(FLEX, flex)],
    [AC_MSG_CHECKING([for $with_flex])
     AS_IF([test -x $with_flex],
           [AC_MSG_RESULT([yes])
            FLEX=$with_flex],
           [AC_MSG_RESULT([no])])])
AC_MSG_CHECKING([$FLEX version])
FLEX_VERSION=$("$FLEX" --version | head -1 | rev | cut -d ' ' -f 1 | rev)
AX_COMPARE_VERSION([$FLEX_VERSION], [ge], [2.6],[],[
    AC_MSG_ERROR([$FLEX_VERSION found, should be at least 2.6])])
AC_MSG_RESULT($FLEX_VERSION)

AC_ARG_WITH([bison],
    [AS_HELP_STRING([--with-bison=PATH],[location of the bison binary])])
AS_IF([test "x$with_bison" = x],
    [AC_CHECK_PROGS(BISON, bison)],
    [AC_MSG_CHECKING([for $with_bison])
     AS_IF([test -x $with_bison],
           [AC_MSG_RESULT([yes])
            BISON=$with_bison],
           [AC_MSG_RESULT([no])])])
AC_MSG_CHECKING([$BISON version])
BISON_VERSION=$("$BISON" --version | head -1 | rev | cut -d ' ' -f 1 | rev)
AX_COMPARE_VERSION([$BISON_VERSION], [ge], [3.8],[],[
    AC_MSG_ERROR([$BISON_VERSION found, should be at least 3.8])])
AC_MSG_RESULT($BISON_VERSION)

# Documentation programs
#    * Add a '--enable-html' option to build the HTML packages. Only enable it
#      if required binaries are found.
AC_ARG_WITH([muddoc],
    [AS_HELP_STRING([--with-muddoc=PATH],[location of the muddoc binary])])
AC_ARG_WITH([xsltproc],
    [AS_HELP_STRING([--with-xsltproc=PATH],[location of the xsltproc binary])])
AC_ARG_ENABLE([html],
  [AS_HELP_STRING([--enable-html],
                  [enable generation of HTML documenatation])],
  [enable_html="yes"],
  [enable_html="no"])
AS_IF([test "x$enable_html" = "xyes"],[
    AS_IF([test "x$with_muddoc" = x],
        [AC_CHECK_PROGS(MUDDOC, muddoc)],
        [AC_MSG_CHECKING([for $with_muddoc])
         AS_IF([test -x $with_muddoc],
               [AC_MSG_RESULT([yes])
                MUDDOC=$with_muddoc],
               [AC_MSG_RESULT([no])])])
    AS_IF([test "x$with_xsltproc" = x],
        [AC_CHECK_PROGS(XSLTPROC, xsltproc)],
        [AC_MSG_CHECKING([for $with_xsltproc])
         AS_IF([test -x $with_xsltproc],
               [AC_MSG_RESULT([yes])
                XSLTPROC=$with_xsltproc],
               [AC_MSG_RESULT([no])])])
    AS_IF([test "x$MUDDOC" = "x" -o "x$XSLTPROC" = "x" ],[
        AC_MSG_WARN([Building of HTML documentation is disabled])
        enable_html="no"])])
    # MUDDOC_CXX_FLAGS=
    AC_SUBST([MUDDOC_CXX_FLAGS])
AM_CONDITIONAL([HTML], [test "x$enable_html" = "xyes"])

# Create the configuration files. Note that the UI related library and sample
# is not included.
AC_CONFIG_FILES([
    Makefile
    core/Makefile core/test/Makefile
    crypto/Makefile crypto/test/Makefile
    doc/Makefile
    html/Makefile html/test/Makefile
    html/src/entities_scanner_generator/Makefile
    io/Makefile io/test/Makefile
    mudlib.pc
    protocols/Makefile protocols/test/Makefile
    protocols/http/Makefile protocols/http/test/Makefile
    protocols/soap/Makefile protocols/soap/test/Makefile
    protocols/tls/Makefile protocols/tls/test/Makefile
    samples/Makefile
    samples/core/bicalc/Makefile
    samples/core/log/Makefile
    samples/crypto/Makefile
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
