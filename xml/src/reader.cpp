#include "mud/xml.h"
#include "src/parser.hpp"
#include <utility>

#include <iostream>

/* External declations from the scanner */
extern struct yy_buffer_state*
yyxml_scan_string(const char*, yyscan_t);
extern int
yyxmllex_init(yyscan_t*);
extern int yyxmllex_destroy(yyscan_t);
extern void
yyxmlset_debug(int, yyscan_t);
extern void
yyxmlset_lineno(int, yyscan_t);
extern void
yyxmlset_column(int, yyscan_t);
extern void
yyxml_scan_stream(std::istream&, yyscan_t);

/* External declations from the reader */
extern int yyxmldebug;

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
operator>>(std::istream& istr, mud::xml::document& doc)
{
    /* Set-up scanning from an input-stream */
    xml_ctx_t ctx;
    xml_ctx_init(&ctx, "", NULL);
    yyxmllex_init(&(ctx.scanner));
    yyxml_scan_stream(istr, ctx.scanner);

    // Set the debugging options if it has been specified
    // if (istr.iword(__scanner_debug) == 1) {
    //     yyxmlset_debug(1, ctx.scanner);
    // }
    // if (istr.iword(__parser_debug) == 1) {
    //     yyxmldebug = 1;
    // }

    /* Parse into a document */
    int result = yyxmlparse(&ctx);
    yyxmllex_destroy(ctx.scanner);
    if (result != 0 || ctx.errors > 0) {
        /* Error parsing. */
        throw mud::xml::exception("XML parsing error");
    }

    /* Return the parsed document using move semantics */
    std::unique_ptr<mud::xml::document> doc_ptr(ctx.document);
    doc = std::move(*doc_ptr);
    return istr;
}

/* vi: set ai ts=4 expandtab: */
