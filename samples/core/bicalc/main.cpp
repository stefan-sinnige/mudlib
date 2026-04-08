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

#include "parser.h"
#include <string.h>
#include <iostream>

/* External declarations */
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern int yyparse();

void
help()
{
    std::cout <<
R"(Usage: bicalc [OPTIONS] [EXPRESSION]

Calculator for big interger arithmetic expressions.

OPTIONS:
    -h, --help    Show this help

An EXPRESSION is a mathematical equation that is evaluated on integer values
and may include the following operations
    +    Addition
    -    Subtraction
    *    Multiplication
    /    Division
    %    Modulus
    ^    Exponent
    ()   Parenthesis
If no expression is presented as an argument, it shall be read from the console
input in an interactive manner. The usual BODMAN/PEMDAS order of operations is
observed. The use of parenthesis can be used to override operation order.

If used in an interactive manner, reading the expressions from the console
input, each expression should be terminated by a new-line, after which the
result of that expression of that line is returned.`)" << std::endl;
}

int
main(int argc, char** argv)
{
    // Command line arguments
    while (--argc && *(++argv)[0] == '-') {
        if ((strcmp(*argv, "-h") == 0) || (strcmp(*argv, "--help") == 0)) {
            help();
            return 0;
        }
        else {
            std::cerr << "Unkown option '" << *argv << "'" << std::endl;
            return 1;
        }
    }

    // If there is an expression on the command-line, set the scanner to read
    // from the string instead of console input.
    if (argc > 0) {
        (void) yy_scan_string(*argv);
    }
    return yyparse();
}
