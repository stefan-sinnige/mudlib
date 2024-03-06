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

# Create the configuration files. Note that the UI related library and sample
# is not included.
AC_CONFIG_FILES([
    Makefile
    core/Makefile core/test/Makefile
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
