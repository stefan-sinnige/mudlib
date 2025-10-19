%{
/* Verified to work with Bison 3.8
 *
 * In conjunction with xml.l, this implements the following standard:
 *    Extensible Markup Language (XML) 1.0 (Fifth Edition)
 * Available at: https://www.w3.org/TR/xml/
 */

/* Additional C++ statement */
#include <cstring>
#include <iostream>
#include <mud/xml.h>
#include <string>
#include <utility>

%}

%output "src/parser.cpp"
%header "src/parser.hpp"
%name-prefix "yyxml"

%define api.pure
%lex-param { xml_ctx_t* ctx }
%parse-param { xml_ctx_t* ctx }
%locations

/* The union of rule result types. Note that we may have pointer to a shared_ptr
 * object, as we're stuck with POD types. This is 'safe' as we do not actually
 * use the shared_ptr object, until we are actually using it, other than a
 * result of a rule (ie when embedding the shared_ptr into another object).
 * In future, we may be able to use a C++ parser/scanner with a variant like
 *    %define language "c++"
 *    %define api.prefix {yyxml}
 *    %define api.token.constructor
 *    %define parse.assert
 *    %define api.value.type variant
 * which should then be safe to use a st::shared_ptr and other non-POD types
 * directly. But that requires extensive migration changes in both the scanner
 * and the parser. */
%union {
    std::string*             str;
    mud::xml::node::ptr*     node;
    mud::xml::node_seq*      nodes;
    mud::xml::attribute_set* attributes;
}

%code requires {
    /* The reentrant parser uses the following context. */
    typedef void* yyscan_t;
    typedef struct xml_ctx_t {
      yyscan_t scanner;                 /* The lexical scanner */
      xml_ctx_t* parent;                /* Reference to the parent. */
      int errors;                       /* The number of errors detected. */
      mud::xml::document::ptr document; /* The resulting XML document. */
    } xml_ctx_t;
}

%code provides {
    /* Initialise the XML parser context */
    extern void xml_ctx_init(xml_ctx_t* ctx, const std::string& filename,
            xml_ctx_t* parent);
}

%code {
    /* Initialise the XML parser context */
    void xml_ctx_init(xml_ctx_t* ctx, const std::string& filename,
            xml_ctx_t* parent)
    {
        memset(ctx, 0, sizeof(xml_ctx_t));
        ctx->scanner = nullptr;
        ctx->parent = parent;
        ctx->errors = 0;
        ctx->document = nullptr;
    }

    /* The lexical scanner wrapper */
    #undef yylex
    static int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param,
            xml_ctx_t* ctx_param)
    {
        /* External definitions of the actual scanner. */
       extern int yyxmllex(YYSTYPE*, YYLTYPE*, yyscan_t);

       /* Invoke the scanner to get the next lexical token. */
       return yyxmllex(yylval_param, yylloc_param, ctx_param->scanner);
    }

    /* The error handler. */
    int yyxmlerror(YYLTYPE* loc, xml_ctx_t* ctx, const char* msg)
    {
        /* Externale definition to get the token text. */
       extern char* yyxmlget_text(yyscan_t);

       /* Report the error (to standard error for now). */
       std::cerr << "Error: "
                 << loc->first_line << ":" << loc->first_column
                 << ": error: near '" << yyxmlget_text(ctx->scanner)
                 << "': " << msg << std::endl;
       if (++ctx->errors > 25) {
           /* Too many errors */
           throw mud::xml::exception("Too many errors");
       }
       return 1;
    }
}

%token VERSION VERSIONNUM 
%token ENCODING ENCNAME
%token STANDALONE YESNO
%token DOCTYPE SYSTEM PUBLIC SYSTEMLITERAL PUBIDLITERAL
%token ELEMENT EMPTY ANY PCDATA
%token ATTLIST ATTYPE STRINGTYPE TOKENIZEDTYPE NOTATION NMTOKEN ATTVALUE
%token ENTITY ENTITYVALUE NDATA 
%token OPENTAG OPENTAGEND CLOSETAG EMPTYCLOSETAG COMMENTOPEN COMMENT COMMENTCLOSE 
%token XMLPIOPEN PIOPEN PITARGET PIDATA PICLOSE EQ NAME
%token PEREFERENCE REQUIRED IMPLIED FIXED
%token OPENPAR CLOSEPAR CLOSEPARSTAR OPENBRACK CLOSEBRACK SQUOTE DQUOTE PERCENT QUESTION 
%token STAR PLUS COMMA PIPE CHARDATA CDSTART CDATA CDEND

%start document

%%

/* 
 * [1] document : prolog element misc*
 */

document
  : prolog element Misc_seq_opt
    {
      std::unique_ptr<mud::xml::node_seq> children($<nodes>1);
      children->push_back(*$<node>2);
      children->insert(children->end(), $<nodes>3->begin(), $<nodes>3->end());
      ctx->document = mud::xml::dom::create_document();
      ctx->document->children(std::move(*children));
    }
 ;

/*
 * [9] EntityValue : '"' ([^%&"] | PEReference | Reference))* '"' |
 *                   ''' ([^%&'] | PEReference | Reference))* '''
 */

EntityValue
  : DQUOTE ENTITYVALUE DQUOTE
    {
      std::unique_ptr<std::string> entity_value($<str>2);
    }
  | SQUOTE ENTITYVALUE SQUOTE
    {
      std::unique_ptr<std::string> entity_value($<str>2);
    }
 ;

/*
 * [10] AttValue : '"' ([^<&"] | Reference)* '"' | ''' ([^<&'] | Reference)* '''
 */

AttValue
  : DQUOTE ATTVALUE DQUOTE
    {
      $<str>$ = $<str>2;
    }
  | SQUOTE ATTVALUE SQUOTE
    {
      $<str>$ = $<str>2;
    }
  | DQUOTE DQUOTE
    {
      $<str>$ = new std::string;
    }
  | SQUOTE SQUOTE
    {
      $<str>$ = new std::string;
    }
 ;

/*
 * [14] CharData
 */

CharData_opt
  : CHARDATA
    {
      std::unique_ptr<std::string> text($<str>1);
      std::string unescaped = mud::xml::char_reference::unescape(*text);
      auto char_data = mud::xml::dom::create_char_data(unescaped);
      $<node>$ = new mud::xml::node::ptr(char_data);
    }
  | /* empty */
    {
      $<node>$ = nullptr;
    }
;

/*
 * [15] Comment : '<!-' ((Char - '-') | ('-' (Char - '-')))* '-->'
 */

Comment
  : COMMENTOPEN COMMENT COMMENTCLOSE
    {
      std::unique_ptr<std::string> text($<str>2);
      auto comment = mud::xml::dom::create_comment(*text);
      $<node>$ = new mud::xml::node::ptr(comment);
    }
  | COMMENTOPEN COMMENTCLOSE
    {
      $<node>$ = nullptr;
    }
 ;

/*
 * [16] PI : '<&' PITarget (S (Char* - (Char* '?>') Char*)))? '?>'
 * [17] PITarget : Name - (('X'|'x')('M'|'m')('L'|'l'))
 */

PI
  : PIOPEN PITARGET PIDATA PICLOSE
    {
      std::unique_ptr<std::string> target($<str>2);
      std::unique_ptr<std::string> data($<str>3);
      auto pi = mud::xml::dom::create_processing_instruction(*target, *data);
      $<node>$ = new mud::xml::node::ptr(pi);
    }
 ;

/*
 * [18] CDSect : CDStart CData CDEnd
 */

CDSect
  : CDSTART CDATA CDEND
    {
      std::unique_ptr<std::string> text($<str>2);
      auto cdata_section = mud::xml::dom::create_cdata_section(*text);
      $<node>$ = new mud::xml::node::ptr(cdata_section);
    }
 ;

/*
 * [22] Prolog : XMLDecl_opt Misc* (doctypedecl Misc*)?
 */

prolog
  : XMLDecl_opt Misc_seq_opt
    {
      auto nodes = $<nodes>2;
      if ($<node>1 != nullptr)
      {
          nodes->insert(nodes->begin(), *$<node>1);
      }
      $<nodes>$ = nodes;
    }
  | XMLDecl_opt Misc_seq_opt doctypedecl Misc_seq_opt
    {
      auto nodes = $<nodes>2;
      if ($<node>1 != nullptr)
      {
          nodes->insert(nodes->begin(), *$<node>1);
      }
      nodes->insert(nodes->end(), $<nodes>4->begin(), $<nodes>4->end());
      $<nodes>$ = nodes;
    }
 ;

/*
 * [23] XMLDecl : '<xml?' VersionInfo EncodingDecl? SDDecl? '?>'
 */

XMLDecl_opt
  : XMLPIOPEN VersionInfo EncodingDecl_opt SDDecl_opt PICLOSE
    {
      auto decl = mud::xml::dom::create_declaration();
      std::unique_ptr<std::string> versioninfo($<str>2);
      decl->version(std::move(*versioninfo));
      if ($<str>3 != nullptr)
      {
          std::unique_ptr<std::string> encoding($<str>3);
          decl->encoding(std::move(*encoding));
      }
      if ($<str>4 != nullptr)
      {
          std::unique_ptr<std::string> standalone($<str>4);
          decl->standalone(*standalone == "yes");
      }
      $<node>$ = new mud::xml::node::ptr(decl);
    }
  | /* empty */
    {
      $<node>$ = nullptr;
    }
 ;

/*
 * [24] VersionInfo : S 'version' Eq (''' VersionNum ''' | '"' VersionNum '"') S?
 */

VersionInfo
  : VERSION Eq SQUOTE VERSIONNUM SQUOTE
    {
      $<str>$ = $<str>4;
    }
  | VERSION Eq DQUOTE VERSIONNUM DQUOTE
    {
      $<str>$ = $<str>4;
    }
 ;

/*
 * [25] Eq : S? '=' S?
 */

Eq
  : EQ
 ;

/*
 * [27] Misc : Comment | PI | S
 */

Misc_seq_opt
  : Misc_seq_opt Misc
    {
      auto nodes = $<nodes>1;
      if ($<node>2 != nullptr)
      {
          nodes->push_back(*$<node>2);
      }
      $<nodes>$ = nodes;
    }
  | /* empty */
    {
      auto nodes = new mud::xml::node_seq();
      $<nodes>$ = nodes;
    }
 ;

Misc
  : Comment
    {
        $<node>$ = $<node>1;
    }
  | PI
    {
        $<node>$ = $<node>1;
    }
  | /* empty */ 
    {
        $<node>$ = nullptr;
    }
 ;

/*
 * [28] doctypedecl : '<!DOCTYPE' S NAME (S ExternalID)? S? ('[' (MarkupDecl | DeclSep)* ']' S?)? '>'
 */

doctypedecl
  : DOCTYPE NAME ExternalID_opt doctypedeclarations CLOSETAG
    {  
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

ExternalID_opt
  : ExternalID
  | /* empty */
 ;

doctypedeclarations
  : OPENBRACK declarations_seq_opt CLOSEBRACK
  | /* empty */
 ;

declarations_seq_opt
  : declarations_seq_opt declaration
  | /* empty */
 ;

declaration
  : MarkupDecl
  | DeclSep
 ;

/*
 * [28a] DeclSep : PEReference | S
 */

DeclSep
  : PEREFERENCE
    {
      std::unique_ptr<std::string> pe_reference($<str>1);
    }
  | /* empty */
 ;

/*
 * [29] MarkupDecl : elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
 */

MarkupDecl 
  : ElementDecl
  | AttlistDecl
  | EntityDecl
  | Notation
  | PI
  | Comment
 ;

/*
 * [32] SDDecl : 'standalone' Eq (''' ('yes'|'no') ''' | '"' ('yes'|'no') '"') S?
 */

SDDecl_opt
  : STANDALONE Eq SQUOTE YESNO SQUOTE
    {
      $<str>$ = $<str>4;
    }
  | STANDALONE Eq DQUOTE YESNO DQUOTE
    {
      $<str>$ = $<str>4;
    }
  | /* empty */
    {
      $<str>$ = nullptr;
    }
 ;

/*
 * [39] element : EmptyElemTag | (STag content ETag)
 */

element
  : EmptyElemTag
    {
      $<node>$ = $<node>1;
    }
  | STag content ETag
    {
      auto element = ((*$<node>1)->get_shared<mud::xml::element>());
      std::unique_ptr<std::string> etag($<str>3);
      std::unique_ptr<mud::xml::node_seq> nodes($<nodes>2);
      element->children(std::move(*nodes));
      $<node>$ = $<node>1;
    }
 ;

/*
 * [40] STag : '<' Name S? ((Attribute S)* Attribute S?)? '>'
 */

STag 
  : OPENTAG NAME Attribute_seq_opt CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
      std::unique_ptr<mud::xml::attribute_set> attrs($<attributes>3);
      auto element = mud::xml::dom::create_element(*name);
      element->attributes(std::move(*attrs));
      $<node>$ = new mud::xml::node::ptr(element);
    }
 ;

/*
 * [41] Attribute : Name Eq AttValue
 */

Attribute_seq_opt
  : Attribute_seq
    {
      $<attributes>$ = $<attributes>1;
    }
  | /* empty */
    {
      auto attrs = new mud::xml::attribute_set();
      $<attributes>$ = attrs;
    }
 ;

Attribute_seq
  : Attribute_seq Attribute
    {
      auto attrs = $<attributes>1;
      attrs->insert((*$<node>2)->get_shared<mud::xml::attribute>());
      $<attributes>$ = attrs;
    }
  | Attribute
    {
      auto attrs = new mud::xml::attribute_set();
      attrs->insert((*$<node>1)->get_shared<mud::xml::attribute>());
      $<attributes>$ = attrs;
    }
 ;

Attribute
  : NAME Eq AttValue
    {
      std::unique_ptr<std::string> name($<str>1);
      std::unique_ptr<std::string> value($<str>3);
      auto attr = mud::xml::dom::create_attribute(*name);
      std::string unescaped = mud::xml::char_reference::unescape(*value);
      attr->value(unescaped);
      $<node>$ = new mud::xml::node::ptr(attr);
    }
 ;

/*
 * [42] ETag : '<\' Name S? '>'
 */

ETag 
  : OPENTAGEND NAME CLOSETAG
    {
      $<str>$ = $<str>2;
    }
 ;

/*
 * [43] content : CharData? ((Element | Reference | CDSect | PI | Comment) CharData?)*
 */

content
  : CharData_opt contents_seq_opt
    {
      auto nodes = $<nodes>2;
      if ($<node>1 != nullptr) {
          nodes->insert(nodes->begin(), *$<node>1);
      }
      $<nodes>$ = nodes;
    }
 ;

contents_seq_opt
  : contents_seq
    {
      $<nodes>$ = $<nodes>1;
    }
  | /* empty */
    {
      auto nodes = new mud::xml::node_seq();
      $<nodes>$ = nodes;
    }
 ;

contents_seq
  : contents_seq contents_choice CharData_opt
    {
      auto nodes = $<nodes>1;
      nodes->push_back(*$<node>2);
      if ($<node>3 != nullptr) {
          nodes->push_back(*$<node>3);
      }
      $<nodes>$ = nodes;
    }
  | contents_choice CharData_opt
    {
      auto nodes = new mud::xml::node_seq();
      nodes->push_back(*$<node>1);
      if ($<node>2 != nullptr) {
          nodes->push_back(*$<node>2);
      }
      $<nodes>$ = nodes;
    }
 ;

contents_choice
  : element
    {
      $<node>$ = $<node>1;
    }
  | CDSect
    {
      $<node>$ = $<node>1;
    }
  | PI
    {
      $<node>$ = $<node>1;
    }
  | Comment
    {
      $<node>$ = $<node>1;
    }
 ;

/*
 * [44] EmptyElemTag : '<' Name S? (Attribute S)* Attribute S?)? '/>'
 */

EmptyElemTag 
  : OPENTAG NAME Attribute_seq_opt EMPTYCLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
      auto element = mud::xml::dom::create_element(*name);
      auto attrs = $<attributes>3;;
      element->attributes(std::move(*attrs));
      $<node>$ = new mud::xml::node::ptr(element);
    }
 ;

/*
 * [45] ElementDecl : '<!ELEMENT' S Name S contentspec S? '>'
 */

ElementDecl
  : ELEMENT NAME contentspec CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

/*
 * [46] : contentspec : 'EMPTY' | 'ANY' | Mixed | children
 */

contentspec
  : EMPTY
  | ANY
  | Mixed
  | children
 ;

/*
 * [47] children : (choice | seq) ('?' | '*' | '+')?
 */

children 
  : choice_sequence multiplicity_opt
 ;

choice_sequence
  : choice
  | sequence
 ;

multiplicity_opt
  : QUESTION
  | STAR
  | PLUS
  | /* empty */
 ;

/*
 * [48] cp : (Name | choice | seq) ('?' | '*' | '+')?
 */

cp
  : name_choice_sequence multiplicity_opt
 ;

name_choice_sequence
  : NAME
    {
      std::unique_ptr<std::string> name($<str>1);
    }
  | choice
  | sequence
 ;

/*
 * [49] choice : '(' S? cp (S? '|' cp)+ S? ')'
 */

choice
  : OPENPAR cp_choice CLOSEPAR
 ;

cp_choice
  : cp_choice PIPE cp
  | cp PIPE cp
 ;

/*
 * [50] seq : '(' S? cp (S" ',' S cp)* S? ')'
 */

sequence 
  : OPENPAR cp CLOSEPAR
  | OPENPAR cp COMMA cp_seq CLOSEPAR
 ;

cp_seq
  : cp_seq COMMA cp
  | cp
 ;

/*
 * [51] Mixed : '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')
 */

Mixed 
  : OPENPAR PCDATA names_seq CLOSEPARSTAR
  | OPENPAR PCDATA CLOSEPAR
 ;

names_seq
  : names_seq PIPE NAME
    {
      std::unique_ptr<std::string> name($<str>3);
    }
  | PIPE NAME
    {
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

/*
 * [52] AttlistDecl : '<!ATTLIST' S Name AttDef* S? '>'
 */

AttlistDecl 
  : ATTLIST NAME AttDef_seq_opt CLOSETAG
 ;

/*
 * [53] AttDef : Name AttType DefaultDecl
 */

AttDef_seq_opt
  : AttDef_seq_opt AttDef
  | /* empty */
 ;

AttDef 
  : NAME AttType DefaultDecl
    {
      std::unique_ptr<std::string> name($<str>1);
    }
 ;

/*
 * [54] AttType : StringType | TokenizedType | EnumeratedType
 */

AttType
  : STRINGTYPE
  | TOKENIZEDTYPE
  | EnumeratedType
 ;

/*
 * [57] EnumeratedType : NotationType | Enumeration
 */

EnumeratedType
  : NotationType
  | Enumeration
 ;

/*
 * [58] NotationType : 'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
 */

NotationType 
  : NOTATION OPENPAR NotationTypeNames_seq CLOSEPAR
 ;

NotationTypeNames_seq
  : NotationTypeNames_seq PIPE NAME
    {
      std::unique_ptr<std::string> name($<str>3);
    }
  | NAME
    {
      std::unique_ptr<std::string> name($<str>1);
    }
 ;

/*
 * [59] Enumeration : '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
 */

Enumeration
  : OPENPAR EnumerationTokens_seq CLOSEPAR
 ;

EnumerationTokens_seq
  : EnumerationTokens_seq PIPE NMTOKEN
    {
      std::unique_ptr<std::string> token($<str>3);
    }
  | NMTOKEN
    {
      std::unique_ptr<std::string> token($<str>1);
    }
 ;

/*
 * [60] DefaultDecl : '#REQUIRED' | '#IMPLIED' | (('#FIXED' S)? AttValue)
 */

DefaultDecl
  : REQUIRED
  | IMPLIED
  | FIXED_opt AttValue
 ;

FIXED_opt 
  : FIXED
  | /* empty */
 ;

/*
 * [67] Reference : EntityRef | CharRef
 *
 * These are not distinguished by the lexical analyser and are just made part
 * of the CharData instead.
 */

/*
 * [70] EntityDecl : GEDecl | PEDecl
 */

EntityDecl
  : GEDecl
  | PEDecl
 ;

/*
 * [71] GEDecl : '<!ENTITY' S Name S EntityDef S? '>'
 */

GEDecl
  : ENTITY NAME EntityDef CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

/*
 * [72] PEDecl : '<!ENTITY' S '%' S  Name S PEDef S? CLOSETAG
 */

PEDecl
  : ENTITY PERCENT NAME PEDef CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>3);
    }
 ;

/* 
 * [73] EntityDef : EntityValue | (ExternalID NDataDecl?)
 */

EntityDef
  : EntityValue 
  | ExternalID NDataDecl_opt
 ;

/*
 * [74] PEDef : : EntityValue | ExternalID
 */

PEDef 
  : EntityValue
  | ExternalID
 ;

/*
 * [75] ExternalID : ('SYSTEM' S SystemLiteral) | ('PUBLIC' S PubidLiteral S SystemLiteral)
 */

ExternalID
  : SYSTEM SYSTEMLITERAL
    {
      std::unique_ptr<std::string> literal($<str>2);
    }
  | PUBLIC PUBIDLITERAL SYSTEMLITERAL
    {
      std::unique_ptr<std::string> pubid_literal($<str>2);
      std::unique_ptr<std::string> system_literal($<str>3);
    }
 ;

/* 
 * [76] NDataDecl : S 'NDATA' S Name
 */

NDataDecl_opt
  : NDataDecl
  | /* empty */
 ;

NDataDecl
  : NDATA NAME
    {
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

/*
 * [80] EncodingDecl : 'encoding' Eq (''' EncName ''' | '"' EncName '"') S?
 * [81] EncName : [A-Za-z] ([A-Za-z0-9._] | '-')*
 */

EncodingDecl_opt 
  : ENCODING Eq SQUOTE ENCNAME SQUOTE
    {
      $<str>$ = $<str>4;
    }
  | ENCODING Eq DQUOTE ENCNAME DQUOTE
    {
      $<str>$ = $<str>4;
    }
  | /* empty */
    {
      $<str>$ = nullptr;
    }
 ;

/*
 * [82] Notation : '<!NOTATION' S NAME S (ExternalID | PublicID) S? '>'
 * [83] PublicID : 'PUBLIC' S PubidLiteral
 */

Notation
  : NOTATION NAME ExternalID CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
    }
 ;

%% 

