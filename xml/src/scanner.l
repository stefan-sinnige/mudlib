%option outfile="src/scanner.cpp"
%option prefix="yyxml"

%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno

%option extra-type="struct yyxmlextra"

%{
/* Verified with Flex 2.6 
 *
 * In conjunction with xml.y, this implements the following standard:
 *    Extensible Markup Language (XML) 1.0 (Fifth Edition)
 * Available at: https://www.w3.org/TR/xml/
 */

/* Additional C++ statements */
#include <stdio.h>
#include <ctype.h>
#include <istream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <mud/xml.h>
#include "src/parser.hpp"

/* Additional reentrant scanner context details. This allows for:
 *     - using std::istreams instead of FILE*
 *     - keep track of a stack of states
 * The element stack level keeps track how deep we are in the recursion of
 * the [39] element rules. This is used when another state needs to be entered
 * while we're already in a different state. When the state is returned, it
 * returns to the previous state. Only one level is currently allowed.
 */
struct yyxmlextra {
    std::istream* istr;
    int element_stack_level;
    int saved_state;
};

/* Update the column */
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; \
    yylloc->last_column = yycolumn + yyleng - 1; \
    yycolumn += (int)yyleng;

/* Redefine YY_INPUT to use std::istream, instead of the standard FILE*. This
 * does render the yyxml_set_in useless, unless we cater for that here. 
 * Don;t do buffering here,but rely on the buffering within the std::istream
 * instead. */
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
    { \
        char c; \
        yyextra.istr->get(c); \
        result = !(*(yyextra.istr)) ? YY_NULL : (buf[0] = c, 1); \
    }

/* Push/Pop a state. This is used when another state needs to be entered
 * while we're already in a different state. When the state is returned, it
 * returns to the previous state. Only one level is currently allowed. */
#define PUSH_STATE() yyextra.saved_state = YY_START
#define POP_STATE()  BEGIN(yyextra.saved_state)

%}

        /*
         * [2] Char : #x09 | #x0A | #x0D [#x20-#xFF] 
         *     CharExceptDash    : Char - '-'
         *     CharExceptQuest   : Char - '?'
         *     CharExceptGreat   : Char - '>'
         *       CharExceptSqClose : Char - ']'
         */
Char              [\x09\x0A\x0D\x20-\xFF]
CharExceptDash      [\x09\x0A\x0D\x20-\x2C\x2E-\xFF]
CharExceptQuest      [\x09\x0A\x0D\x20-\x3E\x40-\xFF]
CharExceptGreat   [\x09\x0A\x0D\x20-\x3D\x3F-\xFF]
CharExceptSqClose [\x09\x0A\x0D\x20-\x5C\x5E-\xFF]
        /*
         * [3] S : (#x20 | #x09 | #x0D | #x0A)+
         */
S                [\x20\x09\x0D\x0A]+
        /*
         * [4] NameChar : Letter | Digit | '.' | '-' | '_' | ':' | Extender
         */
NameChar        {Letter}|{Digit}|[\.\-_:]|{Extender}
        /*
         * [5] Name : (Letter | '_' | ':') (NameChar)*
         */
Name            ({Letter}|[_:]){NameChar}*
        /*
         * [6] Names : Name ( S Name)*
         */
Names            {Name}({S}{Name})*
        /*
         * [7] NmToken : (NameChar)+
         */
Nmtoken            {NameChar}+
        /*
         * [8] Nmtokens : Nmtoken (S Nmtoken)*
         */
Nmtokens        {Nmtoken}({S}{Nmtoken})*
        /*
         * [11] SystemLiteral : ('"' [^"]* '"') | (''' [^']* ''')
         */
SystemLiteral    (\"[^"]*\")|(\'[^']*\')
        /*
         * [12] PubidLiteral : ('"' PubidChar* '"') | (''' (PubidChar - '-')* ''')
         */
PubidLiteral    (\"{PubidChar}*\")|(\'{PubidCharExceptSQuot}\')
        /*
         * [13] PubidChar : #x20 | #x0D | #x0A | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
         *      PubidCharExceptSQuot : PubidChar - '''
         */
PubidChar                [\x20\x0D\x0Aa-zA-Z0-9\-'()+,\./:=?;!#@$_%]
PubidCharExceptSQuot    [\x20\x0D\x0Aa-zA-Z0-9\-()+,\./:=?;!#@$_%]
        /*
         * [19] CDStart : '<![CDATA['
         */
CDStart            <!\[CDATA\[
        /*
         * [21] CDEnd : ']]>'
         */
CDEnd            \]\]>
        /*
         * [26] VersionNum : ([a-zA-Z0-9_\.:] | '-')+
         */
VersionNum        ([a-zA-Z0-9_\.:]|\-)+
        /*
         * [55] StringType : 'CDATA'
         */
StringType        CDATA
        /*
         * [56] TokenizedType : 'ID' | 'IDREF' | 'IDREF' | 'ENTITY' | 'ENTITIES' |
         *                      NMTOKEN | NMTOKENS
         */
TokenizedType    ID|IDREF|IDREFS|ENTITY|ENTITIES|NMTOKEN|NMTOKENS
        /*
         * [66] CharRef : '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
         */
CharRef            (&#[0-9]+;)|(&#x[0-9a-fA-F]+;)
        /*
         * [68] EntityRef : '&' Name ';'
         */
EntityRef        &{Name};
        /*
         * [69] PEReference : '%' Name ';'
         */
PEReference        %{Name};
        /*
         * [81] EncName : [a-zA-Z] ([a-zA-Z0-9._]| '-')*
         */
EncName            [a-zA-Z][a-zA-Z0-9\._\-]*
        /*
         * [84] Letter : BaseChar
         */
Letter            {BaseChar}
        /*
         * [85] BaseChar : [#x41-#x5A] | [#x61-#x7A] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#xFF]
         */
BaseChar        [\x41-\x5A\x61-\x7A\xC0-\xD6\xF8-\xFF]
        /*
         * [88] Digit : [#x30-#x39]
         */
Digit            [\x30-\x39]
        /*
         * [89] : Extender : #xB7
         */
Extender        \xB7

%x xmldecl 
%x versioninfo encoding standalone doctype doctype2 markupdecl publicextent systemextent
%x elementdecl contentspec mixedchildren
%x attlistdecl attdef atttype defaultdecl attvalue_dquote attvalue_squote notationtype 
%x entitydecl entitydecldef entityvalue_dquote entityvalue_squote ndatadecl
%x notationdecl notationdecl2 
%x enumeration cdsect
%x pitarget pidata comment
%x element attribute content etag

%%

        /*
         * [10] AttValue
         * The attribute value can be reached from the defaultdecl and
         * attribute startstates. The attribute value is entered by a single or
         * double quote and reads everything until the matching single or double
         * quote is scanned. The state is then restored to the state its
         * originated from.
         */
<attribute,defaultdecl>\" {
            PUSH_STATE();
            BEGIN(attvalue_dquote);
            return (DQUOTE);
        }
<attvalue_dquote>[^"]* {
            yylval->str = new std::string(yytext);
            return(ATTVALUE);
        }
<attvalue_dquote>\" {
            POP_STATE();
            return(DQUOTE);
        }
<attribute,defaultdecl>\' {
            PUSH_STATE();
            BEGIN(attvalue_squote);
            return (SQUOTE);
        }
<attvalue_squote>[^']* {
            yylval->str = new std::string(yytext);
            return(ATTVALUE);
        }
<attvalue_squote>\' {
            POP_STATE();
            return(SQUOTE);
        }
        /*
         * [15] Comment
         * The comment is between "<!--" and "-->". Furthermore, a "--" is not
         * allowed. Everything between these two tokens is captured and returned.
         */
<*>"<!--" {
            PUSH_STATE();
            BEGIN(comment);
            return(COMMENTOPEN);
        }
<comment>({CharExceptDash}|(\-{CharExceptDash}))*    {
            yylval->str = new std::string(yytext);
            return(COMMENT);
        }
<comment>"-->" {
            POP_STATE();
            return(COMMENTCLOSE);
        }
        /*
         * [18] CDSect
         * This is only as part of the content state and is enclosed by the
         * '<!CDATA[' and ']]>'. Everything captured between two tokens is
         * captured and returned.
         */
<content>{CDStart} {
            BEGIN(cdsect);
            return(CDSTART);
        }
<cdsect>({CharExceptSqClose}|(\[{CharExceptSqClose})|(\[\[{CharExceptGreat}))* {
            yylval->str = new std::string(yytext);
            return(CDATA);
        }
<cdsect>{CDEnd} {
            BEGIN(content);
            return(CDEND);
        }
        /*
         * [23] XMLDecl
         * This special processing instruction enters the 'xmldecl' start state. From
         * this point only the VersionInfo, EncodingDecl and SDDecl can be scanned.
         * This will be ended by the <xmldecl>"&>"
         */
"<?xml"    {
            BEGIN(xmldecl);
            return(XMLPIOPEN);
        }
        /*
         * [24] VersionInfo
         * This is only as part of a XMLDecl, hence its 'xmldecl' start state. From here
         * it will accept the 'version' string, followed by the VersionNum.
         */ 
<xmldecl>"version" {
            BEGIN(versioninfo);
            return(VERSION);
        }
<versioninfo>{VersionNum} {
            BEGIN(xmldecl);
            yylval->str = new std::string(yytext);
            return(VERSIONNUM);
        }
        /*
         * [25] Eq
         * This can be entered at various places.
         */
<versioninfo,standalone,attribute,encoding>\= {
            return(EQ);
        }
        /*
         * [80] EncodingDecl
         * This is only as part of a XMLDecl, hence its 'xmldecl' start state. From here
         * it will accept the 'encoding' string, followed by the EncName.
         */
<xmldecl>"encoding" {
            BEGIN(encoding);
            return(ENCODING);
        }
<encoding>{EncName} {
            BEGIN(xmldecl);
            yylval->str = new std::string(yytext);
            return(ENCNAME);
        }
        /*
         * [32] SDDecl
         * This is only as part of a XMLDecl, hence its 'xmldecl' start state. From here
         * it will accept the 'standalone' string, followed by eitehr 'yes' or 'no'.
         */
<xmldecl>"standalone"    {
            BEGIN(standalone);
            return(STANDALONE);
        }
<standalone>("yes"|"no") {
            BEGIN(xmldecl);
            yylval->str = new std::string(yytext);
            return(YESNO);
        }
<xmldecl>"?>"    {
            BEGIN(0);
            return(PICLOSE);
        }
        /*
         * [16] PI
         * This can be part of the INITIAL state (the prolog) or the doctype2 state. From
         * here we accept the target and data parts.
         */
<INITIAL,markupdecl,content>"<?" {
            PUSH_STATE();
            BEGIN(pitarget);
            return(PIOPEN);
        }
<pitarget>{Name} {
            yylval->str = new std::string(yytext);
            BEGIN(pidata);
            return(PITARGET);
        }
<pidata>({CharExceptQuest}|(\?{CharExceptGreat}))* {
            yylval->str = new std::string(yytext);
            return(PIDATA);
        }
<pidata>"?>"    {
            POP_STATE();
            return(PICLOSE);
        }
        /*
         * [28] doctypedecl
         * From here we can accept the ExternalID, markupdecl and DeclSep. All will
         * enter their own start state to control specific scanning.
         */
"<!DOCTYPE"    {
            BEGIN(doctype);
            return(DOCTYPE);
        }
<doctype>{Name} {
            BEGIN(doctype2);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<doctype2>\> {
            BEGIN(0);
            return(CLOSETAG);
        }
        /*
         * [75] ExternalID
         * This state can be reached from the doctype2 and the notation2 start states.
         * From here we can enter the SYSTEM and PUBLIC external entities. Both must
         * end with the SystemLiteral.
         */
<doctype2,notationdecl2,entitydecldef>"SYSTEM" {
            PUSH_STATE();
            BEGIN(systemextent);
            return(SYSTEM);
        }
<doctype2,notationdecl2,entitydecldef>"PUBLIC" {
            PUSH_STATE();
            BEGIN(publicextent);
            return(PUBLIC);
        }
<systemextent>{SystemLiteral} {
            POP_STATE();
            yylval->str = new std::string(yytext);
            return(SYSTEMLITERAL);
        }
<publicextent>{PubidLiteral} {
            BEGIN(systemextent);
            yylval->str = new std::string(yytext);
            return(PUBIDLITERAL);
        }
        /*
         * [28a] DeclSep
         * This one is only as part of the markupdecl state.
         */
<markupdecl>{PEReference} {
            yylval->str = new std::string(yytext);
            return(PEREFERENCE);
        }
        /* 
         * [29] markupdecl
         * The start and end are marked with square brackets. This declaration is only
         * reachable from the doctype2 state. In this state we accept a variety of
         * declarations: [45] elementdecl, [52] AttlistDecl, [70] EntityDecl, [82]
         * NotationDecl. Also, the [16] PI, [15] Comment and [69] PEReference can be 
         * entered.
         */
<doctype2>\[ {
            BEGIN(markupdecl);
            return(OPENBRACK);
        }
<markupdecl>\] {
            BEGIN(doctype2);
            return(CLOSEBRACK);
        }
        /*
         * [45] ElementDecl
         * This is only as part of the markupdecl start state. From here, the [5] Name
         * is expected, followed by [46] contentspec.
         */
<markupdecl>"<!ELEMENT" {
            BEGIN(elementdecl);
            return(ELEMENT);
        }
<elementdecl>{Name} {
            BEGIN(contentspec);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
        /*
         * [46] contentspec
         * This is only as part of the elementdecl start state.
         */
<contentspec>"EMPTY" {
            return(EMPTY);
        }
<contentspec>"ANY" {
            return(ANY);
        }
<contentspec>\> {
            BEGIN(markupdecl);
            return(CLOSETAG);
        }
        /*
         * [51] Mixed
         * [47] Children
         * These two are only as part of the contentspec and are enclosed by
         * '(' and ')' or ')*'.
         */
<contentspec>\? {
            return(QUESTION);
        }
<contentspec>\* {
            return(STAR);
        }
<contentspec>\+ {
            return(PLUS);
        }
<contentspec>\( {
            BEGIN(mixedchildren);
            return(OPENPAR);
        }
<mixedchildren>"#PCDATA" {
            return(PCDATA);
        }
<mixedchildren>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<mixedchildren>\| {
            return(PIPE);
        }
<mixedchildren>\, {
            return(COMMA);
        }
<mixedchildren>")*" {
            BEGIN(contentspec);
            return(CLOSEPARSTAR);
        }
<mixedchildren>\) {
            BEGIN(contentspec);
            return(CLOSEPAR);
        }
        /*
         * [52] AttlistDecl
         * This is only as part of the markupdecl start state. From here, the [5] Name
         * and [53] AttDef can be entered.
         */
<markupdecl>"<!ATTLIST" {
            BEGIN(attlistdecl);
            return(ATTLIST);
        }
<attlistdecl>{Name} {
            BEGIN(attdef);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
        /*
         * [53] AttDef
         * This is only as part of the attdef start state and can be entered
         * one time after another. Frome here, the [5] Name, [54] AttType and
         * [60] DefaultDecl can be entered (multiple times). We can also go
         * straight back to markupdecl (when no attribute definitions follow)
         * when we scan an end-tag.
         */
<attdef>{Name} {
            BEGIN(atttype);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<attdef>\> {
            BEGIN(markupdecl);
            return(CLOSETAG);
        }
        /*
         * [54] AttType
         * This is only as part of the atttype start state. From here, the [55] StringType
         * and the [56] TokenizedType can be entered. The [57] EnumeratedType will be broken
         * down into the [58] NotationType and the [59] Enumeration (see below).
         */
<atttype>{StringType} {
            BEGIN(defaultdecl);
            yylval->str = new std::string(yytext);
            return(STRINGTYPE);
        }
<atttype>{TokenizedType} {
            BEGIN(defaultdecl);
            yylval->str = new std::string(yytext);
            return(TOKENIZEDTYPE);
        }
        /*
         * [58] NotationType
         * This is only as part of the atttype start state. A new state
         * 'notationtype' will be entered and stays valid until the last ')'.
         * After that, te attlist3 state is restored.
         */
<atttype>"NOTATION" {
            BEGIN(notationtype);
            return(NOTATION);
        }
<notationtype>\( {
            return(OPENPAR);
        }
<notationtype>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<notationtype>\| {
            return(PIPE);
        }
<notationtype>\) {
            BEGIN(defaultdecl);
            return(CLOSEPAR);
        }
        /*
         * [59] Enumeration
         * This is only as part of the atttype start state. From here, the [7] Nmtoken
         * is the only one to be accepted between a '(' and a ')'.
         */
<atttype>\( {
            BEGIN(enumeration);
            return(OPENPAR);
        }
<enumeration>{Nmtoken} {
            yylval->str = new std::string(yytext);
            return(NMTOKEN);
        }
<enumeration>\| {
            return(PIPE);
        }
<enumeration>\) {
            BEGIN(defaultdecl);
            return(CLOSEPAR);
        }
        /*
         * [60] DefaultDecl
         * This is only as part of the defaultdecl start state. After this, we go back
         * to the atttype state for the next attribute when we see a new [5] Name or we
         * go to the markupdecl if we see an end-tag.
         */
<defaultdecl>"#REQUIRED" {
            return(REQUIRED);
        }
<defaultdecl>"#IMPLIED" {
            return(IMPLIED);
        }
<defaultdecl>"#FIXED" {
            return(FIXED);
        }
<defaultdecl>{Name} {
            BEGIN(atttype);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<defaultdecl>\> {
            BEGIN(markupdecl);
            return(CLOSETAG);
        }
        /*
         * [70] EntityDecl : GEDecl | PEDecl
         * This is only as part of the markupdecl start state. From here, the [71] GEDecl
         * and [72] PEDecl. These two states are merged into one by the lexical scanner
         * in order to keep the scanner simple.
         */
<markupdecl>"<!ENTITY" {
            BEGIN(entitydecl);
            return(ENTITY);
        }
<entitydecl>\% {
            return(PERCENT);
        }
<entitydecl>{Name} {
            BEGIN(entitydecldef);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<entitydecldef>\> {
            BEGIN(markupdecl);
            return(CLOSETAG);
        }
        /*
         * [9] EntityValue
         * This is only accessible from the entitydecldef. It will parse the
         * entity value between single or double quotes and returns back to the
         * entitydecldef state.
         */
<entitydecldef>\" {
            BEGIN(entityvalue_dquote);
            return(DQUOTE);
        }
<entityvalue_dquote>[^"]* {
            yylval->str = new std::string(yytext);
            return(ENTITYVALUE);
        }
<entityvalue_dquote>\" {
            BEGIN(entitydecldef);
            return(DQUOTE);
        }
<entitydecldef>\" {
            BEGIN(entityvalue_squote);
            return(SQUOTE);
        }
<entityvalue_squote>[^']* {
            yylval->str = new std::string(yytext);
            return(ENTITYVALUE);
        }
<entityvalue_squote>\' {
            BEGIN(entitydecldef);
            return(SQUOTE);
        }
        /*
         * [76] NDataDecl
         * This is only accessible from the entitydecldef. From here, the [] Name 
         * is scanned and the state is returned to entitydecldef.
         */
<entitydecldef>"NDATA" {
            BEGIN(ndatadecl);
            return(NDATA);
        }
<ndatadecl>{Name} {
            BEGIN(entitydecldef);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
        /*
         * [82] NotationDecl
         * This is only as part of the markupdecl start state. From here, the
         * [5] Name and [75] ExternalID can be entered. It is closed by a
         * normal close tag and will return to the doctype2 state.
         */
<markupdecl>"<!NOTATION" {
            BEGIN(notationdecl);
            return(NOTATION);
        }
<notationdecl>{Name} {
            BEGIN(notationdecl2);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<notationdecl2>\> {
            BEGIN(markupdecl);
            return(CLOSETAG);
        }
        /*
         * [39] element
         * This state can be entered from the initial or the content state. Note that
         * this rule can be recursive because the [43] content rule may include this
         * element rule.
         */
<INITIAL,content>\< {
            BEGIN(element);
            ++(yyextra.element_stack_level);
            #ifdef FLEX_DEBUG
                if (yy_flex_debug) 
                    fprintf(stderr, "--element stack level: %d\n", yyextra.element_stack_level);
            #endif
            return(OPENTAG);
        }
<element>{Name} {
            BEGIN(attribute);
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<attribute>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<attribute>"/>" {
            --(yyextra.element_stack_level);
            #ifdef FLEX_DEBUG
                if (yy_flex_debug)
                    fprintf(stderr, "--element stack level: %d\n", yyextra.element_stack_level);
            #endif
            if (yyextra.element_stack_level == 0) {
                BEGIN(0);
            } else {
                BEGIN(content);
            }
            return(EMPTYCLOSETAG);
        }
<attribute>\> {
            BEGIN(content);
            return(CLOSETAG);
        }
        /*
         * [42] ETag
         * This state can only be reached from the content state. At this point
         * it is determined if this end-tag ends an inner or the (single) outer
         * element. This is accomplished by inspecting the element_stack_level.
         * If the inner element is ended, the state is changed to content,
         * otherwise, the state is changed to initial.
         */
<etag>{Name} {
            yylval->str = new std::string(yytext);
            return(NAME);
        }
<etag>\> {
            --(yyextra.element_stack_level);
            #ifdef FLEX_DEBUG
                if (yy_flex_debug)
                    fprintf(stderr, "--element stack level: %d\n", yyextra.element_stack_level);
            #endif
            if (yyextra.element_stack_level == 0) {
                BEGIN(0);
            } else {
                BEGIN(content);
            }
            return(CLOSETAG);
        }
        /*
         * [43] content
         * Note that character and entity references are not distinguished
         * explicitely and just made part of the content itself.
         */
<content>"</" {
            BEGIN(etag);
            return(OPENTAGEND);
        }
<content>[^<]* {
            yylval->str = new std::string(yytext);
            return(CHARDATA);
        }
        /*
         * [3] S
         * This can be at numerous places and is therefore shared among all
         * the states.
         */
<*>{S} {
            //return(S);
        }
        /*
         * Single and double quotes are shard at numerous places.
         */
<*>\' {
            return(SQUOTE);
        }
<*>\" {
            return(DQUOTE);
        }

%%

/*
 * Set the scanning method to use an inut stream.
 */
void yyxml_scan_stream(std::istream& istr, yyscan_t scanner)
{
    struct yyxmlextra extra;
    extra.istr = &istr;
    extra.element_stack_level = 0;
    extra.saved_state = 0;
    yyset_extra(extra, scanner);
}

/* vi: set ai ts=4 expandtab: */

