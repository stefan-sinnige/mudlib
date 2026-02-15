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

#include <mud/test.h>
#include <stdio.h>
#include <string.h>

void
help(int retval)
{
    std::cout << "Command line options:\n"
                 "  --test <spec>    Specify the test(s) to run:\n"
                 "                       feature[#<scenario>]\n"
                 "  --help           Show this help\n";
    exit(retval);
}

int
main(int argc, char** argv)
{
    /* Parse command line arguments */
    std::string test;
    while (--argc && *(++argv)[0] == '-') {
        if (strcmp(*argv, "--test") == 0) {
            if (!--argc) {
                std::cerr << "Option --test requires a test specification\n";
                help(1);
            } else {
                test = *(++argv);
            }
        } else if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0) {
            help(0);
        } else {
            std::cerr << "Unrecognised command line option(s)\n";
            help(1);
        }
    }
    if (argc) {
        std::cerr << "Unrecognised command line option(s)\n";
        help(1);
    }

    /* Run all features */
    std::pair<size_t, size_t> result = FEATURE_RUN(test);

    /* Return 0 upon success */
    return result.first == result.second ? 0 : 1;
}

/* vi: set ai ts=4 expandtab: */
