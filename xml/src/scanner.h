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

