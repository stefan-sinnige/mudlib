/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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
