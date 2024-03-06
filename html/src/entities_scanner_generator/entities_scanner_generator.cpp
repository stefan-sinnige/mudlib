/*
 * This is not part of the mudlib library.
 *
 * This tool uses the entities table and creates scanning tables for converting
 *    * Named entitiy to the index in the corresponding entities table.
 *    * UTF-8 codepoint to a corresponding named entitiy.
 * After generating the tables, it uses them to verify their correctness on
 * the same entities table.
 *
 * Compilation:
 *    g++ -std=c++17 -o entities_scanner_generator \
 *          entities_scanner_generator.cpp entities.cpp
 *
 * Usage:
 *    ./entities_scanner_generator --named
 *
 * The scanner is iterating over each entity that needs to be recognised, one
 * at a time. For each character in the entity, it consults or updates the
 * uncompressed table. This table lists the states and acceptable character
 * code and is constructed in such a way that indexed by the current state and
 * the character code of the entity, it points to the next state. For example,
 * we have two entities:
 *    &Acy;
 *    &Afr;
 * Then we can create the following deterministic finite automaton:
 *
 *   0 -- & --> 1 -- A --> 2 -- c --> 3 -- y --> 4 -- ; --> *1
 *                          \-- f --> 5 -- r --> 6 -- ; --> *2
 *
 * Where the numbers represent the state and the characters the input character
 * to be processed. The states 5 and 8 are end-states and can only be reached
 * after it had been following the exact order of characters. This DFA graph
 * can be represented in an uncompressed array as follows.
 *
 *     State  '&' ';' 'c' 'f' 'r' 'y' 'A'
 *      [0]:   1,  0,  0,  0,  0,  0,  0
 *      [1]:   0,  0,  0,  0,  0,  0,  2
 *      [2]:   0,  0,  3,  5,  0,  0,  0
 *      [3]:   0,  0,  0,  0,  0,  4,  0
 *      [4]:   0, *1,  0,  0,  0,  0,  0
 *      [5]:   0,  0,  0,  0,  6,  0,  0
 *      [6]:   0, *2,  0,  0,  0,  0,  0
 *
 * Scanning this table against the input is then very straight forward. Note
 * that we only list a subset the characters here. IN reality, this can be
 * all accetiable ASCII characters or any 8-bit value.
 *
 * The uncompressed table contains lots of zero-values of input characters at
 * a certain state which does not lead to any known pattern, therefore leading
 * to a huge table of many invalid entries. We can compress this table by a
 * technique that removes all leading and trailing zero's in a state and
 * aligning each state underneath while keeping track of the shift:
 *
 *    State Shift
 *     [0]    0    1
 *     [1]   -5       2
 *     [2]    0          3  5
 *     [3]   -1                4
 *     [4]    4                  *1
 *     [5]    2                     6
 *     [6]    6                       *2
 *
 *    Check       0  1  2  2  3  4  5  6
 *
 * This compreased table is now a single-dimensional array and a separate
 * state-shift table. We still need to know if we fall within the bounds of
 * the original state by having a check table for every index. This allows us
 * to check that the index number corresponds with the current state row. The
 * character processing is still straight forward by adding/substracting the
 * state's shift number.
 */

#include "entities_scanner_generator_data.cpp"
#include <cstring>
#include <iomanip>
#include <iostream>

/* The maximum number of states required. If we need more than provided, an
 * error will be generated to increase this number. */
#define MAX_STATES 10000
#define INPUT_RANGE 256
#define START_STATE 1

/* Options */
bool show_statistics = false;   /* Show statistics */
bool add_named = false;         /* Add named entity patterns */
bool add_utf8 = false;          /* Add UTF-8 entity patterns */
bool show_entities = false;     /* Show the entities table */
bool show_uncompressed = false; /* Show the uncompressed table */
bool show_compressed = false;   /* Show the compressed table */
int trace = -1;                 /* Trace a scan of an entity index */

/* The uncompressed and compressed tables and the shifts table. */
int (*uncompressed)[INPUT_RANGE] = { nullptr };
int* compressed = nullptr;
int* shifts = nullptr;
int* checks = nullptr;

/* The current maximum state. */
int max_state = 1;

/* The current maximum index in the compressed table. */
int max_index = 0;

/*
 * Initialise the uncompressed scanner
 */
void
initialise_uncompressed_scanner()
{
    /* Initialise the uncompressed table for the maximum number of states and
     * the acceptable input character range. */
    uncompressed = new int[MAX_STATES][INPUT_RANGE];
    memset(uncompressed, 0, sizeof(int) * MAX_STATES * INPUT_RANGE);
    max_state = 1;
}

/*
 * Generate the uncompressed scanner. The final state of each pattern is set to
 * how many times 'next' has been run. Ideally, this is the index to an array
 * of some sorts.
 */
void
generate_uncompressed_scanner(std::function<const char*()> next)
{
    /* Process each pattern */
    const char* pattern;
    const char* cptr;
    int processed = 0;
    for (int i = 0; (pattern = cptr = next()) != nullptr; ++i) {
        ++processed;
        int state = START_STATE;
        while (*cptr != '\0') {
            if (*(cptr + 1) == '\0') {
                /* End-state points to the (negative) pattern index */
                if (uncompressed[state][*cptr] > 0) {
                    std::cerr << "Error: patterns are non-deterministic!\n"
                              << "Trying to add '" << pattern
                              << "' (index=" << processed << ")\n";
                    ;
                    exit(1);
                } else {
                    if (uncompressed[state][*cptr] != 0 &&
                        uncompressed[state][*cptr] != -processed) {
                        std::cout << "Warning: pattern '" << pattern
                                  << "' (index=" << processed << ") "
                                  << "already added\n";
                    }
                    uncompressed[state][*cptr] = -processed;
                }
            } else if (uncompressed[state][*cptr] != 0) {
                /* Intermediate state already existsing. */
                state = uncompressed[state][*cptr];
            } else {
                /* New intermediate state */
                uncompressed[state][*cptr] = ++max_state;
                state = max_state;
                if (state >= MAX_STATES) {
                    std::cerr << "Error: maximum states exhausted\n"
                              << "Processed " << processed << " patterns.\n"
                              << "Increase MAX_STATES and recompile\n";
                    exit(1);
                }
            }
            ++cptr;
        }
    }

    /* Show some statistics */
    if (show_statistics) {
        size_t bytes = sizeof(int) * INPUT_RANGE * max_state;
        std::cout << "Uncompressed table:\n"
                  << "    Number of states: " << max_state << "\n"
                  << "    Table size      : " << bytes << " ("
                  << std::setprecision(2) << (bytes / (1024.0 * 1024.0))
                  << "MB)\n";
    }
}

/*
 * Test the uncompressed table with matching and mismatching input.
 */
int
test_uncompressed_scanner(std::function<const char*()> next)
{
    const char* pattern;
    const char* cptr;
    int processed = 0;
    int failures = 0;

    /* Testing known patterns */
    for (int i = 0; (pattern = cptr = next()) != nullptr; ++i) {
        ++processed;
        int state = 1;
        while (*cptr != '\0' && state > 0) {
            uint8_t byte = *cptr;
            state = uncompressed[state][*cptr++];
        }
        if (state < 0) {
            if (processed != -state) {
                std::cout << "Warning: f " << pattern << " (index " << processed
                          << "): "
                          << "Wrong result: ["
                          << " Expected: " << processed << " Found: " << -state
                          << " ]" << std::endl;
                ++failures;
            }
        } else {
            std::cerr << "Error: testing scanning of " << pattern << " (index "
                      << processed << "): "
                      << "Not found\n";
            ++failures;
        }
    }

    /* Test unknown patterns */
    const char* mismatches[] = { "&A;", "&AELIG;", "&amp", /* "" */
                                 nullptr };
    for (int i = 0; (pattern = cptr = mismatches[i]) != nullptr; ++i) {
        ++processed;
        int state = 1;
        while (*cptr != '\0' && state > 0) {
            state = uncompressed[state][*cptr++];
        }
        if (state < 0) {
            std::cerr << "Error: pattern " << pattern << " "
                      << "should have returned no-match: "
                      << "Wrong result: ["
                      << " Expected: " << 0 << " Found: " << -state << " ]"
                      << std::endl;
            ++failures;
        }
    }

    /* If any failures, bail out */
    if (failures) {
        std::cerr << "Error: Test failed\n";
    }
    return failures;
}

/*
 * Print the compressed table as a C declaration.
 */
void
print_uncompressed_scanner()
{
    int width = 5;
    std::cout << "/* clang-format off */\n\n"
              << "/*\n"
              << " * DO NOT MODIFY!\n"
              << " * Generated by the entities_scanner_generator. This table\n"
              << " * is used by the generated entities scanner and relies on\n"
              << " * fixed table indices.\n"
              << " */\n";
    std::cout << "int uncompressed[][] = {\n";
    std::cout << "  /*";
    for (int c = 0; c < INPUT_RANGE; ++c) {
        if (c % 16 == 0)
            std::cout << std::endl;
        std::cout << std::setw(width) << std::right
                  << (char)(isprint(c) ? c : ' ') << ", ";
    }
    std::cout << "\n  */\n";
    std::cout << "  /*";
    for (int c = 0; c < INPUT_RANGE; ++c) {
        if (c % 16 == 0)
            std::cout << std::endl;
        std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << (int)(c) << ", ";
    }
    std::cout << "\n  */\n";
    for (int s = 0; s < max_state + 1; ++s) {
        for (int c = 0; c < INPUT_RANGE; ++c) {
            if (c % 16 == 0)
                std::cout << std::endl;
            std::cout << std::setw(width) << std::right << std::setfill(' ')
                      << std::dec << uncompressed[s][c] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "};" << std::endl;
}

/*
 * Initialise the compressed scanner
 */
void
initialise_compressed_scanner()
{
    /* Initialise the compressed table for the maximum number of states and
     * the acceptable input character range. */
    compressed = new int[max_state * INPUT_RANGE];
    memset(compressed, 0, sizeof(int) * max_state * INPUT_RANGE);
    checks = new int[max_state * INPUT_RANGE];
    shifts = new int[max_state];
}

/*
 * Generate the compressed scanner.
 */
void
generate_compressed_scanner()
{
    /* Compress the table and generate the shifts table. */
    max_index = 0;
    for (int state = 0; state < max_state + 1; ++state) {
        /* Get the index on left and right on first non-zero entry */
        int l, r;
        for (l = 0; l < INPUT_RANGE && uncompressed[state][l] == 0; ++l)
            ;
        for (r = INPUT_RANGE - 1; r > l && uncompressed[state][r] == 0; --r)
            ;

        /* Save the shift state */
        shifts[state] = max_index - l;

        /* Copy all non-zero entries */
        for (int j = l; j <= r; ++j) {
            compressed[max_index] = uncompressed[state][j];
            checks[max_index] = state;
            ++max_index;
        }
    }

    /* Show some statistics */
    if (show_statistics) {
        size_t bytes = sizeof(int) * max_index;
        std::cout << "Compressed table:\n"
                  << "    Table size      : " << bytes << " ("
                  << std::setprecision(2) << (bytes / (1024.0 * 1024.0))
                  << "MB)\n";
        std::cout << "Checks table:\n"
                  << "    Table size      : " << bytes << " ("
                  << std::setprecision(2) << (bytes / (1024.0 * 1024.0))
                  << "MB)\n";
        bytes = sizeof(int) * max_state;
        std::cout << "Shifts table:\n"
                  << "    Table size      : " << bytes << " ("
                  << std::setprecision(2) << (bytes / (1024.0 * 1024.0))
                  << "MB)\n";
    }
}

/*
 * Test the compressed table with matching and mismatching input.
 */
int
test_compressed_scanner(std::function<const char*()> next)
{
    const char* pattern;
    const char* cptr;
    int processed = 0;
    int failures = 0;

    /* Testing known patterns */
    for (int i = 0; (pattern = cptr = next()) != nullptr; ++i) {
        ++processed;
        int state = 1;
        while (*cptr != '\0' && state > 0) {
            if (checks[shifts[state] + *cptr] == state) {
                state = compressed[shifts[state] + *cptr++];
            } else {
                state = 0;
            }
        }
        if (state < 0) {
            if (processed != -state) {
                std::cerr << "Error testing scanning of " << pattern
                          << " (index " << processed << "): "
                          << "Wrong result: ["
                          << " Expected: " << processed << " Found: " << -state
                          << " ]" << std::endl;
                ++failures;
            }
        } else {
            std::cerr << "Error testing scanning of " << pattern << " (index "
                      << processed << "): "
                      << "Not found\n";
            ++failures;
        }
    }
    /* Test unknown patterns */
    const char* mismatches[] = { "&A;", "&AELIG;", "&amp", /* "" */
                                 nullptr };
    for (int i = 0; (pattern = cptr = mismatches[i]) != nullptr; ++i) {
        ++processed;
        int state = 1;
        while (*cptr != '\0' && state > 0) {
            if (checks[shifts[state] + *cptr] == state) {
                state = compressed[shifts[state] + *cptr++];
            } else {
                state = 0;
            }
        }
        if (state < 0) {
            std::cerr << "Error: pattern " << pattern << " "
                      << "should have returned no-match: "
                      << "Wrong result: ["
                      << " Expected: " << 0 << " Found: " << -state << " ]"
                      << std::endl;
            ++failures;
        }
    }

    /* If any failures, bail out */
    if (failures) {
        std::cerr << "Error: Test failed\n";
    }
    return failures;
}

/* Trace a pattern scan */
void
trace_pattern_scan(int index)
{
    /* Get the pattern entity */
    int idx;
    for (idx = 0; entities[idx].named != nullptr && idx < trace; ++idx)
        ;
    if (idx != trace) {
        std::cerr << "Pattern index " << index << " not found.\n";
        return;
    }
    struct entity_t entity = entities[index];

    /* Run it through the compressed table */
    int state = 1;
    const char* cptr = (add_named ? entity.named : entity.utf8);
    std::cout << "Trace of " << cptr << " : [" << state << "]";
    while (*cptr != '\0' && state > 0) {
        if (checks[shifts[state] + *cptr] == state) {
            state = compressed[shifts[state] + *cptr];
        } else {
            state = 0;
        }
        // uint8_t byte = *cptr;
        std::cout << " " << std::hex << std::showbase << std::setfill('0')
                  << std::setw(2) << (int)((uint8_t)*cptr) << " -> " << std::dec
                  << "[" << state << "]";
        ++cptr;
    }
    std::cout << std::endl;
}

/*
 * Print the compressed table, shifts and checks table as a C declaration.
 */
void
print_compressed_scanner()
{
    int width = 5;
    std::cout << "/* clang-format off */\n\n"
              << "/*\n"
              << " * DO NOT MODIFY!\n"
              << " * Generated by the entities_scanner_generator. This table\n"
              << " * is used by the generated entities scanner and relies on\n"
              << " * fixed table indices.\n"
              << " */\n";
    std::cout << "int compressed[] = {";
    for (int j = 0; j < max_index; ++j) {
        if (j % 16 == 0)
            std::cout << std::endl;
        std::cout << std::setw(width) << std::right << compressed[j] << ", ";
    }
    std::cout << std::endl << "};" << std::endl;
    std::cout << "int checks[] = {";
    for (int j = 0; j < max_index; ++j) {
        if (j % 16 == 0)
            std::cout << std::endl;
        std::cout << std::setw(width) << std::right << checks[j] << ", ";
    }
    std::cout << std::endl << "};" << std::endl;
    std::cout << "int shifts[] = {";
    for (int j = 0; j < max_state + 1; ++j) {
        if (j % 16 == 0)
            std::cout << std::endl;
        std::cout << std::setw(width) << std::right << shifts[j] << ", ";
    }
    std::cout << std::endl << "};" << std::endl;
}

/*
 * Convert a UTF-16 codepoint (normal and supplementary planes) to UTF-8.
 */
void
convert(uint16_t high, uint16_t low, char* utf8)
{
    if ((low & 0xFF80) == 0x0000) {
        /* One byte UTF-8 */
        *(utf8++) = low & 0x007F;
    } else if ((low & 0xF800) == 0x0000) {
        /* Two byte UTF-8 */
        *(utf8++) = 0xC0 | (low >> 6);
        *(utf8++) = 0x80 | (low & 0x003F);
    } else if (low < 0xDC00 || low > 0xDFFF) {
        /* Three byte UTF-8 */
        *(utf8++) = 0xE0 | (low >> 12);
        *(utf8++) = 0x80 | ((low & 0x0FFF) >> 6);
        *(utf8++) = 0x80 | (low & 0x003F);
    } else {
        /* Four byte UTF-8 */
        high = high & 0x03FF | 0x0040;
        low = low & 0x03FF;
        *(utf8++) = 0xF0 | (high >> 8);
        *(utf8++) = 0x80 | ((high & 0x00FF) >> 2);
        *(utf8++) = 0x80 | ((high & 0x0003) << 4) | (low >> 6);
        *(utf8++) = 0x80 | (low & 0x003F);
    }
}

/*
 * Convert UTF-16 hexadecimal entities codepoint (as a string) to UTF-8.
 */
void
generate_utf8_codepoints()
{
    for (int e = 0; entities[e].utf16 != nullptr; ++e) {
        // Convert the UTF-16 character string to high/low UTF-16 numbers.
        uint16_t high = 0x0000;
        uint16_t low = 0x0000;
        ;
        uint8_t shift = 12;
        const char* cptr = entities[e].utf16;
        char* utf8 = entities[e].utf8;
        while (*cptr != '\0') {
            if (*cptr >= 'A')
                low |= ((*cptr - 'A') + 10) << shift;
            else
                low |= (*cptr - '0') << shift;
            if (shift == 0) {
                if (*(cptr + 1) == '\0') {
                    convert(high, low, (char*)utf8);
                } else {
                    if (low >= 0xD800) {
                        // Supplementary plane, single codeoint. Read the
                        // second part.
                        high = low;
                        shift = 12;
                        low = 0x0000;
                    } else {
                        // Two separate codepoints. Output the current code-
                        // point and continue.
                        convert(0x0000, low, (char*)utf8);
                        while (*utf8 != '\0')
                            ++utf8;
                        shift = 12;
                        low = 0x0000;
                    }
                }
            } else {
                shift -= 4;
            }
            ++cptr;
        }
    }
}

/*
 * Print the entities structure as a C declaration.
 */
void
print_entities()
{
    std::cout << "/* clang-format off */\n\n"
              << "#include \"entities.h\"\n\n"
              << "/*\n"
              << " * DO NOT MODIFY!\n"
              << " * Generated by the entities_scanner_generator. This table\n"
              << " * is used by the generated entities scanner and relies on\n"
              << " * fixed table indices.\n"
              << " */\n";
    std::cout << "const struct entity_t entities[] =\n"
              << "{\n";
    for (int e = 0; entities[e].utf16 != nullptr; ++e) {
        std::cout << "   \"" << std::left << entities[e].named << "\","
                  << std::setw(34 - strlen(entities[e].named))
                  << std::setfill(' ') << ' ' << "\"";
        for (int i = 0; i < 9; ++i) {
            uint8_t byte = entities[e].utf8[i];
            if (byte != '\0') {
                std::cout << "\\x" << std::hex << std::setw(2)
                          << std::setfill('0') << std::uppercase << (int)byte;
            }
        }
        std::cout << "\"," << std::setw(25 - 4 * strlen(entities[e].utf8))
                  << std::setfill(' ') << ' ';
        std::cout << "/* ";
        if (strcmp(entities[e].utf8, "\x09") == 0 ||
            strcmp(entities[e].utf8, "\x0A") == 0) {
            std::cout << "   */" << std::endl;
        } else {
            std::cout << entities[e].utf8 << "  */" << std::endl;
        }
    }
    std::cout << "};" << std::dec << std::endl;
}

/*
 * Display the help
 */
void
help()
{
    std::cout << "Entities Scanner Generator\n"
              << "Options:\n"
              << "    -n, --named          Add named entity patterns\n"
              << "    -e, --entities       Show the entities table\n"
              << "    -u, --uncompressed   Show the uncompressed table\n"
              << "    -c, --compressed     Show the compressed table\n"
              << "    -s, --statistics     Display table statistics\n"
              << "    -t IDX, --trace IDX  Trace a scan of a pattern index\n";
}

/*
 * Main
 */
int
main(int argc, char** argv)
{
    /* Command line processing */
    while (--argc && *(++argv)[0] == '-') {
        if (strcmp(*argv, "-n") == 0 || strcmp(*argv, "--named") == 0) {
            add_named = true;
        } else if (strcmp(*argv, "--utf8") == 0) {
            add_utf8 = true;
        } else if (strcmp(*argv, "-e") == 0 ||
                   strcmp(*argv, "--entities") == 0) {
            show_entities = true;
        } else if (strcmp(*argv, "-u") == 0 ||
                   strcmp(*argv, "--uncompressed") == 0) {
            show_uncompressed = true;
        } else if (strcmp(*argv, "-c") == 0 ||
                   strcmp(*argv, "--compressed") == 0) {
            show_compressed = true;
        } else if (strcmp(*argv, "-s") == 0 ||
                   strcmp(*argv, "--statistics") == 0) {
            show_statistics = true;
        } else if (strcmp(*argv, "-t") == 0 || strcmp(*argv, "--trace") == 0) {
            --argc, ++argv;
            if (argc <= 0) {
                std::cerr << "Trace option requires an argument\n";
                help();
                exit(1);
            }
            trace = atoi(*argv);
        } else if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0) {
            help();
            return 0;
        } else {
            help();
            return 1;
        }
    }

    /* Generate UTF8 entitiy codepoints */
    generate_utf8_codepoints();
    if (show_entities) {
        print_entities();
    }

    /* If no patterns defined, bail out */
    if (add_named == false && add_utf8 == false) {
        std::cerr << "No patterns specified\n";
        return 0;
    }

    /* Initialise the uncompressed scanner */
    initialise_uncompressed_scanner();

    /* Add all patterns */
    if (add_named) {
        int e = 0;
        generate_uncompressed_scanner([&e]() { return entities[e++].named; });
        e = 0;
        test_uncompressed_scanner([&e]() { return entities[e++].named; });
    }
    if (add_utf8) {
        int e = 0;
        generate_uncompressed_scanner([&e]() {
            if (entities[e].named == nullptr) {
                return (const char*)nullptr;
            }
            return (const char*)entities[e++].utf8;
        });
        e = 0;
        test_uncompressed_scanner([&e]() {
            if (entities[e].named == nullptr) {
                return (const char*)nullptr;
            }
            return (const char*)entities[e++].utf8;
        });
    }

    /* Print the uncompressed table, if asked to. */
    if (show_uncompressed) {
        print_uncompressed_scanner();
    }

    /* Initialise the compressed scanner */
    initialise_compressed_scanner();

    /* Generate the compressed scanner */
    generate_compressed_scanner();

    /* Test the compressed scanner */
    if (add_named) {
        int e = 0;
        test_compressed_scanner([&e]() { return entities[e++].named; });
    }
    if (add_utf8) {
        int e = 0;
        test_compressed_scanner([&e]() {
            if (entities[e].named == nullptr) {
                return (const char*)nullptr;
            }
            return (const char*)entities[e++].utf8;
        });
    }

    /* Print the compressed table, if asked to. */
    if (show_compressed) {
        print_compressed_scanner();
    }

    /* Trace a scan of a pattern. */
    if (trace >= 0) {
        trace_pattern_scan(trace);
    }

    return 0;
}
