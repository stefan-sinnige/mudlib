#include "mud/xml.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    bool debug = false;

    // Command line arguments
    while (--argc > 0 && *(++argv)[0] == '-') {
        if (strcmp(*argv, "--debug") == 0) {
            debug = true;
        }
    }

    // Define the input stream. If there is an argument, the argument is the
    // name of the file, otherwise use standard input.
    std::istream* istr;
    if (argc >= 1) {
        std::cout << "Using filename " << *argv << std::endl;
        istr = new std::ifstream(*argv);
    } else {
        istr = &std::cin;
    }

    // Extract an XML document from the input stream
    mud::xml::document::ptr doc;
    *istr >> mud::xml::parser_debug >> mud::xml::scanner_debug >> doc;

    // Output the XML document to standard-out.
    std::cout << doc;

    // Cleanup
    if (istr != &std::cin) {
        delete istr;
    }

    return 0;
}

/* vi: set ai ts=4 expandtab: */
