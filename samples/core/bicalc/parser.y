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

/* Make the scanner declaration  known to the generated parser. */
%{
  int yylex(void);
%}

/* Define the output file names */
%output "parser.cpp"
%header "parser.h"

/* Make the bigint type for '$$' and '$n' known to the parser and scanner */
%code requires {
  #include <mud/core/bigint.h>
}

/* Define an implementation to print out parsing errors */
%code {
  void yyerror(const char* s) {
    std::cerr << "Error: " << s << std::endl;
  }
}

/* The parser state type is the calculated big integer value */
%define api.value.type {  mud::core::bigint }

/* The known tokens produced by the scanner */
%token BIGINT
%token PLUS MINUS TIMES DIVIDE MODULUS EXPONENT LPAREN RPAREN EOL

/* Define the operator precedence and evaluation order. */
%left  PLUS MINUS            /* Lowest precedence */
%left  TIMES DIVIDE MODULUS  /* Higher precedence */
%right EXPONENT              /* Highest precedence */
%right UMINUS UPLUS          /* Unary minus and unary plus */

%%

statements:
      statements EOL statement
    | statement
    ;

statement:
      expression
      { std::cout << $1 << std::endl; }
    |
    ;

expression:
      BIGINT
    | expression PLUS expression
      { $$ = $1 + $3; }
    | expression MINUS expression
      { $$ = $1 - $3; }
    | expression TIMES expression
      { $$ = $1 * $3; }
    | expression DIVIDE expression
      { $$ = $1 / $3; }
    | expression MODULUS expression
      { $$ = $1 % $3; }
    | expression EXPONENT expression
      { $$ = mud::core::exp($1, $3); }
    | LPAREN expression RPAREN
      { $$ = $2; }
    | PLUS expression %prec UPLUS
      { $$ = $2; }
    | MINUS expression %prec UMINUS
      { $$ = -$2; }
    ;
%%
