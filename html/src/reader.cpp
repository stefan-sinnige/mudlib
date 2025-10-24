#include "src/parser.hpp"
#include "src/scanner.h"
#include "mud/html.h"
#include <utility>

#include <iostream>

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
    // Set-up scanning context
    html_ctx_t ctx;
    html_ctx_init(&ctx, "", NULL);
    yyhtmllex_init(&(ctx.scanner));

    // Use a reentrant extra structure
    struct yyhtmlextra extra;
    extra.istr = &istr;
    yyhtmllex_init_extra(&extra, &(ctx.scanner));

    // Set the debugging options if it has been specified
    if (istr.iword(__scanner_debug) == 1) {
        yyhtmlset_debug(1, ctx.scanner);
    }
    if (istr.iword(__parser_debug) == 1) {
#if YYDEBUG
        yyhtmldebug = 1;
#endif
    }

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
