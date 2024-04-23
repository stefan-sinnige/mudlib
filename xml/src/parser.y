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
#include <vector>

%}

%output "src/parser.cpp"
%header "src/parser.hpp"
%name-prefix "yyxml"

%define api.pure
%lex-param { xml_ctx_t* ctx }
%parse-param { xml_ctx_t* ctx }
%locations

%union {
    std::string*                            str;
    mud::xml::document*                     document;
    mud::xml::element*                      element;
    mud::xml::attribute*                    attribute;
    std::vector<mud::xml::attribute>*       attributes;
    mud::xml::node*                         node;
    mud::core::poly_vector<mud::xml::node>* nodes;
}

%code requires {
    /* The reentrant parser uses the following context. */
    typedef void* yyscan_t;
    typedef struct xml_ctx_t {
      yyscan_t scanner;             /* The lexical scanner */
      xml_ctx_t* parent;            /* Reference to the parent. */
      int errors;                   /* The number of errors detected. */
      mud::xml::document* document; /* The resulting XML document. */
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

    /* THe error handler. */
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

    /* Push a 'node' to the end of a vector using correct type for
     * move-semantice */
    template<class NodeType>
    void push_back(NodeType* node,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        nodes->push_back(std::move(*node));
    }

    void push_back(mud::xml::node* node,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        switch (node->type())
        {
            case mud::xml::node::type_t::CDATA_SECTION:
                push_back<mud::xml::cdata_section>(
                    static_cast<mud::xml::cdata_section*>(node), nodes);
                break;
            case mud::xml::node::type_t::CHAR_DATA:
                push_back<mud::xml::char_data>(
                    static_cast<mud::xml::char_data*>(node), nodes);
                break;
            case mud::xml::node::type_t::COMMENT:
                push_back<mud::xml::comment>(
                    static_cast<mud::xml::comment*>(node), nodes);
                break;
            case mud::xml::node::type_t::DECL:
                push_back<mud::xml::declaration>(
                    static_cast<mud::xml::declaration*>(node),
                    nodes);
                break;
            case mud::xml::node::type_t::ELEMENT:
                push_back<mud::xml::element>(
                    static_cast<mud::xml::element*>(node), nodes);
                break;
            case mud::xml::node::type_t::PI:
                push_back<mud::xml::processing_instruction>(
                    static_cast<mud::xml::processing_instruction*>(node),
                    nodes);
                break;
            default:
                break;
        }
    }

    /* Push a 'node' to the beginning of a vector using correct type for
     * move-semantice */
    template<class NodeType>
    void push_front(NodeType* node,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        nodes->insert(nodes->begin(), std::move(*node));
    }

    void push_front(mud::xml::node* node,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        switch (node->type())
        {
            case mud::xml::node::type_t::CDATA_SECTION:
                push_front<mud::xml::cdata_section>(
                    static_cast<mud::xml::cdata_section*>(node), nodes);
                break;
            case mud::xml::node::type_t::CHAR_DATA:
                push_front<mud::xml::char_data>(
                    static_cast<mud::xml::char_data*>(node), nodes);
                break;
            case mud::xml::node::type_t::COMMENT:
                push_front<mud::xml::comment>(
                    static_cast<mud::xml::comment*>(node), nodes);
                break;
            case mud::xml::node::type_t::DECL:
                push_front<mud::xml::declaration>(
                    static_cast<mud::xml::declaration*>(node), nodes);
                break;
            case mud::xml::node::type_t::ELEMENT:
                push_front<mud::xml::element>(
                    static_cast<mud::xml::element*>(node), nodes);
                break;
            case mud::xml::node::type_t::PI:
                push_front<mud::xml::processing_instruction>(
                    static_cast<mud::xml::processing_instruction*>(node),
                    nodes);
            default:
                break;
        }
    }

    /* Push all nodes from the other vector to the end using move-semantics */
    template<class NodeType>
    void push_back(NodeType& node,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        nodes->insert(nodes->end(), std::move(node));
    }

    void push_back(mud::core::poly_vector<mud::xml::node>* other,
                   mud::core::poly_vector<mud::xml::node>* nodes)
    {
        for (auto& node: *other)
        {
            switch (node.type())
            {
                case mud::xml::node::type_t::CDATA_SECTION:
                    push_back<mud::xml::cdata_section>(
                        static_cast<mud::xml::cdata_section&>(node), nodes);
                    break;
                case mud::xml::node::type_t::CHAR_DATA:
                    push_back<mud::xml::char_data>(
                        static_cast<mud::xml::char_data&>(node), nodes);
                    break;
                case mud::xml::node::type_t::COMMENT:
                    push_back<mud::xml::comment>(
                        static_cast<mud::xml::comment&>(node), nodes);
                    break;
                case mud::xml::node::type_t::DECL:
                    push_back<mud::xml::declaration>(
                        static_cast<mud::xml::declaration&>(node), nodes);
                    break;
                case mud::xml::node::type_t::ELEMENT:
                    push_back<mud::xml::element>(
                        static_cast<mud::xml::element&>(node), nodes);
                    break;
                case mud::xml::node::type_t::PI:
                    push_back<mud::xml::processing_instruction>(
                        static_cast<mud::xml::processing_instruction&>(node),
                        nodes);
                    break;
                default:
                    break;
            }
        }
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
%token S XMLPIOPEN PIOPEN PITARGET PIDATA PICLOSE EQ NAME
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
      std::unique_ptr<mud::core::poly_vector<mud::xml::node>> prolog($<nodes>1);
      std::unique_ptr<mud::core::poly_vector<mud::xml::node>> misc($<nodes>3);
      std::unique_ptr<mud::xml::element> element($<element>2);
      push_back(element.get(), prolog.get());
      push_back(misc.get(), prolog.get());
      ctx->document = new mud::xml::document;
      ctx->document->nodes(std::move(*prolog));
    }
 ;

/*
 * [3] S : (#x20 | #x9 | #xD | #xA)+
 */

S_opt
  : S
  | /* empty */
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
      auto char_data = new mud::xml::char_data;
      std::unique_ptr<std::string> text($<str>1);
      char_data->text(std::move(*text));
      $<node>$ = char_data;
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
      auto comment = new mud::xml::comment;
      std::unique_ptr<std::string> text($<str>2);
      comment->text(std::move(*text));
      $<node>$ = comment;
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
      auto pi = new mud::xml::processing_instruction;
      std::unique_ptr<std::string> target($<str>2);
      std::unique_ptr<std::string> data($<str>3);
      pi->target(std::move(*target));
      pi->data(std::move(*data));
      $<node>$ = pi;
    }
 ;

/*
 * [18] CDSect : CDStart CData CDEnd
 */

CDSect
  : CDSTART CDATA CDEND
    {
      auto cdata_section = new mud::xml::cdata_section;
      std::unique_ptr<std::string> text($<str>2);
      cdata_section->text(std::move(*text));
      $<node>$ = cdata_section;
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
          std::unique_ptr<mud::xml::node> decl($<node>1);
          push_front(decl.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
  | XMLDecl_opt Misc_seq_opt doctypedecl Misc_seq_opt
    {
      auto nodes = $<nodes>2;
      if ($<node>1 != nullptr)
      {
          std::unique_ptr<mud::xml::node> decl($<node>1);
          push_front(decl.get(), nodes);
      }
      std::unique_ptr<mud::core::poly_vector<mud::xml::node>> other($<nodes>4);
      push_back(other.get(), nodes);
      $<nodes>$ = nodes;
    }
 ;

/*
 * [23] XMLDecl : '<xml?' VersionInfo EncodingDecl? SDDecl? '?>'
 */

XMLDecl_opt
  : XMLPIOPEN VersionInfo EncodingDecl_opt SDDecl_opt PICLOSE
    {
      auto decl = new mud::xml::declaration;
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
      $<node>$ = decl;
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
  : S VERSION Eq SQUOTE VERSIONNUM SQUOTE S_opt
    {
      $<str>$ = $<str>5;
    }
  | S VERSION Eq DQUOTE VERSIONNUM DQUOTE S_opt
    {
      $<str>$ = $<str>5;
    }
 ;

/*
 * [25] Eq : S? '=' S?
 */

Eq
  : S_opt EQ S_opt
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
          std::unique_ptr<mud::xml::node> misc($<node>2);
          push_back(misc.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
  | /* empty */
    {
      auto nodes = new mud::core::poly_vector<mud::xml::node>();
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
  | S
    {
        $<node>$ = nullptr;
    }
 ;

/*
 * [28] doctypedecl : '<!DOCTYPE' S NAME (S ExternalID)? S? ('[' (MarkupDecl | DeclSep)* ']' S?)? '>'
 */

doctypedecl
  : DOCTYPE S NAME S_ExternalID_opt S_opt doctypedeclarations CLOSETAG
    {  
      std::unique_ptr<std::string> name($<str>3);
    }
 ;

S_ExternalID_opt
  : S ExternalID
  | /* empty */
 ;

doctypedeclarations
  : OPENBRACK declarations_seq_opt CLOSEBRACK S_opt
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
  | S
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
  : STANDALONE Eq SQUOTE YESNO SQUOTE S_opt
    {
      $<str>$ = $<str>4;
    }
  | STANDALONE Eq DQUOTE YESNO DQUOTE S_opt
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
      $<element>$ = $<element>1;
    }
  | STag content ETag
    {
      auto element = $<element>1;
      std::unique_ptr<std::string> etag($<str>3);
      std::unique_ptr<mud::core::poly_vector<mud::xml::node>> nodes($<nodes>2);
      element->nodes(std::move(*nodes));
      $<element>$ = element;
    }
 ;

/*
 * [40] STag : '<' Name S? ((Attribute S)* Attribute S?)? '>'
 */

STag 
  : OPENTAG NAME S_opt Attribute_seq_opt CLOSETAG
    {
      auto element = new mud::xml::element;
      std::unique_ptr<std::string> name($<str>2);
      std::unique_ptr<std::vector<mud::xml::attribute>> attrs($<attributes>4);
      element->name(std::move(*name));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
 ;

/*
 * [41] Attribute : Name Eq AttValue
 */

Attribute_seq_opt
  : Attribute_seq S_opt
    {
      $<attributes>$ = $<attributes>1;
    }
  | /* empty */
    {
      auto attrs = new std::vector<mud::xml::attribute>();
      $<attributes>$ = attrs;
    }
 ;

Attribute_seq
  : Attribute_seq S Attribute
    {
      auto attrs = $<attributes>1;
      std::unique_ptr<mud::xml::attribute> attr($<attribute>3);
      attrs->push_back(std::move(*attr));
      $<attributes>$ = attrs;
    }
  | Attribute
    {
      auto attrs = new std::vector<mud::xml::attribute>();
      std::unique_ptr<mud::xml::attribute> attr($<attribute>1);
      attrs->push_back(std::move(*attr));
      $<attributes>$ = attrs;
    }
 ;

Attribute
  : NAME Eq AttValue
    {
      std::unique_ptr<std::string> name($<str>1);
      std::unique_ptr<std::string> value($<str>3);
      auto attr = new mud::xml::attribute();
      attr->name(std::move(*name));
      attr->value(std::move(*value));
      $<attribute>$ = attr;
    }
 ;

/*
 * [42] ETag : '<\' Name S? '>'
 */

ETag 
  : OPENTAGEND NAME S_opt CLOSETAG
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
          std::unique_ptr<mud::xml::node> char_data($<node>1);
          push_front(char_data.get(), nodes);
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
      auto nodes = new mud::core::poly_vector<mud::xml::node>();
      $<nodes>$ = nodes;
    }
 ;

contents_seq
  : contents_seq contents_choice CharData_opt
    {
      auto nodes = $<nodes>1;
      std::unique_ptr<mud::xml::node> contents($<node>2);
      push_back(contents.get(), nodes);
      if ($<node>3 != nullptr) {
          std::unique_ptr<mud::xml::node> char_data($<node>3);
          push_back(char_data.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
  | contents_choice CharData_opt
    {
      auto nodes = new mud::core::poly_vector<mud::xml::node>();
      std::unique_ptr<mud::xml::node> contents($<node>1);
      push_back(contents.get(), nodes);
      if ($<node>2 != nullptr) {
          std::unique_ptr<mud::xml::node> char_data($<node>2);
          push_back(char_data.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
 ;

contents_choice
  : element
    {
      $<node>$ = $<element>1;
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
  : OPENTAG NAME S_opt Attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::xml::element;
      std::unique_ptr<std::string> name($<str>2);
      std::unique_ptr<std::vector<mud::xml::attribute>> attrs($<attributes>4);
      element->name(std::move(*name));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
 ;

/*
 * [45] ElementDecl : '<!ELEMENT' S Name S contentspec S? '>'
 */

ElementDecl
  : ELEMENT S NAME S contentspec S_opt CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>3);
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
  : OPENPAR S_opt cp_choice S_opt CLOSEPAR
 ;

cp_choice
  : cp_choice S_opt PIPE S_opt cp
  | cp S_opt PIPE S_opt cp
 ;

/*
 * [50] seq : '(' S? cp (S" ',' S cp)* S? ')'
 */

sequence 
  : OPENPAR S_opt cp S_opt CLOSEPAR
  | OPENPAR S_opt cp S_opt COMMA S_opt cp_seq CLOSEPAR
 ;

cp_seq
  : cp_seq S_opt COMMA S_opt cp
  | cp
 ;

/*
 * [51] Mixed : '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')
 */

Mixed 
  : OPENPAR S_opt PCDATA names_seq S_opt CLOSEPARSTAR
  | OPENPAR S_opt PCDATA S_opt CLOSEPAR
 ;

names_seq
  : names_seq S_opt PIPE S_opt NAME
    {
      std::unique_ptr<std::string> name($<str>5);
    }
  | S_opt PIPE S_opt NAME
    {
      std::unique_ptr<std::string> name($<str>4);
    }
 ;

/*
 * [52] AttlistDecl : '<!ATTLIST' S Name AttDef* S? '>'
 */

AttlistDecl 
  : ATTLIST S NAME AttDef_seq_opt S_opt CLOSETAG
 ;

/*
 * [53] AttDef : S Name S AttType S DefaultDecl
 */

AttDef_seq_opt
  : AttDef_seq_opt AttDef
  | /* empty */
 ;

AttDef 
  : S NAME S AttType S DefaultDecl
    {
      std::unique_ptr<std::string> name($<str>2);
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
  : NOTATION S OPENPAR S_opt NotationTypeNames_seq S_opt CLOSEPAR
 ;

NotationTypeNames_seq
  : NotationTypeNames_seq S_opt PIPE S_opt NAME
    {
      std::unique_ptr<std::string> name($<str>5);
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
  : OPENPAR S_opt EnumerationTokens_seq S_opt CLOSEPAR
 ;

EnumerationTokens_seq
  : EnumerationTokens_seq S_opt PIPE S_opt NMTOKEN
    {
      std::unique_ptr<std::string> token($<str>5);
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
  : FIXED S
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
  : ENTITY S NAME S EntityDef CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>3);
    }
 ;

/*
 * [72] PEDecl : '<!ENTITY' S '%' S  Name S PEDef S? CLOSETAG
 */

PEDecl
  : ENTITY S PERCENT S NAME S PEDef S_opt CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>5);
    }
 ;

/* 
 * [73] EntityDef : EntityValue | (ExternalID NDataDecl?)
 */

EntityDef
  : EntityValue S_opt
  | ExternalID S_opt NDataDecl_opt
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
  : SYSTEM S SYSTEMLITERAL
    {
      std::unique_ptr<std::string> literal($<str>3);
    }
  | PUBLIC S PUBIDLITERAL S SYSTEMLITERAL
    {
      std::unique_ptr<std::string> pubid_literal($<str>3);
      std::unique_ptr<std::string> system_literal($<str>5);
    }
 ;

/* 
 * [76] NDataDecl : S 'NDATA' S Name
 */

NDataDecl_opt
  : NDataDecl S_opt
  | /* empty */
 ;

NDataDecl
  : NDATA S NAME
    {
      std::unique_ptr<std::string> name($<str>3);
    }
 ;

/*
 * [80] EncodingDecl : 'encoding' Eq (''' EncName ''' | '"' EncName '"') S?
 * [81] EncName : [A-Za-z] ([A-Za-z0-9._] | '-')*
 */

EncodingDecl_opt 
  : ENCODING Eq SQUOTE ENCNAME SQUOTE S_opt
    {
      $<str>$ = $<str>4;
    }
  | ENCODING Eq DQUOTE ENCNAME DQUOTE S_opt
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
  : NOTATION S NAME S ExternalID S_opt CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>3);
    }
 ;

%% 

