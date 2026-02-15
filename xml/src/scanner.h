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

#ifndef _MUDLIB_XML_SCANNER_H_
#define _MUDLIB_XML_SCANNER_H_

#include <istream>
#include <stack>

#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yyxmldebug;
#endif

/* Additional reentrant scanner context details. This allows for:
 *     - using std::istreams instead of FILE*
 *     - keep track of a stack of states
 * The element stack level keeps track how deep we are in the recursion of
 * the [39] element rules. This is used when another state needs to be entered
 * while we're already in a different state. When the state is returned, it
 * returns to the previous state.
 */
struct yyxmlextra {
    std::istream* istr;
    std::stack<int> saved_states;
};

/* External declations from the scanner */
extern struct yy_buffer_state* yyxml_scan_string(const char*, yyscan_t);
extern int yyxmllex_init(yyscan_t*);
extern int yyxmllex_init_extra(struct yyxmlextra*, yyscan_t*);
extern int yyxmllex_destroy(yyscan_t);
extern void yyxmlset_debug(int, yyscan_t);
extern void yyxmlset_lineno(int, yyscan_t);
extern void yyxmlset_column(int, yyscan_t);
extern void yyxml_scan_stream(std::istream&, yyscan_t);

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_SCANNER_H_ */

