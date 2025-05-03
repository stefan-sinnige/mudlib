#include "mud/xml/query.h"
#include "mud/xml/exception.h"
#include "src/operation_tree.h"
#include <iostream>
#include <stack>
#include <string>

BEGIN_MUDLIB_XML_NS

/* ======================================================================
 * Class query_parser
 * ====================================================================== */

/**
 * @brief The query parser to compile an expression to an XPath operation tree.
 *
 * @details
 * The @c query_parser parses the XPath query into an operation tree. The
 * operation tree can be evaluated against an XML Info Set (an XML document)
 * to retrieve the selected items.
 *
 * At the moment, this is a hand-built parser. This may be replaced by a
 * Lex/Yacc based parser generator in future.
 */
class query_parser
{
public:
    /**
     * @brief Create a new parser.
     */
    query_parser() = default;

    /**
     * @brief Compile the query to an XML operation tree.
     *
     * @detail
     * Compile the XPath query to an @c operation_tree. If the XPath query can
     * not be compiled successfully, a @c mud::xml::exception is raised.
     *
     * @param xpath_query The XPath query to compile.
     * @return The XML operation tree.
     */
    const xpath::operation_tree& compile(const std::string& xpath_query);

private:
    /* The token from the lexical scanner. The copying semantics uses a
     * copy of the values which may not be truly efficient. */
    struct token {
        /* The token type */
        enum class type_t {
            EOQ,                /**< Enf of query */
            ERROR,              /**< Error in lexical anaylser */
            SEPARATOR,          /**< A path separator "/" */
            DOUBLE_SEPARATOR,   /**< A path double-separator "//" */
            OPEN_PARENTHESIS,   /**< Open parenthesis "(" */
            CLOSE_PARENTHESIS,  /**< Close parenthesis ")" */
            OPEN_BRACKET,       /**< Open parenthesis "[" */
            CLOSE_BRACKET,      /**< Close parenthesis "]" */
            DOUBLE_COLON,       /**< Double colon "::" */
            AT,                 /**< At symbol "@" */
            STRING              /**< Generic string, ie element name */
        };
   
        /* Any associated value type */
        typedef union {
            std::string* string;
        } value_t;

        /* Construct a token and initialise it's value */
        token(type_t t) : type(t) {}
        token(type_t t, const std::string& v) : type(t) {
            value.string = new std::string(v);
        }
        token(const token& other) {
            this->operator=(other);
        }
        token& operator=(const token& other) {
            if (this == &other) {
                return *this;
            }
            type = other.type;
            if (type == type_t::STRING) {
                value.string = new std::string(*other.value.string);
            }
            return *this;
        }

        /* Destructor */
        ~token() {
            switch (type) {
                case type_t::STRING:
                    delete value.string;
                    break;
                default:
                    break;
            }
        }

        /* The token type and value. */
        type_t type;
        value_t value;
    };

    /**
     * @brief Return the next token from the lexical scanning scanner.
     *
     * @return The next token from the query. If there are no more tokens,
     * the EOQ (end-of-query) token is returned.
     */
    token lex();

    /**
     * @brief Push a token to the stack.
     *
     * @details
     * When a token has been retrieved by the lexical analyser but not used by
     * an XPath reduce rule, it can be pushed to the stack such that it will be
     * returned again with the next invocation of @c lex.
     *
     * @param token The token to push.
     */
    void push(token tok);

    /**
     * @brief Reducing a subset of the XPath gramnar BNF rules.
     */
    std::shared_ptr<xpath::expr> reduce_xpath();
    std::shared_ptr<xpath::path_expr> reduce_path_expr();
    std::shared_ptr<xpath::step_expr> reduce_step_expr();
    std::shared_ptr<xpath::axis_step> reduce_axis_step();
    std::shared_ptr<xpath::postfix_expr> reduce_postfix_expr();

    /** The query to evaluate */
    std::string _query;

    /** The current lexical analyser position, pointing ot the first character
     * to be tokenised. */
    std::string::size_type _pos;

    /** The saved lexical token stack. */
    std::stack<token> _saved_tokens;

    /** The operation tree. */
    xpath::operation_tree _op_tree;
};

const xpath::operation_tree&
query_parser::compile(const std::string& xpath_query)
{
    // Initialise the lexical scanner.
    _query = xpath_query;
    _pos = 0;

    // Manually built rudimentary XPath parser to build the operation-tree.
    _op_tree.expr() = reduce_xpath();

    return _op_tree;
}

std::shared_ptr<xpath::expr>
query_parser::reduce_xpath()
{
    // Accepts the following subset
    //    [ 1] XPath               ::= Expr
    //    [ 6] Expr                ::= ExprSingle
    //    [ 7] ExprSingle          ::= OrExpr
    //    [16] OrExpr              ::= AndExpr
    //    [17] AndExpr             ::= ComparisonExpr
    //    [18] ComparisonExpr      ::= StringConcatExpr
    //    [19] StringConcatExpr    ::= RangeExpr
    //    [20] RangeExpr           ::= AdditiveExpr
    //    [21] AdditiveExpr        ::= MultiplicativeExpr
    //    [22] MultiplicativeExpr  ::= UnionExpr
    //    [23] UnionExpr           ::= IntersectExceptExpr
    //    [24] IntersectExceptExpr ::= InstanceOfExpr
    //    [25] InstanceOfExpr      ::= TreatExpr
    //    [26] TreatExpr           ::= CastableExpr
    //    [27] CastableExpr        ::= CastExpr
    //    [28] CastExpr            ::= ArrowExpr
    //    [29] ArrowExpr           ::= UnaryExpr
    //    [30] UnaryExpr           ::= ValueExpr
    //    [31] ValueExpr           ::= SimpleMapExpr
    //    [35] SimpleMapExpr       ::= PathExpr
    //
    // Or in simple terms:
    //    [ 1] XPath               ::= PathExpr

    auto expr = std::make_shared<xpath::expr>();
    expr->path_expr()= reduce_path_expr();
    return expr;
}

std::shared_ptr<xpath::path_expr>
query_parser::reduce_path_expr()
{
    // Accepts the following rules
    //    [36] PathExpr         ::= "/" RelativePathExpr    |
    //                              "//" RelativePathExpr   |
    //                              RelativePathExpr
    //    [37] RelativePathExpr ::= StepExpr ( ("/" | "//" ) StepExpr )*

    auto expr = std::make_shared<xpath::path_expr>();

    // Check if we start with a separator
    token tok = lex();
    switch (tok.type) {
        case token::type_t::ERROR:
            throw xml::exception(*tok.value.string);
            break;
        case token::type_t::EOQ:
            break;
        case token::type_t::SEPARATOR:
        case token::type_t::DOUBLE_SEPARATOR:
            break;
        default:
            push(tok);
            break;
    }

    // Only reduce one or more step expressions
    bool reduce_step = true;
    while (reduce_step)
    {
        // Reduce a step expression
        auto step = reduce_step_expr();
        if (!step) {
            break;
        }
        expr->step_exprs().push_back(step);

        // Accept any separator and continue reducing steps. 
        tok = lex();
        switch (tok.type) {
            case token::type_t::ERROR:
                throw mud::xml::exception(*tok.value.string);
                break;
            case token::type_t::EOQ:
                reduce_step = false;
                break;
            case token::type_t::SEPARATOR:
            case token::type_t::DOUBLE_SEPARATOR:
                // Record which one ... somewhere
                break;
            default:
                reduce_step = false;
                push(tok);
                break;
        }
    }

    return expr;
}

std::shared_ptr<xpath::step_expr>
query_parser::reduce_step_expr()
{
    // Accepts the following rule
    //    [38] StepExpr ::= PostfixExpr | AxisStep

    // Expecting a STRING or an AT
    token tok = lex();
    switch (tok.type) {
        case token::type_t::ERROR:
            throw mud::xml::exception(*tok.value.string);
            break;
        case token::type_t::EOQ:
            return nullptr;
        case token::type_t::STRING:
            break;
        case token::type_t::AT:
            break;
        default:
            throw mud::xml::exception("XPath syntax error");
    }

    // Check the next token that determines what step expression we have. Note
    // that we need to push-back all the tokens so each expression can parse
    // it properly.
    token lookahead = lex();
    push(lookahead);
    push(tok);
    switch (lookahead.type) {
        case token::type_t::EOQ:
        case token::type_t::SEPARATOR:
        case token::type_t::DOUBLE_SEPARATOR:
        case token::type_t::DOUBLE_COLON:
        case token::type_t::OPEN_BRACKET: 
        case token::type_t::STRING: 
        {
            if (tok.type == token::type_t::AT &&
                lookahead.type != token::type_t::STRING)
            {
                throw mud::xml::exception("XPath syntax error");
            }
            return reduce_axis_step();
        }
        case token::type_t::OPEN_PARENTHESIS:
        {
            return reduce_postfix_expr();
        }
        default:
            throw mud::xml::exception("XPath syntax error");
    }

    return nullptr;
}

std::shared_ptr<xpath::axis_step>
query_parser::reduce_axis_step()
{
    // Only accepts the following subset
    //    [39] AxisStep          ::= ( ReverseStep | ForwardStep )
    //    [40] ForwardStep       ::= ( ForwardAxis NodeTest )      | 
    //                               AbbrevForwardStep
    //    [43] ReverseStep       ::= ( ReverseAxis NodeTest )      |
    //                               AbbrevReverseStep
    //    [41] ForwardAxis       ::= ( "child" "::" )              |
    //                               ( "descendant" "::" )         |
    //                               ( "attribute" "::" )          |
    //                               ( "self" "::" )               |
    //                               ( "descendant-or-self" "::" ) |
    //                               ( "following-sibling" "::" )  |
    //                               ( "following" "::" )          |
    //                               ( "namespace" "::" )
    //    [41] ReverseAxis       ::= ( "parent" "::" )             |
    //                               ( "ancestor" "::" )           |
    //                               ( "preceding-sibling" "::" )  |
    //                               ( "preceding" "::" )          |
    //                               ( "ancestor-or-self" "::" )
    //    [42] AbbrevForwardStep ::= "@"? NodeTest
    //    [45] AbbrevReverseStep ::= ".."
    //    [46] NodeTest          ::= NameTest
    //    [47] NameTest          ::= EQName 
    //                             | Wildcard
    //   [112] EQName            ::= QName
    //   [122] QName             ::= Prefix ':' LocalPart
    //                             | Localpart

    auto expr = std::make_shared<xpath::axis_step>();

    // Expecting a STRING
    token tok = lex();
    switch (tok.type) {
        case token::type_t::ERROR:
            throw mud::xml::exception(*tok.value.string);
            break;
        case token::type_t::EOQ:
            return nullptr;
        case token::type_t::STRING:
            break;
        case token::type_t::AT:
            expr->axis() = xpath::axis_step::axis_t::ATTRIBUTE;
            tok = lex();
            if (tok.type != token::type_t::STRING) {
                throw mud::xml::exception("XPath syntax error");
            }
            break;
        default:
            throw mud::xml::exception("XPath syntax error");
    }

    // Determine what the string represents.
    bool name_test = false;
    token lookahead = lex();
    switch (lookahead.type) {
        case token::type_t::ERROR:
            throw mud::xml::exception(*tok.value.string);
            break;
        case token::type_t::DOUBLE_COLON:
            if (*tok.value.string == "attribute") {
                expr->axis() = xpath::axis_step::axis_t::ATTRIBUTE;
            }
            else
            if (*tok.value.string == "child") {
                expr->axis() = xpath::axis_step::axis_t::CHILD;
            }
            else
            if (*tok.value.string == "descendant-or-self") {
                expr->axis() = xpath::axis_step::axis_t::DESCENDANT_OR_SELF;
            }
            else {
                throw mud::xml::exception("XPath unsupported axis");
            }
            name_test = true;
        default:
            push(lookahead);
            expr->name_test() = *tok.value.string;
            break;
    }

    // If we need a name-test, accept it.
    if (name_test) {
        token tok = lex();
        switch (tok.type) {
            case token::type_t::ERROR:
                throw mud::xml::exception(*tok.value.string);
                break;
            case token::type_t::STRING:
                expr->name_test() = *tok.value.string;
                break;
            default:
                throw mud::xml::exception("XPath syntax error");
        }
    }

    return expr;
}

std::shared_ptr<xpath::postfix_expr>
query_parser::reduce_postfix_expr()
{
    // Only accept the following subset
    //    [49] PostfixExpr  ::= PrimaryExpr
    //    [56] PrimaryExpr  ::= FunctionCall
    //    [63] FunctionCall ::= EQName "(" ")"

    // Expecting a STRING
    token tok = lex();
    switch (tok.type) {
        case token::type_t::ERROR:
            throw mud::xml::exception(*tok.value.string);
            break;
        case token::type_t::EOQ:
            return nullptr;
        case token::type_t::STRING:
            break;
        default:
            throw mud::xml::exception("XPath syntax error");
    }

    // Expecting the open and close parenthesis
    token open_par = lex();
    switch (open_par.type) {
        case token::type_t::OPEN_PARENTHESIS:
            break;
        case token::type_t::ERROR:
            throw mud::xml::exception(*open_par.value.string);
            break;
        default:
            throw mud::xml::exception("XPath syntax error");
    }
    token close_par = lex();
    switch (close_par.type) {
        case token::type_t::CLOSE_PARENTHESIS:
            break;
        case token::type_t::ERROR:
            throw mud::xml::exception(*close_par.value.string);
            break;
        default:
            throw mud::xml::exception("XPath syntax error");
    }

    // Create the function
    auto expr = std::make_shared<xpath::postfix_expr>();
    auto func = std::make_shared<xpath::function_call>();
    func->name() = *tok.value.string;
    expr->primary_expr() = func;
    return expr;
}

query_parser::token
query_parser::lex()
{
    // The lookup table of allowable (UTF-8) characters of a certain type.
    uint8_t table[128] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // NUL .. BEL
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  BS .. SI
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // DLE .. ETB
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CAN .. US
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  SP .. '
        0x02, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, //   ( .. /
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   0 .. 7
        0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, //   8 .. ?
        0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   @ .. G
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   H .. O
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   P .. W
        0x01, 0x01, 0x01, 0x02, 0x00, 0x02, 0x00, 0x00, //   X .. _
        0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   ` .. g
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   h .. o
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //   p .. w
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //   x .. DEL
    };

    // The mask in the character lookup table.
    const uint8_t name = 0x01;
    const uint8_t special = 0x02;

    // Lookahead n'th characters and return the character. If there are no more
    // characters, return EOF.
    auto lookahead = [this](int n = 1) -> char {
        if (this->_pos + n != std::string::npos &&
            this->_pos < this->_query.size())
        {
            return this->_query[this->_pos + n];
        }
        return EOF;
    };

    // Get the character at the current position. If there are no more
    // characetrs, return EOF;
    auto at = [this]() -> char {
        if (this->_pos != std::string::npos &&
            this->_pos < this->_query.size())
        {
            return this->_query[this->_pos];
        }
        return EOF;
    };

    // Advance to the next position and return the character at that position.
    // If there are no more characters, return EOF.
    auto advance = [this]() -> char {
        if (this->_pos != std::string::npos &&
            this->_pos < this->_query.size() - 1)
        {
            ++this->_pos;
            return this->_query[this->_pos];
        }
        else 
        {
            this->_pos = std::string::npos;
            return EOF;
        }
    };

    // If there are saved tokens on the stack, return these first.
    if (_saved_tokens.size()) {
        auto tok = _saved_tokens.top();
        _saved_tokens.pop();
        return tok;
    }

    // Initialise the tokenising
    auto start = _pos;
    char ch = at();

    // Check if we're at the end
    if (ch == EOF) {
        return token(token::type_t::EOQ);
    }

    // Process any special characters
    if (table[ch] & special) {
        advance();
        switch (ch) {
            case '(':
                return token(token::type_t::OPEN_PARENTHESIS);
            case ')':
                return token(token::type_t::CLOSE_PARENTHESIS);
            case '[':
                return token(token::type_t::OPEN_BRACKET);
            case ']':
                return token(token::type_t::CLOSE_BRACKET);
            case '@':
                return token(token::type_t::AT);
            case ':':
                ch = advance();
                if (ch != ':') {
                    return token(token::type_t::ERROR, "XPath syntax error");
                }
                return token(token::type_t::DOUBLE_COLON);
            default:
                return token(token::type_t::ERROR, "XPath unsupported special character");
        }
    }

    // Process any path expression separator.
    if (ch == '/') {
        ch = advance();
        if (ch == '/') {
            advance();
            return token(token::type_t::DOUBLE_SEPARATOR);
        }
        return token(token::type_t::SEPARATOR);
    }

    // Name
    if (table[ch] & name) {
        do {
            ch = advance();
        } while (ch != EOF && table[ch] & name);
        if (ch == EOF) {
            return token(token::type_t::STRING,
                         _query.substr(start));
        }
        else {
            return token(token::type_t::STRING,
                         _query.substr(start, _pos - start));
        }
    }
    // If we've reached the end, we're done.
    if (_pos == std::string::npos) {
        return token(token::type_t::EOQ);
    }

    // Unknown token
    return token(token::type_t::ERROR, "XPath syntax error");
}

void
query_parser::push(query_parser::token tok)
{
    // Push the token
    _saved_tokens.push(tok);
}

/* ======================================================================
 * Class query_result
 * ====================================================================== */

query_result::query_result()
    : _underlying(underlying_t::UNSET)
{
}

const mud::xml::node_seq&
query_result::nodes() const
{
    return _nodes;
}

const std::vector<std::string>&
query_result::strings() const
{
    return _strings;
}

void
query_result::append(const mud::xml::node::ptr& item)
{
    if (_underlying != underlying_t::NODE) {
        if (_underlying == underlying_t::UNSET) {
            _underlying = underlying_t::NODE;
        }
        else {
            throw mud::xml::exception("XPath result type changed");
        }
    }
    _nodes.push_back(item);
}

void
query_result::append(const std::string& item)
{
    if (_underlying != underlying_t::STRING) {
        if (_underlying == underlying_t::UNSET) {
            _underlying = underlying_t::STRING;
        }
        else {
            throw mud::xml::exception("XPath result type changed");
        }
    }
    _strings.push_back(item);
}

/* ======================================================================
 * Class query implementation
 * ====================================================================== */

/**
 * @brief The implementation of the query class.
 *
 * @details
 * The XPath query will compile the xpatch query and evaluate it against an XML
 * document.
 */

class query::impl
{
public:
    /**
     * @brief Construct a compiled query.
     */
    impl(const std::string& xpath_query);

    /**
     * @brief Evaluate the XPath query against a document.
     */
    query_result evaluate(const xml::document::ptr& doc);

    /**
     * @brief Evaluate the XPath query against a node.
     */
    query_result evaluate(const xml::node::ptr& node);

private:
    /**
     * Evaluate an expression against a node.
     *
     * @param expr The expression to evaluate.
     * @param node The node to start evaluating against.
     * @param result The object to add the result to.
     *
     * @return The result of the expression.
     */
    query_result evaluate(
            const xpath::basic_expression& expr,
            const xml::node::ptr& context_node);
    query_result evaluate(
            const xpath::expr& expr,
            const xml::node::ptr& context_node);
    query_result evaluate(
            const xpath::path_expr& expr,
            const xml::node::ptr& context_node);
    query_result evaluate(
            const xpath::step_expr& expr,
            const xml::node_seq& context_nodes);
    query_result evaluate(
            const xpath::axis_step& expr,
            const xml::node_seq& context_nodes);
    query_result evaluate(
            const xpath::postfix_expr& expr,
            const xml::node_seq& context_nodes);
    query_result evaluate(
            const xpath::function_call& expr,
            const xml::node_seq& context_nodes);

    /* The XPath query string */
    std::string _query;

    /* The compiled XPath query as an operation tree. */
    xpath::operation_tree _op_tree;
};

query::impl::impl(const std::string& xpath_query)
    : _query(xpath_query)
{
    // Compile the XPath query.
    query_parser parser;
    _op_tree = parser.compile(_query);
}

query_result 
query::impl::evaluate(const xml::document::ptr& doc)
{
    if (doc == nullptr) {
        return query_result();
    }
    return evaluate(*_op_tree.expr(), doc);
}

query_result 
query::impl::evaluate(const xml::node::ptr& node)
{
    if (node == nullptr) {
        return query_result();
    }
    return evaluate(*_op_tree.expr(), node);
}

query_result
query::impl::evaluate(
        const xpath::basic_expression& expr,
        const xml::node::ptr& context_node)
{
    switch (expr.type())
    {
        case xpath::basic_expression::type_t::EXPR:
            return evaluate(static_cast<const xpath::expr&>(expr),
                   context_node);
        case xpath::basic_expression::type_t::PATH_EXPR:
            return evaluate(static_cast<const xpath::path_expr&>(expr),
                   context_node);
        case xpath::basic_expression::type_t::AXIS_STEP:
            return evaluate(static_cast<const xpath::axis_step&>(expr),
                   context_node);
        case xpath::basic_expression::type_t::POSTFIX_EXPR:
            return evaluate(static_cast<const xpath::postfix_expr&>(expr),
                   context_node);
        case xpath::basic_expression::type_t::FUNCTION_CALL:
            return evaluate(static_cast<const xpath::function_call&>(expr),
                   context_node);
    }
    return query_result();
}

query_result
query::impl::evaluate(
        const xpath::expr& expr,
        const xml::node::ptr& context_node)
{
    if (expr.path_expr()) {
        return evaluate(*expr.path_expr(), context_node);
    }
    return query_result();
}

query_result
query::impl::evaluate(
        const xpath::path_expr& expr,
        const xml::node::ptr& context_node)
{
    // Evaluating the sequence of step expressions using the path operator (as
    // defined in XPath 3.1 section 3.3.1.1). Each step expression returns a
    // sequence of nodes matching that step expression which will be passed
    // as input to the next step expression. The final step-expression to be
    // executed may return a result that is not a sequence of nodes, but
    // a sequence of non-nodes (ie a sequence of strings).
    query_result result;
    xml::node_seq context_nodes;
    context_nodes.push_back(context_node);
    for (auto& step: expr.step_exprs())
    {
        result = evaluate(*step, context_nodes);
        context_nodes = result.nodes();
    }
    return result;
}

query_result
query::impl::evaluate(
        const xpath::step_expr& expr,
        const xml::node_seq& context_nodes)
{
    switch (expr.type())
    {
        case xpath::basic_expression::type_t::AXIS_STEP:
            return evaluate(static_cast<const xpath::axis_step&>(expr),
                   context_nodes);
        case xpath::basic_expression::type_t::POSTFIX_EXPR:
            return evaluate(static_cast<const xpath::postfix_expr&>(expr),
                   context_nodes);
        default:
            return query_result();
    }
}

query_result
query::impl::evaluate(
        const xpath::axis_step& expr,
        const xml::node_seq& context_nodes)
{
    // Evaluate over all context nodes and combine the results. At this moment,
    // only support a 'QName ::= LocalPart' as a name-test (namespaces prefixes
    // are currently not supported).
    query_result result;
    for (auto context_node: context_nodes)
    {
        switch (expr.axis())
        {
            case xpath::axis_step::axis_t::ATTRIBUTE:
                if (context_node->type() == xml::node::type_t::ELEMENT)
                {
                    auto element = std::static_pointer_cast<xml::element>(context_node);
                    for (auto attr: element->attributes()) {
                        if (attr->local_name() == expr.name_test()) {
                            result.append(attr);
                        }
                    }
                }
                break;
            case xpath::axis_step::axis_t::CHILD:
                for (auto node: context_node->children()) {
                    if (node->type() == xml::node::type_t::ELEMENT)
                    {
                        auto element = std::static_pointer_cast<xml::element>(node);
                        if (element->local_name() == expr.name_test()) {
                            result.append(node);
                        }
                    }
                }
                break;
            case xpath::axis_step::axis_t::DESCENDANT_OR_SELF:
                break;
            default:
                break;
        }
    }
    return result;
}

query_result
query::impl::evaluate(
        const xpath::postfix_expr& expr,
        const xml::node_seq& context_nodes)
{
    auto primary_expr = expr.primary_expr();
    switch (primary_expr->type())
    {
        case xpath::basic_expression::type_t::FUNCTION_CALL:
            return evaluate(
                static_cast<const xpath::function_call&>(*primary_expr),
                context_nodes);
        default:
            break;
    }
    return query_result();
}

query_result
query::impl::evaluate(
        const xpath::function_call& expr,
        const xml::node_seq& context_nodes)
{
    // Only support 'text()' for now
    query_result result;
    if (expr.name() == "text") {
        for (auto context_node: context_nodes)
        {
            std::string str;
            for (auto node: context_node->children()) {
                switch (node->type())
                {
                    case xml::node::type_t::CHAR_DATA:
                        str += std::static_pointer_cast<xml::char_data>(
                            node)->text();
                        break;
                    case xml::node::type_t::CDATA_SECTION:
                        str += std::static_pointer_cast<xml::cdata_section>(
                            node)->text();
                        break;
                    default:
                        break;
                }
            }
            result.append(str);
        }

    }
    return result;
}

/* ======================================================================
 * Class query
 * ====================================================================== */

query::query()
{
    _impl = nullptr;
}

query::query(const std::string& xpath_query)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(xpath_query));
}

query::query(const query& other)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*other._impl));
}

query::query(query&& other)
{
    if (this != &other) {
        _impl = std::move(other._impl);
    }
}

query&
query::operator=(const query& other)
{
    if (this != &other) {
        _impl = std::unique_ptr<impl, impl_deleter>(new impl(*other._impl));
    }
    return *this;
}

query&
query::operator=(query&& other)
{
    if (this != &other) {
        _impl = std::move(other._impl);
    }
    return *this;
}

query_result 
query::evaluate(const xml::document::ptr& xml_doc)
{
    query_result result;
    if (_impl) {
        result = _impl->evaluate(xml_doc);
    }
    return result;
}

query_result 
query::evaluate(const xml::node::ptr& xml_node)
{
    query_result result;
    if (_impl) {
        result = _impl->evaluate(xml_node);
    }
    return result;
}

void
query::impl_deleter::operator()(query::impl* ptr) const
{
    delete ptr;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
