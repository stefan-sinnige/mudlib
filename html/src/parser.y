%{
/* Verified to work with Bison 3.8
 *
 * In conjunction with xml.l, this implements the HTML standard:
 *    https://html.spec.whatwg.org/
 * but not through the proposed implementation. The grammar provided here
 */

/* Additional C++ statement */
#include <cstring>
#include <iostream>
#include <mud/html.h>
#include <string>
#include <utility>
#include <vector>

%}

%output "src/parser.cpp"
%header "src/parser.hpp"
%name-prefix "yyhtml"

%define api.pure
%lex-param { html_ctx_t* ctx }
%parse-param { html_ctx_t* ctx }
%locations

%union {
    std::string*                            str;
    mud::html::document*                     document;
    mud::html::element*                      element;
    mud::html::attribute*                    attribute;
    std::vector<mud::html::attribute>*       attributes;
    mud::html::node*                         node;
    mud::core::poly_vector<mud::html::node>* nodes;
}

%code requires {
    /* Add the HTML types. */
    #include <mud/html.h>

    /* The reentrant parser uses the following context. */
    typedef void* yyscan_t;
    typedef struct html_ctx_t {
      yyscan_t scanner;             /* The lexical scanner */
      html_ctx_t* parent;            /* Reference to the parent. */
      int errors;                   /* The number of errors detected. */
      mud::html::document* document; /* The resulting XML document. */
    } html_ctx_t;
}

%code provides {
    /* Initialise the XML parser context */
    extern void html_ctx_init(html_ctx_t* ctx, const std::string& filename,
            html_ctx_t* parent);
}

%code {
    /* Initialise the XML parser context */
    void html_ctx_init(html_ctx_t* ctx, const std::string& filename,
            html_ctx_t* parent)
    {
        memset(ctx, 0, sizeof(html_ctx_t));
        ctx->scanner = nullptr;
        ctx->parent = parent;
        ctx->errors = 0;
        ctx->document = nullptr;
    }

    /* The lexical scanner wrapper */
    #undef yylex
    static int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param,
            html_ctx_t* ctx_param)
    {
        /* External definitions of the actual scanner. */
       extern int yyhtmllex(YYSTYPE*, YYLTYPE*, yyscan_t);

       /* Invoke the scanner to get the next lexical token. */
       return yyhtmllex(yylval_param, yylloc_param, ctx_param->scanner);
    }

    /* THe error handler. */
    int yyhtmlerror(YYLTYPE* loc, html_ctx_t* ctx, const char* msg)
    {
        /* Externale definition to get the token text. */
       extern char* yyhtmlget_text(yyscan_t);

       /* Report the error (to standard error for now). */
       std::cerr << "Error: "
                 << loc->first_line << ":" << loc->first_column
                 << ": error: near '" << yyhtmlget_text(ctx->scanner)
                 << "': " << msg << std::endl;
       if (++ctx->errors > 25) {
           /* Too many errors */
           throw mud::html::exception("Too many errors");
       }
       return 1;
    }

    /* Push a 'node' to the end of a vector using correct type for
     * move-semantice */
    template<class NodeType>
    void push_back(NodeType* node,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        nodes->push_back(std::move(*node));
    }

    void push_back(mud::html::node* node,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        switch (node->type())
        {
            case mud::html::node::type_t::CDATA_SECTION:
                push_back<mud::html::cdata_section>(
                    static_cast<mud::html::cdata_section*>(node), nodes);
                break;
            case mud::html::node::type_t::CHAR_DATA:
                push_back<mud::html::char_data>(
                    static_cast<mud::html::char_data*>(node), nodes);
                break;
            case mud::html::node::type_t::COMMENT:
                push_back<mud::html::comment>(
                    static_cast<mud::html::comment*>(node), nodes);
                break;
            case mud::html::node::type_t::DECL:
                push_back<mud::html::declaration>(
                    static_cast<mud::html::declaration*>(node),
                    nodes);
                break;
            case mud::html::node::type_t::ELEMENT:
                push_back<mud::html::element>(
                    static_cast<mud::html::element*>(node), nodes);
                break;
            case mud::html::node::type_t::PI:
                push_back<mud::html::processing_instruction>(
                    static_cast<mud::html::processing_instruction*>(node),
                    nodes);
                break;
            case mud::html::node::type_t::RAW_DATA:
                push_back<mud::html::raw_data>(
                    static_cast<mud::html::raw_data*>(node), nodes);
                break;
            default:
                break;
        }
    }

    /* Push a 'node' to the beginning of a vector using correct type for
     * move-semantice */
    template<class NodeType>
    void push_front(NodeType* node,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        nodes->insert(nodes->begin(), std::move(*node));
    }

    void push_front(mud::html::node* node,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        switch (node->type())
        {
            case mud::html::node::type_t::CDATA_SECTION:
                push_front<mud::html::cdata_section>(
                    static_cast<mud::html::cdata_section*>(node), nodes);
                break;
            case mud::html::node::type_t::CHAR_DATA:
                push_front<mud::html::char_data>(
                    static_cast<mud::html::char_data*>(node), nodes);
                break;
            case mud::html::node::type_t::COMMENT:
                push_front<mud::html::comment>(
                    static_cast<mud::html::comment*>(node), nodes);
                break;
            case mud::html::node::type_t::DECL:
                push_front<mud::html::declaration>(
                    static_cast<mud::html::declaration*>(node), nodes);
                break;
            case mud::html::node::type_t::ELEMENT:
                push_front<mud::html::element>(
                    static_cast<mud::html::element*>(node), nodes);
                break;
            case mud::html::node::type_t::PI:
                push_front<mud::html::processing_instruction>(
                    static_cast<mud::html::processing_instruction*>(node),
                    nodes);
                break;
            case mud::html::node::type_t::RAW_DATA:
                push_front<mud::html::raw_data>(
                    static_cast<mud::html::raw_data*>(node), nodes);
                break;
            default:
                break;
        }
    }

    /* Push all nodes from the other vector to the end using move-semantics */
    template<class NodeType>
    void push_back(NodeType& node,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        nodes->insert(nodes->end(), std::move(node));
    }

    void push_back(mud::core::poly_vector<mud::html::node>* other,
                   mud::core::poly_vector<mud::html::node>* nodes)
    {
        for (auto& node: *other)
        {
            switch (node.type())
            {
                case mud::html::node::type_t::CDATA_SECTION:
                    push_back<mud::html::cdata_section>(
                        static_cast<mud::html::cdata_section&>(node), nodes);
                    break;
                case mud::html::node::type_t::CHAR_DATA:
                    push_back<mud::html::char_data>(
                        static_cast<mud::html::char_data&>(node), nodes);
                    break;
                case mud::html::node::type_t::COMMENT:
                    push_back<mud::html::comment>(
                        static_cast<mud::html::comment&>(node), nodes);
                    break;
                case mud::html::node::type_t::DECL:
                    push_back<mud::html::declaration>(
                        static_cast<mud::html::declaration&>(node), nodes);
                    break;
                case mud::html::node::type_t::ELEMENT:
                    push_back<mud::html::element>(
                        static_cast<mud::html::element&>(node), nodes);
                    break;
                case mud::html::node::type_t::PI:
                    push_back<mud::html::processing_instruction>(
                        static_cast<mud::html::processing_instruction&>(node),
                        nodes);
                    break;
                case mud::html::node::type_t::RAW_DATA:
                    push_back<mud::html::raw_data>(
                        static_cast<mud::html::raw_data&>(node), nodes);
                    break;
                default:
                    break;
            }
        }
    }
}

%token COMMENTOPEN COMMENTCLOSE COMMENT DOCTYPE
%token OPENTAG OPENTAGEND CLOSETAG EMPTYCLOSETAG ATTVALUE
%token RAW CHARDATA CDSTART CDATA CDEND NAME DQUOTE SQUOTE EQ
%token HTML
%token AREA BASE BR COL EMBED HR IMG INPUT LINK META PARAM SOURCE TRACK WBR
%token SCRIPT STYLE TEXTAREA TITLE

%start document

%%

document
  : prolog html epilog
    {
      std::unique_ptr<mud::core::poly_vector<mud::html::node>> nodes($<nodes>1);
      std::unique_ptr<mud::core::poly_vector<mud::html::node>> misc($<nodes>3);
      std::unique_ptr<mud::html::element> element($<element>2);
      push_back(element.get(), nodes.get());
      push_back(misc.get(), nodes.get());
      ctx->document = new mud::html::document;
      ctx->document->nodes(std::move(*nodes));
    }
;

prolog
  : misc_seq_opt doctype_opt misc_seq_opt
    {
      auto nodes = $<nodes>1;
      if ($<node>2 != nullptr)
      {
          std::unique_ptr<mud::html::node> doctype($<node>2);
          push_back(doctype.get(), nodes);
      }
      std::unique_ptr<mud::core::poly_vector<mud::html::node>> epilog($<nodes>3);
      push_back(epilog.get(), nodes);
      $<nodes>$ = nodes;
    }
;

epilog
  : misc_seq_opt
    {
      auto nodes = $<nodes>1;
      $<nodes>$ = nodes;
    }
;

misc_seq_opt
  : misc_seq_opt misc
    {
      auto nodes = $<nodes>1;
      if ($<node>2 != nullptr)
      {
          std::unique_ptr<mud::html::node> misc($<node>2);
          push_back(misc.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
  | /* Empty */
    {
      auto nodes = new mud::core::poly_vector<mud::html::node>();
      $<nodes>$ = nodes;
    }
;

misc
  : comment
;

doctype_opt
  : doctype
    {
      $<node>$ = nullptr;
    }
  | /* Empty */
    {
      $<node>$ = nullptr;
    }
;

doctype
  : DOCTYPE NAME doctype_legacy_opt CLOSETAG
    {
      std::unique_ptr<std::string> name($<str>2);
    }
;

doctype_legacy_opt
  : NAME doctype_legacy_compat
    {
      std::unique_ptr<std::string> name($<str>1);
    }
  | /* Empty */
;

doctype_legacy_compat
  : DQUOTE NAME DQUOTE
    {
      std::unique_ptr<std::string> name($<str>2);
    }
  | SQUOTE NAME SQUOTE
    {
      std::unique_ptr<std::string> name($<str>2);
    }
;

comment
  : COMMENTOPEN COMMENT COMMENTCLOSE
    {
      auto comment = new mud::html::comment;
      std::unique_ptr<std::string> text($<str>2);
      comment->text(std::move(*text));
      $<node>$ = comment;
    }
  | COMMENTOPEN COMMENTCLOSE
    {
      auto comment = new mud::html::comment;
      $<node>$ = comment;
    }
;

html
  : OPENTAG HTML attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name("html");
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
  | OPENTAG HTML attribute_seq_opt CLOSETAG content OPENTAGEND HTML CLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      std::unique_ptr<mud::core::poly_vector<mud::html::node>> nodes($<nodes>5);
      element->name("html");
      element->attributes(std::move(*attrs));
      element->nodes(std::move(*nodes));
      $<element>$ = element;
    }
;

element
  : void_element
  | raw_text_element
  | esc_raw_text_element
  | custom_element
;

void_element
  : OPENTAG void_tag attribute_seq_opt CLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
  | OPENTAG void_tag attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
;

void_tag
  : AREA   { $<str>$ = new std::string("area"); }
  | BASE   { $<str>$ = new std::string("base"); }
  | BR     { $<str>$ = new std::string("br"); }
  | COL    { $<str>$ = new std::string("col"); }
  | EMBED  { $<str>$ = new std::string("embed"); }
  | HR     { $<str>$ = new std::string("hr"); }
  | IMG    { $<str>$ = new std::string("img"); }
  | INPUT  { $<str>$ = new std::string("input"); }
  | LINK   { $<str>$ = new std::string("link"); }
  | META   { $<str>$ = new std::string("meta"); }
  | PARAM  { $<str>$ = new std::string("param"); }
  | SOURCE { $<str>$ = new std::string("source"); }
  | TRACK  { $<str>$ = new std::string("tarck"); }
  | WBR    { $<str>$ = new std::string("wbr"); }
;

raw_text_element
  : OPENTAG raw_text_tag attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
  | OPENTAG raw_text_tag attribute_seq_opt CLOSETAG raw_opt OPENTAGEND raw_text_tag CLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::string> etag($<str>7);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      if ($<node>5 != nullptr) {
          auto nodes = new mud::core::poly_vector<mud::html::node>();
          std::unique_ptr<mud::html::node> raw($<node>5);
          push_back(raw.get(), nodes);
          element->nodes(std::move(*nodes));
      }
      $<element>$ = element;
    }
;

raw_text_tag
  : SCRIPT { $<str>$ = new std::string("script"); }
  | STYLE  { $<str>$ = new std::string("style"); }
;

esc_raw_text_element
  : OPENTAG esc_raw_text_tag attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
  | OPENTAG esc_raw_text_tag attribute_seq_opt CLOSETAG chardata_opt OPENTAGEND esc_raw_text_tag CLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> stag($<str>2);
      std::unique_ptr<std::string> etag($<str>7);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(std::move(*stag));
      element->attributes(std::move(*attrs));
      if ($<node>5 != nullptr) {
          auto nodes = new mud::core::poly_vector<mud::html::node>();
          std::unique_ptr<mud::html::node> chardata($<node>5);
          push_back(chardata.get(), nodes);
          element->nodes(std::move(*nodes));
      }
      $<element>$ = element;
    }
;

esc_raw_text_tag
  : TEXTAREA { $<str>$ = new std::string("textarea"); }
  | TITLE  { $<str>$ = new std::string("title"); }
;

custom_element
  : OPENTAG NAME attribute_seq_opt EMPTYCLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> name($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      element->name(*name);
      element->attributes(std::move(*attrs));
      $<element>$ = element;
    }
  | OPENTAG NAME attribute_seq_opt CLOSETAG content OPENTAGEND NAME CLOSETAG
    {
      auto element = new mud::html::element;
      std::unique_ptr<std::string> name($<str>2);
      std::unique_ptr<std::vector<mud::html::attribute>> attrs($<attributes>3);
      std::unique_ptr<mud::core::poly_vector<mud::html::node>> nodes($<nodes>5);
      element->name(*name);
      element->attributes(std::move(*attrs));
      element->nodes(std::move(*nodes));
      $<element>$ = element;
    }
;

attribute_seq_opt
  : attribute_seq
  | /* Empty */
    {
      auto attrs = new std::vector<mud::html::attribute>();
      $<attributes>$ = attrs;
    }
;

attribute_seq
  : attribute_seq attribute
    {
      auto attrs = $<attributes>1;
      std::unique_ptr<mud::html::attribute> attr($<attribute>2);
      attrs->push_back(std::move(*attr));
      $<attributes>$ = attrs;
    }
  | attribute
    {
      auto attrs = new std::vector<mud::html::attribute>();
      std::unique_ptr<mud::html::attribute> attr($<attribute>1);
      attrs->push_back(std::move(*attr));
      $<attributes>$ = attrs;
    }
;

attribute
  : NAME EQ attvalue
    {
      std::unique_ptr<std::string> name($<str>1);
      std::unique_ptr<std::string> value($<str>3);
      auto attr = new mud::html::attribute();
      attr->name(std::move(*name));
      attr->value(std::move(*value));
      $<attribute>$ = attr;
    }
  | NAME
    {
      std::unique_ptr<std::string> name($<str>1);
      auto attr = new mud::html::attribute();
      attr->name(std::move(*name));
      $<attribute>$ = attr;
    }
;

attvalue
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
  | ATTVALUE
    {
      $<str>$ = $<str>1;
    }
;

content
  : chardata_opt contents_seq_opt
    {
      auto nodes = $<nodes>2;
      if ($<node>1 != nullptr) {
          std::unique_ptr<mud::html::node> char_data($<node>1);
          push_front(char_data.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
;

contents_seq_opt
  : contents_seq
  | /* Empty */
    {
      auto nodes = new mud::core::poly_vector<mud::html::node>();
      $<nodes>$ = nodes;
    }
;

contents_seq
  : contents_seq contents_choice chardata_opt
    {
      auto nodes = $<nodes>1;
      std::unique_ptr<mud::html::node> choice($<node>2);
      push_back(choice.get(), nodes);
      if ($<node>3 != nullptr) {
          std::unique_ptr<mud::html::node> char_data($<node>3);
          push_back(char_data.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
  | contents_choice chardata_opt
    {
      auto nodes = new mud::core::poly_vector<mud::html::node>();
      std::unique_ptr<mud::html::node> choice($<node>1);
      push_back(choice.get(), nodes);
      if ($<node>2 != nullptr) {
          std::unique_ptr<mud::html::node> char_data($<node>2);
          push_back(char_data.get(), nodes);
      }
      $<nodes>$ = nodes;
    }
;

contents_choice
  : element
  | comment
  | cdsect
;

cdsect
  : CDSTART CDATA CDEND
    {
      auto cdata_section = new mud::html::cdata_section;
      std::unique_ptr<std::string> text($<str>2);
      cdata_section->text(std::move(*text));
      $<node>$ = cdata_section;
    }
;

chardata_opt
  : CHARDATA
    {
      auto char_data = new mud::html::char_data;
      std::unique_ptr<std::string> text($<str>1);
      char_data->text(std::move(*text));
      $<node>$ = char_data;
    }
  | /* Empty */
    {
      $<node>$ = nullptr;
    }
;

raw_opt
 : RAW
   {
      auto raw_data = new mud::html::raw_data;
      std::unique_ptr<std::string> text($<str>1);
      raw_data->text(std::move(*text));
      $<node>$ = raw_data;
   }
 | /* Empty */
   {
      $<node>$ = nullptr;
   }
;

%%

