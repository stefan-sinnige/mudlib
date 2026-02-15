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

#ifndef _MUDLIB_XML_OPERATION_TREE_H_
#define _MUDLIB_XML_OPERATION_TREE_H_

#include <mud/xml/ns.h>
#include <mud/xml/attribute.h>
#include <mud/xml/cdata_section.h>
#include <mud/xml/char_data.h>
#include <mud/xml/comment.h>
#include <mud/xml/declaration.h>
#include <mud/xml/document.h>
#include <mud/xml/element.h>
#include <mud/xml/exception.h>
#include <mud/xml/node.h>
#include <mud/xml/processing_instruction.h>
#include <memory>
#include <vector>

BEGIN_MUDLIB_XML_NS

namespace xpath {

/* Forward declarations */
class basic_expression;
class expr;
class path_expr;
class step_expr;
class axis_step;
class postfix_expr;
class primary_expr;
class function_call;
class operation_tree;

/**
 * @brief An base class that represents (part of) an XPath expression.
 */
class basic_expression
{
public:
    /**
     * @brief The type of expressions.
     */
    enum class type_t {
        EXPR,
        PATH_EXPR,
        AXIS_STEP,
        POSTFIX_EXPR,
        FUNCTION_CALL
    };

    /**
     * @brief Return the type of the expression.
     */
    type_t type() const { return _type; }

protected:
    /**
     * @brief Constructor, defining the type of expression.
     */
    basic_expression(type_t type) : _type(type) {}

private:
    /** The type of the expression */
    type_t _type;
};

/**
 * @brief Representation of the XPath Expr.
 *
 * @note Only a PathExpr is supported
 */
class expr : public basic_expression
{
public:
    /*
     * @brief Constructor.
     */
    expr()
        : basic_expression(basic_expression::type_t::EXPR)
    {}

    /*
     * @brief Return the path-expression object.
     */
    const std::shared_ptr<mud::xml::xpath::path_expr>& path_expr() const
        { return _expr; }
    std::shared_ptr<mud::xml::xpath::path_expr>& path_expr()
        { return _expr; }

private:
    /* The expression. */
    std::shared_ptr<mud::xml::xpath::path_expr> _expr;
};

/**
 * @brief Representation of the XPath PathExpr
 */
class path_expr : public basic_expression
{
public:
    /*
     * @brief Constructor.
     */
    path_expr()
        : basic_expression(basic_expression::type_t::PATH_EXPR)
    {}

    /*
     * @brief Return the list of step-expression objects.
     */
    const std::vector<std::shared_ptr<mud::xml::xpath::step_expr>>& step_exprs() const
        { return _exprs; }
    std::vector<std::shared_ptr<mud::xml::xpath::step_expr>>& step_exprs()
        { return _exprs; }

private:
    /* The expression. */
    std::vector<std::shared_ptr<mud::xml::xpath::step_expr>> _exprs;
};

/**
 * @brief Representation of the XPath StepExpr
 */
class step_expr : public basic_expression
{
protected:
    step_expr(basic_expression::type_t type)
        : basic_expression(type)
    {}
};

/**
 * @brief Representation of the XPath AxisStep
 */
class axis_step : public step_expr
{
public:
    enum class axis_t {
        ATTRIBUTE,              /**< Select a matching attribute node. This
                                 * can be abbreviated with the "@" symbol. */
        CHILD,                  /**< Select matching child nodes. This is
                                 * equivalent to the '/' path operator. If
                                 * no explicit axis is defined, then this
                                 * is the default axis to use. */
        DESCENDANT_OR_SELF,     /**< Select matching descendants or matching
                                 * self nodes. This is equivalent to the '//'
                                 * path oeprator. */
    };

    /*
     * @brief Constructor.
     */
    axis_step()
        : step_expr(basic_expression::type_t::AXIS_STEP)
        , _axis(axis_t::CHILD)
    {}

    /*
     * @brief The axis.
     *
     * @details
     * If no axis has been explicitely defined, then the axis is by default a
     * @em child axis.
     */
    axis_t axis() const { return _axis; }
    axis_t& axis() { return _axis; }

    /*
     * @brief The name-test.
     */
    const std::string& name_test() const { return _name_test; }
    std::string& name_test() { return _name_test; }

private:
    /* The axis */
    axis_t _axis;

    /* The name test */
    std::string _name_test;
};

/**
 * @brief Representation of the XPath PostfixExpr
 */
class postfix_expr : public step_expr
{
public:
    /*
     * @brief Constructor.
     */
    postfix_expr()
        : step_expr(basic_expression::type_t::POSTFIX_EXPR)
    {}

    /*
     * @brief Return the path-expression object.
     */
    const std::shared_ptr<mud::xml::xpath::primary_expr>& primary_expr() const
        { return _primary_expr; }
    std::shared_ptr<mud::xml::xpath::primary_expr>& primary_expr()
        { return _primary_expr; }

private:
    /* The expression. */
    std::shared_ptr<mud::xml::xpath::primary_expr> _primary_expr;
};

/**
 * @brief Representation of the XPath PrimaryExpr
 */
class primary_expr : public basic_expression
{
protected:
    primary_expr(basic_expression::type_t type)
        : basic_expression(type)
    {}
};

/**
 * @brief Representation of the XPath FunctionCall
 */
class function_call : public primary_expr
{
public:
    /*
     * @brief Constructor.
     */
    function_call()
        : primary_expr(basic_expression::type_t::FUNCTION_CALL)
    {}

    /*
     * @brief The function name.
     */
    const std::string& name() const { return _name; }
    std::string& name() { return _name; }

private:
    /* The function name */
    std::string _name;
};

/**
 * @brief The main operation tree.
 *
 * @details
 * The operation tree is a compiled representation of an XPath query. This
 * operation tree can be used to evaluate the tree against an XML document and
 * in that way evaluate the XPath expression.
 *
 * The operation tree is closely related to the XML Path Language grammer and
 * its BNF representation.
 */
class operation_tree
{
public:
    /**
     * @brief Constructor.
     */
    operation_tree() : _expr(nullptr) {}

    /**
     * @brief Get the expression.
     */
    const std::shared_ptr<mud::xml::xpath::expr>& expr() const
        { return _expr; }
    std::shared_ptr<mud::xml::xpath::expr>& expr()
        { return _expr; }

private:
    std::shared_ptr<mud::xml::xpath::expr> _expr;
};

}; // namespace xpath

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /*  _MUDLIB_XML_OPERATION_TREE_H_ */
