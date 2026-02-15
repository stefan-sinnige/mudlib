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

%option reentrant
%option bison-bridge
%option bison-locations
%option caseless
%option noyywrap
%option yylineno
%option outfile="src/scanner.cpp"
%option prefix="yyhtml"
%option extra-type="struct yyhtmlextra*"

%{
/* Verified with Flex 2.6
 *
 * In conjunction with xml.y, this implements the HTML standard:
 *    https://html.spec.whatwg.org/
 * but not through the proposed implementation. The grammar provided here
 * is stricter.
 *
 * There are similarities to the XML specification!
 */

/* Additional C++ statements */
#include <stdio.h>
#include <ctype.h>
#include <istream>
#include <stack>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <mud/html.h>
#include "src/parser.hpp"
#include "src/scanner.h"

/* Update the column */
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; \
    yylloc->last_column = yycolumn + yyleng - 1; \
    yycolumn += (int)yyleng;

/* Redefine YY_INPUT to use std::istream, instead of the standard FILE*. This
 * does render the yyhtml_set_in useless, unless we cater for that here. 
 * Don't do buffering here,but rely on the buffering within the std::istream
 * instead. */
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
    { \
        char c; \
        yyextra->istr->get(c); \
        result = !(*(yyextra->istr)) ? YY_NULL : (buf[0] = c, 1); \
    }

/* Push/Pop a state. This is used to have stack-level control of the scanning
 * state. */
#define PUSH(STATE) yyhtmllex_push_state(yyg, STATE, __LINE__)
#define POP() yyhtmllex_pop_state(yyg, __LINE__)

void yyhtmllex_push_state(struct yyguts_t* yyg, int state, int linenr);
void yyhtmllex_pop_state(struct yyguts_t* yyg, int linenr);


%}

CharExceptDash        [^-]
CharExceptSqClose     [^[]
CharExceptSmaller     [^<]
CharExceptGreater     [^>]
CharExceptSlash       [^/]
Letter                [A-Za-z]
NameChar              [A-Za-z0-9:-]
Name                  {Letter}{NameChar}*
CDStart               <!\[CDATA\[
CDEnd                 \]\]>
S                     [\x20\x09\x0D\x0A]+

%x comment doctype stag attribute attvalue attvalue_dquote attvalue_squote etag
%x content void_content raw_data char_data  cdsect

%%

        /*
         * The comment is between "<!--" and "-->". Furthermore, a "--" is not
         * allowed. Everything between these two tokens is captured.
         */
<*>"<!--" {
            PUSH(comment);
            return(COMMENTOPEN);
        }
<comment>({CharExceptDash}|(\-{CharExceptDash}))*    {
            yylval->str = new std::string(yytext);
            return(COMMENT);
        }
<comment>"-->" {
            POP();
            return(COMMENTCLOSE);
        }
        /*
         * The document type is a simplied XML document type. There is no
         * markup declarations defined.
         */
"<!DOCTYPE" {
            PUSH(doctype);
            return(DOCTYPE);
        }
<doctype>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<doctype>\' {
            return(SQUOTE);
        }
<doctype>\" {
            return(DQUOTE);
        }
<doctype>\> {
            POP();
            return(CLOSETAG);
        }
        /*
         * Elements can be entered from the initial or content states. The
         * states stack for an element:
         *        <tag att="value">content</tag>
         *         ----------------------------    stag
         *            -------------------- ----    content* / etag
         *            ------------                 attribute
         *                  ------                 attvalue_dquote
         * based on the tag, the content* might be a special type of content,
         * like raw_data text. That's why the specific content state is pushed
         * right after the tag has been processed as at that time we know it is
         * should be raw_data, char_data or normal content. A simple pop from
         * the attribute will then automatically use the appropriate content
         * state.
         */
<INITIAL,content>\< {
            PUSH(stag); 
            return(OPENTAG);
        }
<stag,etag>"html" {
            if (YY_START == stag) {
                PUSH(content);
                PUSH(attribute);
            }
            return(HTML);
        }
<stag,etag>"script" {
            if (YY_START == stag) {
                PUSH(raw_data);
                PUSH(attribute);
            }
            return(SCRIPT);
        }
<stag,etag>"style" {
            if (YY_START == stag) {
                PUSH(raw_data);
                PUSH(attribute);
            }
            return(STYLE);
        }
<stag,etag>"textarea" {
            if (YY_START == stag) {
                PUSH(char_data);
                PUSH(attribute);
            }
            return(TEXTAREA);
        }
<stag,etag>"title" {
            if (YY_START == stag) {
                PUSH(char_data);
                PUSH(attribute);
            }
            return(TITLE);
        }
        /*
         * Void tags only have a start tag, no contents and no end-tag, but
         * may have attributes.
         */
<stag>"area" {
            PUSH(void_content);
            PUSH(attribute);
            return(AREA);
        }
<stag>"base" {
            PUSH(void_content);
            PUSH(attribute);
            return(BASE);
        }
<stag>"br" {
            PUSH(void_content);
            PUSH(attribute);
            return(BR);
        }
<stag>"col" {
            PUSH(void_content);
            PUSH(attribute);
            return(COL);
        }
<stag>"embed" {
            PUSH(void_content);
            PUSH(attribute);
            return(EMBED);
        }
<stag>"hr" {
            PUSH(void_content);
            PUSH(attribute);
            return(HR);
        }
<stag>"img" {
            PUSH(void_content);
            PUSH(attribute);
            return(IMG);
        }
<stag>"input" {
            PUSH(void_content);
            PUSH(attribute);
            return(INPUT);
        }
<stag>"link" {
            PUSH(void_content);
            PUSH(attribute);
            return(LINK);
        }
<stag>"meta" {
            PUSH(void_content);
            PUSH(attribute);
            return(META);
        }
<stag>"param" {
            PUSH(void_content);
            PUSH(attribute);
            return(PARAM);
        }
<stag>"source" {
            PUSH(void_content);
            PUSH(attribute);
            return(SOURCE);
        }
<stag>"track" {
            PUSH(void_content);
            PUSH(attribute);
            return(TRACK);
        }
<stag>"wbr" {
            PUSH(void_content);
            PUSH(attribute);
            return(WBR);
        }
<raw_data>({CharExceptSmaller}|(<{CharExceptSlash}))* {
            yylval->str = new std::string(yytext);
            return(RAW);
        }
<raw_data>"</" {
            POP();
            PUSH(etag);
            return(OPENTAGEND);
        }
<char_data>{CharExceptSmaller}+ {
            std::string unescaped = mud::html::char_reference::unescape(yytext);
            yylval->str = new std::string(std::move(unescaped));
            return(CHARDATA);
        }
<char_data>"</" {
            POP();
            PUSH(etag);
            return(OPENTAGEND);
        }
<stag>{Name} {
            PUSH(content);
            PUSH(attribute);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<attribute>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<attribute>\= {
            PUSH(attvalue);
            return(EQ);
        }
<attvalue>\" {
            POP();
            PUSH(attvalue_dquote);
            return(DQUOTE);
        }
<attvalue>\' {
            POP();
            PUSH(attvalue_squote);
            return(SQUOTE);
        }
<attvalue>[^\x09\x0A\x0A\x20\x3E\x22\x27\x3C\x3D\x60]+ {
            std::string unescaped = mud::html::char_reference::unescape(yytext);
            yylval->str = new std::string(std::move(unescaped));
            POP();
            return(ATTVALUE);
        }
<attvalue_dquote>[^"]* {
            std::string unescaped = mud::html::char_reference::unescape(yytext);
            yylval->str = new std::string(std::move(unescaped));
            return(ATTVALUE);
        }
<attvalue_squote>[^']* {
            std::string unescaped = mud::html::char_reference::unescape(yytext);
            yylval->str = new std::string(std::move(unescaped));
            return(ATTVALUE);
        }
<attvalue_dquote>\" {
            POP(); 
            return(DQUOTE);
        }
<attvalue_squote>\' {
            POP();
            return(SQUOTE);
        }
<attribute>"/>" {
            POP(); /* attvalue_xquote */
            POP(); /* attribute */
            POP(); /* element */
            return(EMPTYCLOSETAG);
        }
<attribute>\> {
            POP();
            if (YY_START == void_content) {
                POP();
                POP();
            }
            return(CLOSETAG);
        }
<content>"</" {
            POP();
            PUSH(etag);
            return(OPENTAGEND);
        }
<etag>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<etag>\> {
            POP();
            POP();
            return(CLOSETAG);
        }
        /*
         * Character data content
         */
<content>[^<]* {
            std::string unescaped = mud::html::char_reference::unescape(yytext);
            yylval->str = new std::string(std::move(unescaped));
            return(CHARDATA);
        }
<content>{CDStart} {
            PUSH(cdsect);
            return(CDSTART);
        }
<cdsect>({CharExceptSqClose}|(\[{CharExceptSqClose})|(\[\[{CharExceptGreater}))* {
            yylval->str = new std::string(yytext);
            return(CDATA);
        }
<cdsect>{CDEnd} {
            POP();
            return(CDEND);
        }
        /*
         * White space can be at numerous places and is therefore shared
         * among all the states. It is not returned as a token.
         */
<*>{S} {
            //return(S);
        }

%%

/*
 * Keep track of the scanner states:
 *    - The state at the front of the queue is the current scanning state
 *    - If the queue is empty, the current scanning state is INITIAL (0)
 */

void
yyhtmllex_print_states(struct yyguts_t* yyg, int linenr)
{
    bool top = true;
    std::stack<int> copy = yyextra->saved_states;
    std::stack<int> reverse;
    while (!copy.empty()) {
        reverse.push(copy.top());
        copy.pop();
    }
    fprintf(stderr, "--scan stack at line %d (", linenr);
    while (!reverse.empty())
    {
        fprintf(stderr, " %d", reverse.top());
        reverse.pop();
    }
    fprintf(stderr, " )\n");
}

void
yyhtmllex_push_state(struct yyguts_t* yyg, int state, int linenr)
{
    yyextra->saved_states.push(state);
    BEGIN(state);
    if (yy_flex_debug) {
        yyhtmllex_print_states(yyg, linenr);
    }
}

void
yyhtmllex_pop_state(struct yyguts_t* yyg, int linenr)
{
    if (yyextra->saved_states.size()) {
        yyextra->saved_states.pop();
    }
    BEGIN(yyextra->saved_states.size() ? yyextra->saved_states.top() : INITIAL);
    if (yy_flex_debug) {
        yyhtmllex_print_states(yyg, linenr);
    }
    
}
/* vi: set ai ts=4 expandtab: */

