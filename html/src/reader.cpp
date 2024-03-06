#include "src/parser.hpp"
#include "mud/html.h"
#include <utility>

#include <iostream>

/* External declations from the scanner */
extern struct yy_buffer_state*
yyhtml_scan_string(const char*, yyscan_t);
extern int
yyhtmllex_init(yyscan_t*);
extern int
yyhtmllex_destroy(yyscan_t);
extern void
yyhtmllex_init_states();
extern void
yyhtmlset_debug(int, yyscan_t);
extern void
yyhtmlset_lineno(int, yyscan_t);
extern void
yyhtmlset_column(int, yyscan_t);
extern void
yyhtml_scan_stream(std::istream&, yyscan_t);

/* External declations from the reader */
extern int yyhtmldebug;

/* Scanner and parser debug stream manipulators */
static int __scanner_debug = std::ios_base::xalloc();
static int __parser_debug = std::ios_base::xalloc();

BEGIN_MUDLIB_HTML_NS

std::ios_base&
scanner_debug(std::ios_base& istr)
{
    istr.iword(__scanner_debug) = 1;
    return istr;
}

std::ios_base&
parser_debug(std::ios_base& istr)
{
    istr.iword(__parser_debug) = 1;
    return istr;
}

END_MUDLIB_HTML_NS

std::istream&
operator>>(std::istream& istr, mud::html::document& doc)
{
    /* Set-up scanning from an input-stream */
    html_ctx_t ctx;
    html_ctx_init(&ctx, "", NULL);
    yyhtmllex_init_states();
    yyhtmllex_init(&(ctx.scanner));
    yyhtml_scan_stream(istr, ctx.scanner);

    // Set the debugging options if it has been specified
    // if (istr.iword(__scanner_debug) == 1) {
    //     yyhtmlset_debug(1, ctx.scanner);
    // }
    // if (istr.iword(__parser_debug) == 1) {
    //     yyhtmldebug = 1;
    // }

    /* Parse into a document */
    int result = yyhtmlparse(&ctx);
    yyhtmllex_destroy(ctx.scanner);
    if (result != 0 || ctx.errors > 0) {
        /* Error parsing. */
        throw mud::html::exception("HTML parsing error");
    }

    /* Return the parsed document using move semantics */
    std::unique_ptr<mud::html::document> doc_ptr(ctx.document);
    doc = std::move(*doc_ptr);
    return istr;
}

/* vi: set ai ts=4 expandtab: */
