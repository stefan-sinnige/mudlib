#include "mud/xml.h"
#include "src/parser.hpp"
#include "src/scanner.h"
#include <utility>
#include <iostream>

/* Scanner and parser debug stream manipulators */
static int __scanner_debug = std::ios_base::xalloc();
static int __parser_debug = std::ios_base::xalloc();

BEGIN_MUDLIB_XML_NS

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

END_MUDLIB_XML_NS

std::istream&
operator>>(std::istream& istr, mud::xml::document::ptr& doc)
{
    // Set-up scanning context
    xml_ctx_t ctx;
    xml_ctx_init(&ctx, "", NULL);
    yyxmllex_init(&(ctx.scanner));

    // Use a reentrant extra structure
    struct yyxmlextra extra;
    extra.istr = &istr;
    yyxmllex_init_extra(&extra, &(ctx.scanner));

    // Set the debugging options if it has been specified
    if (istr.iword(__scanner_debug) == 1) {
        yyxmlset_debug(1, ctx.scanner);
    }
    if (istr.iword(__parser_debug) == 1) {
#if YYDEBUG
        yyxmldebug = 1;
#endif
    }

    /* Parse into a document */
    int result = yyxmlparse(&ctx);
    yyxmllex_destroy(ctx.scanner);
    if (result != 0 || ctx.errors > 0) {
        /* Error parsing. */
        throw mud::xml::exception("XML parsing error");
    }

    /* Return the parsed document using move semantics */
    doc = ctx.document;
    doc->resolve();
    return istr;
}

/* vi: set ai ts=4 expandtab: */
